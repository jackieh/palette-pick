#include "lib/image_get_sample_colors_mode.h"

#include <string>

namespace palette {

ImageGetSampleColorsMode::ImageGetSampleColorsMode(Value value) :
    value_(value) { }

ImageGetSampleColorsMode::ImageGetSampleColorsMode(
    const std::string &value_str) :
    value_(value_from_string(value_str)) { }

ImageGetSampleColorsMode::ImageGetSampleColorsMode(
    const ImageGetSampleColorsMode &other) :
    value_(other.value_) { }

ImageGetSampleColorsMode &ImageGetSampleColorsMode::operator=(
    const ImageGetSampleColorsMode &other) {
    value_ = other.value_;
    return *this;
}

ImageGetSampleColorsMode::Value ImageGetSampleColorsMode::get_value() const {
    return value_;
}

bool ImageGetSampleColorsMode::valid() const {
    return value_ != Value::unknown;
}

std::string ImageGetSampleColorsMode::to_string() const {
    return value_to_string(value_);
}

ImageGetSampleColorsMode::Value ImageGetSampleColorsMode::value_from_string(
    const std::string &value_str) {
    if (value_str.compare(value_to_string(Value::quantize)) == 0) {
        return Value::quantize;
    }
    if (value_str.compare(value_to_string(Value::kmeans_random_spread)) == 0) {
        return Value::kmeans_random_spread;
    }
    if (value_str.compare(value_to_string(Value::kmeans_static_spread)) == 0) {
        return Value::kmeans_static_spread;
    }
    if (value_str.compare(value_to_string(Value::kmeans_hue_spread)) == 0) {
        return Value::kmeans_hue_spread;
    }
    if (value_str.compare(value_to_string(
                Value::kmeans_bright_hue_spread)) == 0) {
        return Value::kmeans_bright_hue_spread;
    }
    if (value_str.compare(value_to_string(
                Value::kmeans_saturated_hue_spread)) == 0) {
        return Value::kmeans_saturated_hue_spread;
    }
    return Value::unknown;
}

std::string ImageGetSampleColorsMode::value_to_string(const Value value) {
    switch (value) {
        case Value::quantize:
            return "quantize";
        case Value::kmeans_random_spread:
            return "kmeans-random-spread";
        case Value::kmeans_static_spread:
            return "kmeans-static-spread";
        case Value::kmeans_hue_spread:
            return "kmeans-hue-spread";
        case Value::kmeans_bright_hue_spread:
            return "kmeans-bright-hue-spread";
        case Value::kmeans_saturated_hue_spread:
            return "kmeans-saturated-hue-spread";
        default: break;
    }
    return "unknown";
}
}  // namespace palette
