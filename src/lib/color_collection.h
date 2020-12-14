#pragma once

#include <set>
#include <string>

namespace palette {

class Color;

class ColorCollection {
 public:
    ColorCollection();
    explicit ColorCollection(const std::set<Color> &colors);
    explicit ColorCollection(std::set<Color> &&colors);
    ColorCollection(const ColorCollection &other);
    ColorCollection(ColorCollection &&other);

    ColorCollection &operator=(const ColorCollection &other);

    std::set<Color> &get();
    const std::set<Color> &get() const;

    std::string to_string(const std::string &delimiter) const;

 private:
    std::set<Color> colors_;
};
}  // namespace palette
