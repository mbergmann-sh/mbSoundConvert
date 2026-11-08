// SPDX-License-Identifier: MIT
// Selbsttest für WavReader + Converter (ohne Qt). Erzeugt WAV-Dateien in
// verschiedenen Formaten, konvertiert sie und prüft die MP3-Frame-Header.
#define _USE_MATH_DEFINES
#include "core/Converter.h"

#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;
static int g_failures = 0;

#define CHECK(cond, msg)                                                          \
    do {                                                                          \
        if (!(cond)) {                                                            \
            std::cerr << "  FEHLER: " << msg << "  [" #cond "]  (Zeile " << __LINE__ << ")\n"; \
            ++g_failures;                                                         \
        }                                                                         \
    } while (0)

static void w16(std::ofstream& f, uint16_t v) { f.put(char(v & 0xFF)); f.put(char(v >> 8)); }
static void w32(std::ofstream& f, uint32_t v) { w16(f, v & 0xFFFF); w16(f, v >> 16); }

// format: 1 = PCM, 3 = Float; bytes = Bytes pro Sample
static void writeWav(const fs::path& p, int channels, int rate, int format, int bytes, double seconds,
                     bool extensible = false) {
    std::ofstream f(p, std::ios::binary);
    const uint32_t frames = uint32_t(seconds * rate);
    const uint32_t dataBytes = frames * channels * bytes;
    const uint32_t fmtSize = extensible ? 40 : 16;
    f.write("RIFF", 4); w32(f, 4 + 8 + fmtSize + 8 + dataBytes); f.write("WAVE", 4);
    f.write("fmt ", 4); w32(f, fmtSize);
    w16(f, extensible ? 0xFFFE : format); w16(f, channels); w32(f, rate);
    w32(f, rate * channels * bytes); w16(f, channels * bytes); w16(f, bytes * 8);
    if (extensible) {
        w16(f, 22); w16(f, bytes * 8); w32(f, 3);
        w16(f, format);  // SubFormat-GUID (erste 2 Byte = Format)
        const unsigned char tail[14] = {0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};
        f.write(reinterpret_cast<const char*>(tail), 14);
    }
    f.write("data", 4); w32(f, dataBytes);
    for (uint32_t i = 0; i < frames; ++i) {
        const double t = double(i) / rate;
        for (int c = 0; c < channels; ++c) {
            const double x = 0.5 * std::sin(2 * M_PI * (440.0 + 110.0 * c) * t);
            if (format == 3 && bytes == 4) { float v = float(x); f.write(reinterpret_cast<char*>(&v), 4); }
            else if (format == 3 && bytes == 8) { f.write(reinterpret_cast<const char*>(&x), 8); }
            else if (bytes == 1) { f.put(char(uint8_t(128 + x * 127))); }
            else if (bytes == 2) { w16(f, uint16_t(int16_t(x * 32767))); }
            else if (bytes == 3) { int32_t v = int32_t(x * 8388607); f.put(char(v & 0xFF)); f.put(char((v >> 8) & 0xFF)); f.put(char((v >> 16) & 0xFF)); }
            else { w32(f, uint32_t(int32_t(x * 2147483647.0))); }
        }
    }
}

struct Mp3Head { bool ok = false; int version = 0, layer = 0, bitrateIdx = 0, srIdx = 0, channelMode = 0; std::string tag; };

static Mp3Head parseHead(const fs::path& p) {
    std::ifstream f(p, std::ios::binary);
    std::vector<unsigned char> b(256);
    f.read(reinterpret_cast<char*>(b.data()), 256);
    Mp3Head h;
    if (f.gcount() < 64 || b[0] != 0xFF || (b[1] & 0xE0) != 0xE0) return h;
    h.version = (b[1] >> 3) & 3; h.layer = (b[1] >> 1) & 3;
    h.bitrateIdx = b[2] >> 4; h.srIdx = (b[2] >> 2) & 3; h.channelMode = b[3] >> 6;
    const bool mpeg1 = h.version == 3;
    const bool mono = h.channelMode == 3;
    const size_t off = 4 + (mpeg1 ? (mono ? 17 : 32) : (mono ? 9 : 17));
    h.tag.assign(reinterpret_cast<char*>(&b[off]), 4);
    h.ok = true;
    return h;
}

int main() {
    const fs::path dir = fs::temp_directory_path() / "mbsc_test";
    fs::create_directories(dir);
    std::atomic<bool> noCancel{false};
    std::cout << "LAME " << mbsc::lameVersion() << "\n";

    {   std::cout << "[1] 16 Bit / 44,1 kHz / Stereo, CBR 320 (Voreinstellung)\n";
        auto in = dir / "a.wav", out = dir / "a.mp3";
        writeWav(in, 2, 44100, 1, 2, 2.0);
        int lastPct = -1; bool monotone = true;
        auto r = mbsc::convertWavToMp3(in, out, {}, noCancel, [&](int p) { if (p < lastPct) monotone = false; lastPct = p; });
        CHECK(r.ok, r.error);
        auto h = parseHead(out);
        CHECK(h.ok && h.version == 3 && h.layer == 1, "MPEG-1 Layer III");
        CHECK(h.bitrateIdx == 14, "Bitrate-Index 14 = 320 kbit/s");
        CHECK(h.srIdx == 0, "44,1 kHz");
        CHECK(h.tag == "Info", "Info-Tag (CBR) vorhanden, gefunden: " + h.tag);
        CHECK(fs::exists(out) && fs::file_size(out) > 76000 && fs::file_size(out) < 90000, "Dateigröße ~80 kB");
        CHECK(lastPct == 100 && monotone, "Fortschritt monoton bis 100 %");
    }
    {   std::cout << "[2] VBR V2, Mono, 24 Bit / 22,05 kHz, WAVE_FORMAT_EXTENSIBLE\n";
        auto in = dir / "b.wav", out = dir / "b.mp3";
        writeWav(in, 1, 22050, 1, 3, 1.5, true);
        mbsc::EncoderSettings s; s.mode = mbsc::RateMode::Vbr; s.vbrQuality = 2;
        auto r = mbsc::convertWavToMp3(in, out, s, noCancel);
        CHECK(r.ok, r.error);
        auto h = parseHead(out);
        CHECK(h.ok && h.layer == 1, "Layer III");
        CHECK(h.channelMode == 3, "Mono");
        CHECK(h.tag == "Xing", "Xing-Tag (VBR), gefunden: " + h.tag);
    }
    {   std::cout << "[3] Float32 / 96 kHz / Stereo (LAME resampelt), CBR 192\n";
        auto in = dir / "c.wav", out = dir / "c.mp3";
        writeWav(in, 2, 96000, 3, 4, 1.0);
        mbsc::EncoderSettings s; s.cbrKbps = 192;
        auto r = mbsc::convertWavToMp3(in, out, s, noCancel);
        CHECK(r.ok, r.error);
        auto h = parseHead(out);
        CHECK(h.ok && h.layer == 1, "gültiger MP3-Frame");
    }
    {   std::cout << "[4] 8 Bit, 32 Bit int, Float64\n";
        struct { int fmt, bytes; const char* n; } cases[] = {{1, 1, "d8"}, {1, 4, "d32"}, {3, 8, "d64"}};
        for (auto& c : cases) {
            auto in = dir / (std::string(c.n) + ".wav"), out = dir / (std::string(c.n) + ".mp3");
            writeWav(in, 2, 48000, c.fmt, c.bytes, 0.5);
            auto r = mbsc::convertWavToMp3(in, out, {}, noCancel);
            CHECK(r.ok, std::string(c.n) + ": " + r.error);
            CHECK(parseHead(out).ok, std::string(c.n) + ": MP3-Header");
        }
    }
    {   std::cout << "[5] Ungültige Datei\n";
        auto in = dir / "bad.wav", out = dir / "bad.mp3";
        { std::ofstream f(in, std::ios::binary); f << "das ist keine wav-datei, nur text, mehr als zwoelf bytes"; }
        auto r = mbsc::convertWavToMp3(in, out, {}, noCancel);
        CHECK(!r.ok && !r.error.empty(), "Fehler erwartet");
        CHECK(!fs::exists(out), "keine Ausgabedatei bei Fehler");
        auto r2 = mbsc::convertWavToMp3(dir / "gibts_nicht.wav", out, {}, noCancel);
        CHECK(!r2.ok, "fehlende Datei -> Fehler");
    }
    {   std::cout << "[6] Abbruch\n";
        auto in = dir / "e.wav", out = dir / "e.mp3";
        writeWav(in, 2, 44100, 1, 2, 1.0);
        std::atomic<bool> cancel{true};
        auto r = mbsc::convertWavToMp3(in, out, {}, cancel);
        CHECK(!r.ok && r.cancelled, "abgebrochen gemeldet");
        CHECK(!fs::exists(out), "Teil-Datei wurde gelöscht");
    }
    {   std::cout << "[7] Unicode-Dateiname\n";
        auto in = dir / u8"Walgesang äöü ♪.wav", out = dir / u8"Walgesang äöü ♪.mp3";
        writeWav(in, 2, 44100, 1, 2, 0.3);
        auto r = mbsc::convertWavToMp3(in, out, {}, noCancel);
        CHECK(r.ok, r.error);
    }

    fs::remove_all(dir);
    if (g_failures) { std::cerr << g_failures << " Prüfung(en) fehlgeschlagen\n"; return 1; }
    std::cout << "Alle Tests bestanden.\n";
    return 0;
}
