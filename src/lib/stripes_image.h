#pragma once

#include <sstream>
#include <string>

#include "color_vector.h"
#include "orientation.h"

namespace palette {

class StripesImage {
 public:
    StripesImage(int stripe_length,
                 int stripe_width,
                 const Orientation &stripe_orientation);

    ColorVector &get_stripe_colors();

    bool export_image(const std::string file_name,
                      std::stringstream &export_stream,
                      std::stringstream &error_stream);

 private:
    int stripe_length_;
    int stripe_width_;
    Orientation stripe_orientation_;
    ColorVector stripe_colors_;
};
}  // namespace palette
