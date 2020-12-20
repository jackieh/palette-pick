#pragma once

#include <vector>

#include <Magick++.h>

namespace palette {

class Color;

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

 private:
    Magick::Image image_;
};
}  // namespace palette
