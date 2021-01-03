#pragma once

#include <vector>

#include <Magick++.h>

namespace palette {

class Color;
class ImageGetSampleColorsMode;

class Image {
 public:
    Image();
    explicit Image(const Magick::Image &image);
    explicit Image(Magick::Image &&image);
    Image(const Image &other);
    Image(Image &&other);

    Image &operator=(const Image &other);

    Magick::Image &get();
    const Magick::Image &get() const;

    std::vector<Color> get_colors() const;
    std::vector<Color> get_unique_colors() const;
    std::vector<Color> get_sample_colors(
        size_t num_colors, ImageGetSampleColorsMode mode, bool &success) const;

 private:
    Magick::Image image_;
};
}  // namespace palette
