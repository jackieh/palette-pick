#pragma once

#include <set>
#include <string>

namespace palette {

class Color;

class ColorSet {
 public:
    ColorSet();
    explicit ColorSet(const std::set<Color> &colors);
    explicit ColorSet(std::set<Color> &&colors);
    ColorSet(const ColorSet &other);
    ColorSet(ColorSet &&other);

    ColorSet &operator=(const ColorSet &other);

    std::set<Color> &get();
    const std::set<Color> &get() const;

    std::string to_string(const std::string &delimiter) const;

 private:
    std::set<Color> colors_;
};
}  // namespace palette
