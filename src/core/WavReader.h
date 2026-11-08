// SPDX-License-Identifier: MIT
// Liest WAV-Dateien (RIFF/RF64, PCM 8/16/24/32 Bit, IEEE-Float 32/64 Bit,
// WAVE_FORMAT_EXTENSIBLE) und liefert die Samples als interleaved int16.
#pragma once

#include "File.h"

#include <cstdint>
#include <string>
#include <vector>

namespace mbsc {

struct WavInfo {
    uint16_t formatTag = 0;       // 1 = PCM, 3 = IEEE-Float
    uint16_t channels = 0;
    uint32_t sampleRate = 0;
    uint16_t bitsPerSample = 0;   // laut Header
    uint16_t bytesPerSample = 0;  // Containergröße pro Sample
    uint16_t blockAlign = 0;      // Bytes pro Frame (alle Kanäle)
    uint64_t dataOffset = 0;
    uint64_t dataBytes = 0;
    uint64_t totalFrames = 0;

    double durationSeconds() const {
        return sampleRate ? static_cast<double>(totalFrames) / sampleRate : 0.0;
    }
    bool isFloat() const { return formatTag == 3; }
};

class WavReader {
public:
    // Öffnet die Datei und parst den Header. Bei Fehlern: false + Meldung (deutsch).
    bool open(const std::filesystem::path& path, std::string& error);

    const WavInfo& info() const { return info_; }

    // Anzahl der Ausgabekanäle: 1 (Mono) oder 2 (Stereo). Bei mehr als 2 Kanälen
    // werden nur die ersten beiden verwendet.
    int outChannels() const { return info_.channels >= 2 ? 2 : 1; }

    // Liest bis zu maxFrames Frames, konvertiert nach int16 (interleaved,
    // outChannels() Kanäle). Rückgabe: tatsächlich gelesene Frames (0 = Ende).
    size_t readFrames(int16_t* out, size_t maxFrames);

private:
    File file_;
    WavInfo info_;
    uint64_t framesRead_ = 0;
    std::vector<uint8_t> raw_;
};

} // namespace mbsc
