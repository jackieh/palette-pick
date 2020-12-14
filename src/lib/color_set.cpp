#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <utility>

#include "color.h"
#include "color_collection.h"

namespace palette {

ColorCollection::ColorCollection() : colors_() { }

ColorCollection::ColorCollection(const std::set<Color> &colors) :
    colors_(colors.begin(), colors.end()) { }

ColorCollection::ColorCollection(std::set<Color> &&colors) :
    colors_(std::move(colors)) { }

ColorCollection::ColorCollection(const ColorCollection &other) :
    colors_(other.colors_.begin(), other.colors_.end()) { }

ColorCollection::ColorCollection(ColorCollection &&other) :
    colors_(std::move(other.colors_)) { }

ColorCollection &ColorCollection::operator=(const ColorCollection &other) {
    colors_ = other.colors_;
    return *this;
}

std::set<Color> &ColorCollection::get() { return colors_; }

const std::set<Color> &ColorCollection::get() const {
    return colors_;
}

std::string ColorCollection::to_string(const std::string &delimiter) const {
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
