#include "lib/orientation.h"

#include <string>

namespace palette {

Orientation::Orientation(Value value) : value_(value) { }

Orientation::Orientation(const std::string &value_str) :
    value_(value_from_string(value_str)) { }

Orientation::Orientation(const Orientation &other) : value_(other.value_) { }

Orientation::Value Orientation::get() const { return value_; }

bool Orientation::valid() const { return value_ != Value::unknown; }

std::string Orientation::to_string() const { return value_to_string(value_); }

Orientation::Value Orientation::value_from_string(
    const std::string &value_str) {
    if (value_str.compare(value_to_string(Value::horizontal)) == 0) {
        return Value::horizontal;
    }
    if (value_str.compare(value_to_string(Value::vertical)) == 0) {
        return Value::vertical;
    }
    return Value::unknown;
}

std::string Orientation::value_to_string(const Value value) {
    switch (value) {
        case Orientation::Value::vertical: return "vertical";
        case Orientation::Value::horizontal: return "horizontal";
        default: break;
    }
    return "unknown";
}
}  // namespace palette
