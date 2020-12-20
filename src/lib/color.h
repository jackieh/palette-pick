#pragma once

#include <string>

#include <Magick++.h>

namespace palette {

class Color {
 public:
    Color();
    explicit Color(const Magick::Color &color);
    explicit Color(Magick::Color &&color);
    Color(const Color &other);
    Color(Color &&other);

    Color &operator=(const Color &other);

    bool operator==(const Color &other) const;
    bool operator!=(const Color &other) const;
    bool operator<(const Color &other) const;

    static bool lessThanRgb(const Color &left, const Color &right);
    static bool lessThanHsl(const Color &left, const Color &right);

    Magick::Color &get();
    const Magick::Color &get() const;

    std::string to_string() const;

 private:
    Magick::Color color_;
};
}  // namespace palette
