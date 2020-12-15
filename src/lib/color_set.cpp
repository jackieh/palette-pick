#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <utility>

#include "color.h"
#include "color_set.h"

namespace palette {

ColorSet::ColorSet() : colors_() { }

ColorSet::ColorSet(const std::set<Color> &colors) :
    colors_(colors.begin(), colors.end()) { }

ColorSet::ColorSet(std::set<Color> &&colors) :
    colors_(std::move(colors)) { }

ColorSet::ColorSet(const ColorSet &other) :
    colors_(other.colors_.begin(), other.colors_.end()) { }

ColorSet::ColorSet(ColorSet &&other) :
    colors_(std::move(other.colors_)) { }

ColorSet &ColorSet::operator=(const ColorSet &other) {
    colors_ = other.colors_;
    return *this;
}

std::set<Color> &ColorSet::get() { return colors_; }

const std::set<Color> &ColorSet::get() const {
    return colors_;
}

std::string ColorSet::to_string(const std::string &delimiter) const {
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
