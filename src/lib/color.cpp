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
    Magick::ColorHSL color_hsl(color_);
    Magick::ColorHSL other_hsl(other.color_);
    if (color_hsl.hue() == other_hsl.hue()) {
        if (color_hsl.saturation() == other_hsl.saturation()) {
            return (color_hsl.lightness() < other_hsl.lightness());
        }
        return (color_hsl.saturation() < other_hsl.saturation());
    }
    return (color_hsl.hue() < other_hsl.hue());
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
