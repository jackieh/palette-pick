#pragma once

#include <string>

namespace palette {

class ImageGetSampleColorsMode {
 public:
    enum class Value {
        quantize,
        kmeans_random_spread,
        kmeans_static_spread,
        kmeans_hue_spread,
        unknown
    };

    explicit ImageGetSampleColorsMode(Value value);
    explicit ImageGetSampleColorsMode(const std::string &value_str);
    ImageGetSampleColorsMode(const ImageGetSampleColorsMode &other);

    ImageGetSampleColorsMode &operator=(const ImageGetSampleColorsMode &other);

    Value get_value() const;
    bool valid() const;
    std::string to_string() const;

    static Value value_from_string(const std::string &value_str);
    static std::string value_to_string(const Value value);

 private:
    Value value_;
};
}  // namespace palette
