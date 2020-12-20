#include <utility>
#include <vector>

#include <Magick++.h>

#include "color.h"
#include "image.h"

namespace palette {

Image::Image() : image_() { }

Image::Image(const Magick::Image &image) : image_(image) { }

Image::Image(Magick::Image &&image) : image_(std::move(image)) { }

Image::Image(const Image &other) : image_(other.image_) { }

Image::Image(Image &&other) : image_(std::move(other.image_)) { }

Image &Image::operator=(const Image &other) {
    image_ = other.image_;
    return *this;
}

Magick::Image &Image::get() { return image_; }

const Magick::Image &Image::get() const { return image_; }

std::vector<Color> Image::get_colors() const {
    const Magick::Geometry image_size = image_.size();
    std::vector<Color> colors;
    colors.reserve(image_size.width() * image_size.height());
    std::vector<std::pair<Magick::Color, size_t>> color_histogram;
    color_histogram.reserve(image_.totalColors());
    Magick::colorHistogram(&color_histogram, image_);
    for (const auto &histogram_elem : color_histogram) {
        for (size_t c = 0; c < histogram_elem.second; ++c) {
            colors.emplace_back(histogram_elem.first);
        }
    }
    return colors;
}

std::vector<Color> Image::get_unique_colors() const {
    std::vector<Color> unique_colors;
    unique_colors.reserve(image_.totalColors());
    std::vector<std::pair<Magick::Color, size_t>> color_histogram;
    color_histogram.reserve(image_.totalColors());
    Magick::colorHistogram(&color_histogram, image_);
    for (const auto &histogram_elem : color_histogram) {
        unique_colors.emplace_back(histogram_elem.first);
    }
    return unique_colors;
}
}  // namespace palette
