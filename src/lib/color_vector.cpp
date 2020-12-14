#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#include "color.h"
#include "color_vector.h"

namespace palette {

ColorVector::ColorVector() : colors_() { }

ColorVector::ColorVector(const std::vector<Color> &colors) :
    colors_(colors.begin(), colors.end()) { }

ColorVector::ColorVector(std::vector<Color> &&colors) :
    colors_(std::move(colors)) { }

ColorVector::ColorVector(const ColorVector &other) :
    colors_(other.colors_.begin(), other.colors_.end()) { }

ColorVector::ColorVector(ColorVector &&other) :
    colors_(std::move(other.colors_)) { }

ColorVector &ColorVector::operator=(const ColorVector &other) {
    colors_ = other.colors_;
    return *this;
}

std::vector<Color> &ColorVector::get() { return colors_; }

const std::vector<Color> &ColorVector::get() const {
    return colors_;
}

std::string ColorVector::to_string(const std::string &delimiter) const {
    std::stringstream hex_stringstream;
    hex_stringstream << std::setfill('0') << std::hex << std::uppercase;
    bool first_elem = true;
    for (const auto &color : colors_) {
        hex_stringstream << (first_elem ? "" : delimiter) << color.to_string();
        first_elem = false;
    }
    std::string hex(hex_stringstream.str());
    return hex;
}
}  // namespace palette
