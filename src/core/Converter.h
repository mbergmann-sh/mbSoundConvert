// SPDX-License-Identifier: MIT
// WAV -> MP3 mit LAME. Reiner C++17-Kern ohne Qt-Abhängigkeit.
#pragma once

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>

namespace mbsc {

enum class RateMode { Cbr, Vbr };

struct EncoderSettings {
    RateMode mode = RateMode::Cbr;
    int cbrKbps = 320;        // nur bei Cbr: 32 … 320
    int vbrQuality = 2;       // nur bei Vbr: 0 (beste) … 9 (kleinste)
    int algorithmQuality = 2; // LAME -q: 0 (beste/langsamste) … 9 (schnellste)
};

struct ConvertResult {
    bool ok = false;
    bool cancelled = false;
    std::string error;        // deutsche Fehlermeldung, wenn !ok
    uint64_t outBytes = 0;
};

using ProgressFn = std::function<void(int percent)>;

// Konvertiert `in` nach `out`. `cancel` wird regelmäßig geprüft. Bei Fehler oder
// Abbruch wird die (Teil-)Ausgabedatei wieder gelöscht.
ConvertResult convertWavToMp3(const std::filesystem::path& in,
                              const std::filesystem::path& out,
                              const EncoderSettings& settings,
                              const std::atomic<bool>& cancel,
                              const ProgressFn& progress = {});

// Versionsstring der eingebundenen LAME-Bibliothek, z. B. "3.100".
std::string lameVersion();

} // namespace mbsc
