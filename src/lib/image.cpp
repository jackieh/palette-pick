#include "lib/image.h"

#include <algorithm>
#include <utility>
#include <vector>

#include <Magick++.h>

#include "lib/color.h"
#include "lib/color_k_means.h"
#include "lib/image_get_sample_colors_mode.h"

namespace palette {
namespace {

struct HslRangeProperties final {
 public:
    HslRangeProperties(double min_saturation, double max_saturation,
                       double min_lightness, double max_lightness);
    explicit HslRangeProperties(const std::vector<Color> &colors);

    double min_saturation_;
    double max_saturation_;
    double min_lightness_;
    double max_lightness_;
};

std::vector<Color> get_filtered_colors(const std::vector<Color> &colors,
                                       const HslRangeProperties &properties);

std::vector<Color> get_bright_colors(const std::vector<Color> &colors);

std::vector<Color> get_saturated_colors(const std::vector<Color> &colors);

std::vector<Color> get_hue_spread_colors(int num_colors);
}  // namespace

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

std::vector<Color> Image::get_sample_colors(
    size_t num_colors, ImageGetSampleColorsMode mode, bool &success) const {
    success = false;
    std::vector<Color> sample_colors;
    switch (mode.get_value()) {
        case ImageGetSampleColorsMode::Value::quantize: {
            Image quantized_image(image_);
            quantized_image.get().quantizeColors(num_colors);
            quantized_image.get().quantize();
            sample_colors = quantized_image.get_unique_colors();
            success = true;
            break;
        }
        case ImageGetSampleColorsMode::Value::kmeans_random_spread:
            success = ColorKMeans::find_clusters(
                num_colors, ColorKMeans::SeedMode::random_spread,
                get_unique_colors(), sample_colors);
            break;
        case ImageGetSampleColorsMode::Value::kmeans_static_spread:
            success = ColorKMeans::find_clusters(
                num_colors, ColorKMeans::SeedMode::static_spread,
                get_unique_colors(), sample_colors);
            break;
        case ImageGetSampleColorsMode::Value::kmeans_hue_spread:
            sample_colors = get_hue_spread_colors(num_colors);
            success = ColorKMeans::find_clusters(
                num_colors, ColorKMeans::SeedMode::keep_existing,
                get_unique_colors(), sample_colors);
            break;
        case ImageGetSampleColorsMode::Value::kmeans_bright_hue_spread: {
            const std::vector<Color> unique_colors = get_unique_colors();
            std::vector<Color> data_colors = get_bright_colors(unique_colors);
            sample_colors = get_hue_spread_colors(num_colors);
            success = ColorKMeans::find_clusters(
                num_colors, ColorKMeans::SeedMode::keep_existing,
                data_colors, sample_colors);
            break;
        }
        case ImageGetSampleColorsMode::Value::kmeans_saturated_hue_spread: {
            const std::vector<Color> unique_colors = get_unique_colors();
            std::vector<Color> data_colors =
                get_saturated_colors(unique_colors);
            sample_colors = get_hue_spread_colors(num_colors);
            success = ColorKMeans::find_clusters(
                num_colors, ColorKMeans::SeedMode::keep_existing,
                data_colors, sample_colors);
            break;
        }
        default: break;
    }
    return sample_colors;
}

namespace {

HslRangeProperties::HslRangeProperties(
    double min_saturation, double max_saturation,
    double min_lightness, double max_lightness) :
    min_saturation_(min_saturation),
    max_saturation_(max_saturation),
    min_lightness_(min_lightness),
    max_lightness_(max_lightness) { }

HslRangeProperties::HslRangeProperties(const std::vector<Color> &colors) :
    min_saturation_(0.0),
    max_saturation_(0.0),
    min_lightness_(0.0),
    max_lightness_(0.0) {
    if (!colors.empty()) {
        min_saturation_ = 1.0;
        min_lightness_ = 1.0;
    }
    for (const Color &color : colors) {
        Magick::ColorHSL color_hsl(color.get());
        min_saturation_ = std::min(min_saturation_, color_hsl.saturation());
        max_saturation_ = std::max(max_saturation_, color_hsl.saturation());
        min_lightness_ = std::min(min_lightness_, color_hsl.lightness());
        max_lightness_ = std::max(max_lightness_, color_hsl.lightness());
    }
}

std::vector<Color> get_filtered_colors(const std::vector<Color> &colors,
                                       const HslRangeProperties &properties) {
    auto inside_range = [properties](const Color &c) {
        Magick::ColorHSL c_hsl(c.get());
        return ((c_hsl.saturation() >= properties.min_saturation_)
                && (c_hsl.saturation() <= properties.max_saturation_)
                && (c_hsl.lightness() >= properties.min_lightness_)
                && (c_hsl.lightness() <= properties.max_lightness_));
    };
    std::vector<Color> filtered_colors;
    std::copy_if(colors.begin(), colors.end(),
                 std::back_inserter(filtered_colors), inside_range);
    return filtered_colors;
}

std::vector<Color> get_bright_colors(const std::vector<Color> &colors) {
    HslRangeProperties total_range(colors);
    const double min_s = total_range.min_saturation_;
    const double max_s = total_range.max_saturation_;
    const double min_l = total_range.min_lightness_;
    const double max_l = total_range.max_lightness_;
    HslRangeProperties bright_range(
        std::min(0.2, min_s + ((max_s - min_s) * 0.2)), 1.0,
        std::min(0.3, min_l + ((max_l - min_l) * 0.3)), 1.0);
    return get_filtered_colors(colors, bright_range);
}

std::vector<Color> get_saturated_colors(const std::vector<Color> &colors) {
    HslRangeProperties total_range(colors);
    const double min_s = total_range.min_saturation_;
    const double max_s = total_range.max_saturation_;
    const double min_l = total_range.min_lightness_;
    const double max_l = total_range.max_lightness_;
    HslRangeProperties saturated_range(
        std::min(0.5, min_s + ((max_s - min_s) * 0.5)), 1.0,
        std::min(0.1, min_l + ((max_l - min_l) * 0.1)),
        std::max(0.9, min_l + ((max_l - min_l) * 0.9)));
    return get_filtered_colors(colors, saturated_range);
}

std::vector<Color> get_hue_spread_colors(int num_colors) {
    std::vector<Color> hue_spread_colors;
    hue_spread_colors.reserve(num_colors);
    double hue_increment = (1.0 / num_colors);
    for (int i = 0; i < num_colors; ++i) {
        double hue_value = hue_increment * i;
        Magick::ColorHSL hue_spread_color(
            static_cast<Magick::Quantum>(hue_value),
            static_cast<Magick::Quantum>(1.0),
            static_cast<Magick::Quantum>(0.5));
        hue_spread_colors.emplace_back(Magick::Color(hue_spread_color));
    }
    return hue_spread_colors;
}
}  // namespace
}  // namespace palette
