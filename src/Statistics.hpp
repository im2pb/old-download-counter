#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>

namespace old_download_counter {
    inline std::string formatCount(std::int64_t value) {
        auto text = std::to_string(value);
        auto const firstDigit = text.front() == '-' ? 1u : 0u;

        for (auto pos = text.size(); pos > firstDigit + 3;) {
            pos -= 3;
            text.insert(pos, 1, ',');
        }
        
        return text;
    }

    inline std::string formatRating(int value) {
        auto const wide = static_cast<std::int64_t>(value);
        return formatCount(wide < 0 ? -wide : wide);
    }

    struct RowLayout {
        std::array<float, 6> left;
        float scale;
    };

    inline RowLayout layoutRow(std::array<float, 6> const& widths, float available, float pairGap, float groupGap) {
        float width = pairGap * 3 + groupGap * 2;
        for (auto item : widths) width += item;

        RowLayout result {{}, std::min(1.f, std::max(0.f, available) / width)};

        float cursor = 0.f;

        for (std::size_t i = 0; i < widths.size(); ++i) {
            result.left[i] = cursor * result.scale;
            cursor += widths[i] + (i % 2 == 0 ? pairGap : groupGap);
        }

        return result;
    }
}
