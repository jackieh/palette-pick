#include <iomanip>
#include <set>
#include <sstream>
#include <string>

#include <Magick++.h>

#include <color_collection.h>

namespace palette {

    ColorCollection::ColorCollection() :
        colors_(std::set<Magick::Color>())
    { }

    ColorCollection::ColorCollection(std::vector<Magick::Color> colors) :
        colors_(std::set<Magick::Color>())
    {
        colors_.insert(colors.begin(), colors.end());
    }

    ColorCollection::ColorCollection(ColorCollection &colors) :
        colors_(std::set<Magick::Color>())
    {
        colors_.insert(colors.begin(), colors.end());
    }

    void ColorCollection::insert(Magick::Color color) {
        colors_.insert(color);
    }

    void ColorCollection::insert(std::vector<Magick::Color> colors) {
        colors_.insert(colors.begin(), colors.end());
    }

    void ColorCollection::insert(ColorCollection colors) {
        colors_.insert(colors.begin(), colors.end());
    }

    ColorCollection::iterator ColorCollection::begin() noexcept {
        return colors_.begin();
    }

    ColorCollection::const_iterator ColorCollection::cbegin() const noexcept {
        return colors_.cbegin();
    }

    ColorCollection::iterator ColorCollection::end() noexcept {
        return colors_.end();
    }

    ColorCollection::const_iterator ColorCollection::cend() const noexcept {
        return colors_.cend();
    }

    std::string ColorCollection::to_string(std::string delimiter) const {
        std::stringstream hex_stringstream;
        hex_stringstream << std::setfill('0') << std::hex << std::uppercase;
        bool first_elem = true;
        for (const auto &color : colors_) {
            if (first_elem) {
                first_elem = false;
            } else {
                hex_stringstream << delimiter;
            }
            hex_stringstream << "#"
                << std::setw(2) << ((int)color.quantumRed() >> 8)
                << std::setw(2) << ((int)color.quantumGreen() >> 8)
                << std::setw(2) << ((int)color.quantumBlue() >> 8);
        }
        std::string hex(hex_stringstream.str());
        return hex;
    }

    bool ColorCollection::empty() const {
        return (colors_.size() == 0);
    }

    size_t ColorCollection::size() const {
        return colors_.size();
    }
}
