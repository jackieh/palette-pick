#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

#include <Magick++.h>

#include "color.h"

namespace palette {

Color::Color() : color_() { }

Color::Color(const Magick::Color &color) : color_(color) { }

Color::Color(Magick::Color &&color) : color_(std::move(color)) { }

Color::Color(const Color &other) : color_(other.color_) { }

Color::Color(Color &&other) : color_(std::move(other.color_)) { }

Color &Color::operator=(const Color &other) {
    color_ = other.color_;
    return *this;
}

bool Color::operator==(const Color &other) const {
    return color_ == other.color_;
}

bool Color::operator!=(const Color &other) const {
    return color_ != other.color_;
}

bool Color::operator<(const Color &other) const {
    return color_ < other.color_;
}

bool Color::lessThanRgb(const Color &left, const Color &right) {
    Magick::ColorRGB left_rgb(left.color_);
    Magick::ColorRGB right_rgb(right.color_);
    if (left_rgb.red() == right_rgb.red()) {
        if (left_rgb.green() == right_rgb.green()) {
            return (left_rgb.blue() < right_rgb.blue());
        }
        return (left_rgb.green() < right_rgb.green());
    }
    return (left_rgb.red() < right_rgb.red());
}

bool Color::lessThanHsl(const Color &left, const Color &right) {
    Magick::ColorHSL left_hsl(left.color_);
    Magick::ColorHSL right_hsl(right.color_);
    if (left_hsl.hue() == right_hsl.hue()) {
        if (left_hsl.saturation() == right_hsl.saturation()) {
            return (left_hsl.lightness() < right_hsl.lightness());
        }
        return (left_hsl.saturation() < right_hsl.saturation());
    }
    return (left_hsl.hue() < right_hsl.hue());
}

Magick::Color &Color::get() { return color_; }

const Magick::Color &Color::get() const { return color_; }

std::string Color::to_string() const {
    std::ostringstream hex_stringstream;
    hex_stringstream << std::setfill('0') << std::hex << std::uppercase;
    hex_stringstream << "#"
        << std::setw(2) << (static_cast<int>(color_.quantumRed()) >> 8)
        << std::setw(2) << (static_cast<int>(color_.quantumGreen()) >> 8)
        << std::setw(2) << (static_cast<int>(color_.quantumBlue()) >> 8);
    return hex_stringstream.str();
}
}  // namespace palette
