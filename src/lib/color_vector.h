#pragma once

#include <string>
#include <vector>

namespace palette {

class Color;

class ColorVector {
 public:
    ColorVector();
    explicit ColorVector(const std::vector<Color> &colors);
    explicit ColorVector(std::vector<Color> &&colors);
    ColorVector(const ColorVector &other);
    ColorVector(ColorVector &&other);

    ColorVector &operator=(const ColorVector &other);

    std::vector<Color> &get();
    const std::vector<Color> &get() const;

    std::string to_string(const std::string &delimiter) const;

 private:
    std::vector<Color> colors_;
};
}  // namespace palette
