#include <sstream>
#include <string>

#include <Magick++.h>

#include "color_collection.h"
#include "orientation.h"
#include "stripes_image.h"

namespace palette {

StripesImage::StripesImage(int stripe_length,
                           int stripe_width,
                           const Orientation &stripe_orientation) :
    stripe_length_(stripe_length),
    stripe_width_(stripe_width),
    stripe_orientation_(stripe_orientation),
    stripe_colors_() { }

ColorCollection &StripesImage::get_colors() { return stripe_colors_; }

// Return whether or not export was successful.
bool StripesImage::export_image(const std::string file_name,
                                std::stringstream &export_stream,
                                std::stringstream &error_stream) {
    if (stripe_colors_.empty()) {
        error_stream << "Empty list of colors" << std::endl;
        return false;
    }

    if (stripe_length_ <= 0) {
        error_stream << "Image was configured with stripe length "
            << stripe_length_ << " which is not a positive integer; "
            << "it must be a positive integer" << std::endl;
        return false;
    }

    if (stripe_width_ <= 0) {
        error_stream << "Image was configured with stripe width "
            << stripe_width_ << " which is not a positive integer; "
            << "it must be a positive integer" << std::endl;
        return false;
    }

    int image_width;
    int image_height;
    switch (stripe_orientation_.get()) {
        case Orientation::Value::vertical:
            image_width = stripe_width_ * stripe_colors_.size();
            image_height = stripe_length_;
            break;
        case Orientation::Value::horizontal:
            image_width = stripe_length_;
            image_height = stripe_width_ * stripe_colors_.size();
            break;
        default:
            error_stream << "Unknown stripes orientation" << std::endl;
            return false;
    }

    // Initialize image as a canvas with the first color in the collection
    // of stripe colors.
    const Magick::Geometry canvas_size(image_width, image_height);
    const Magick::Color canvas_color(*stripe_colors_.cbegin());
    Magick::Image stripes(canvas_size, canvas_color);

    int stripe_idx = 0;
    for (const auto &stripe_color : stripe_colors_) {
        stripes.fillColor(stripe_color);
        int stripe_width_begin = stripe_idx * stripe_width_;
        switch (stripe_orientation_.get()) {
            case Orientation::Value::vertical:
                stripes.draw(Magick::DrawableRectangle(
                        stripe_width_begin, 0,
                        image_width, image_height));
                break;
            case Orientation::Value::horizontal:
                stripes.draw(Magick::DrawableRectangle(
                        0, stripe_width_begin,
                        image_width, image_height));
                break;
            default:
                error_stream << "Unknown stripes orientation" << std::endl;
                return false;
        }
        ++stripe_idx;
    }

    try {
        stripes.write(file_name);
    } catch (Magick::Exception &error) {
        error_stream << "ImageMagick exception: " << error.what() << std::endl;
        return false;
    }
    export_stream << "Wrote " << std::dec << image_width << "x" << image_height
        << " image to " << file_name << " with colors: "
        << stripe_colors_.to_string(", ") << std::endl;
    return true;
}
}  // namespace palette
