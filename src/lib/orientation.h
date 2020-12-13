#pragma once

#include <string>

namespace palette {

class Orientation {
 public:
    enum class Value { vertical, horizontal, unknown };

    explicit Orientation(Value value);
    explicit Orientation(const std::string &value_str);
    Orientation(const Orientation &other);

    Value get() const;
    bool valid() const;
    std::string to_string() const;

    static Value value_from_string(const std::string &value_str);
    static std::string value_to_string(const Value value);

 private:
    Value value_;
};
}  // namespace palette
