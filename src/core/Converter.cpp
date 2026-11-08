// SPDX-License-Identifier: MIT
#include "Converter.h"

#include "File.h"
#include "WavReader.h"

#if __has_include(<lame/lame.h>)
#  include <lame/lame.h>   // System-Installation (qmake: CONFIG+=system_lame)
#else
#  include <lame.h>        // mitgelieferte LAME-4.0-Quellen (third_party/lame/include)
#endif

#include <algorithm>
#include <memory>
#include <system_error>
#include <vector>

namespace mbsc {

namespace {

struct LameDeleter {
    void operator()(lame_global_flags* p) const {
        if (p) lame_close(p);
    }
};
using LamePtr = std::unique_ptr<lame_global_flags, LameDeleter>;

constexpr size_t kChunkFrames = 16384;

} // namespace

std::string lameVersion() {
    const char* v = get_lame_version();
    return v ? v : "?";
}

ConvertResult convertWavToMp3(const std::filesystem::path& in,
                              const std::filesystem::path& out,
                              const EncoderSettings& s,
                              const std::atomic<bool>& cancel,
                              const ProgressFn& progress) {
    ConvertResult res;

    WavReader wav;
    if (!wav.open(in, res.error)) return res;
    const WavInfo& info = wav.info();
    const int ch = wav.outChannels();

    LamePtr gf(lame_init());
    if (!gf) {
        res.error = "LAME konnte nicht initialisiert werden";
        return res;
    }
    lame_t g = gf.get();
    lame_set_in_samplerate(g, static_cast<int>(info.sampleRate));
    lame_set_num_channels(g, ch);
    lame_set_mode(g, ch == 1 ? MONO : JOINT_STEREO);
    lame_set_quality(g, std::clamp(s.algorithmQuality, 0, 9));
    if (s.mode == RateMode::Cbr) {
        lame_set_VBR(g, vbr_off);
        lame_set_brate(g, std::clamp(s.cbrKbps, 8, 320));
    } else {
        lame_set_VBR(g, vbr_mtrh);
        lame_set_VBR_quality(g, static_cast<float>(std::clamp(s.vbrQuality, 0, 9)));
    }
    lame_set_bWriteVbrTag(g, 1);  // Xing/Info-Header: korrekte Dauer in allen Playern
    if (lame_init_params(g) < 0) {
        res.error = "Ungültige Encoder-Einstellungen (Abtastrate " + std::to_string(info.sampleRate) + " Hz)";
        return res;
    }

    File outFile;
    if (!outFile.openWrite(out)) {
        res.error = "Zieldatei kann nicht erstellt werden";
        return res;
    }

    auto fail = [&](const std::string& msg, bool cancelled = false) -> ConvertResult {
        outFile.close();
        std::error_code ec;
        std::filesystem::remove(out, ec);
        res.ok = false;
        res.cancelled = cancelled;
        res.error = msg;
        return res;
    };

    std::vector<int16_t> pcm(kChunkFrames * static_cast<size_t>(ch));
    std::vector<unsigned char> mp3(static_cast<size_t>(1.25 * kChunkFrames) + 7200);

    const uint64_t total = info.totalFrames;
    uint64_t done = 0;
    int lastPct = -1;
    auto report = [&](int pct) {
        if (progress && pct != lastPct) {
            lastPct = pct;
            progress(pct);
        }
    };
    report(0);

    while (true) {
        if (cancel.load(std::memory_order_relaxed)) return fail("Abgebrochen", true);

        const size_t n = wav.readFrames(pcm.data(), kChunkFrames);
        if (n == 0) break;

        const int frames = static_cast<int>(n);
        const int bytes = (ch == 2)
            ? lame_encode_buffer_interleaved(g, pcm.data(), frames, mp3.data(), static_cast<int>(mp3.size()))
            : lame_encode_buffer(g, pcm.data(), pcm.data(), frames, mp3.data(), static_cast<int>(mp3.size()));
        if (bytes < 0) return fail("LAME-Kodierfehler (Code " + std::to_string(bytes) + ")");
        if (bytes > 0 && outFile.write(mp3.data(), static_cast<size_t>(bytes)) != static_cast<size_t>(bytes))
            return fail("Schreibfehler (Datenträger voll oder Zugriff verweigert?)");

        done += n;
        const uint64_t denom = std::max<uint64_t>(total, done);
        report(static_cast<int>(std::min<uint64_t>(99, done * 100 / denom)));
    }

    const int flushed = lame_encode_flush(g, mp3.data(), static_cast<int>(mp3.size()));
    if (flushed < 0) return fail("LAME-Fehler beim Abschluss der Kodierung");
    if (flushed > 0 && outFile.write(mp3.data(), static_cast<size_t>(flushed)) != static_cast<size_t>(flushed))
        return fail("Schreibfehler (Datenträger voll oder Zugriff verweigert?)");

    const int64_t endPos = outFile.tell();

    // Info-/Xing-Frame am Dateianfang aktualisieren (enthält Dauer, Frame-Anzahl, Gain-Info).
    std::vector<unsigned char> tag(8192);
    const size_t tagSize = lame_get_lametag_frame(g, tag.data(), tag.size());
    if (tagSize > 0 && tagSize <= tag.size()) {
        if (!outFile.seek(0, SEEK_SET) || outFile.write(tag.data(), tagSize) != tagSize)
            return fail("Schreibfehler beim Aktualisieren des MP3-Headers");
    }

    if (!outFile.close()) {
        std::error_code ec;
        std::filesystem::remove(out, ec);
        res.error = "Schreibfehler beim Schließen der Zieldatei";
        return res;
    }

    report(100);
    res.ok = true;
    res.outBytes = endPos > 0 ? static_cast<uint64_t>(endPos) : 0;
    return res;
}

} // namespace mbsc
