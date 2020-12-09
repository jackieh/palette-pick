#include <assert.h>
#include <sstream>
#include <string>

#include <Magick++.h>

#include "color_collection.h"
#include "stripes_image.h"

namespace palette {

    StripesImage::Orientation::Orientation(StripesImage::Orientation::OValue
                                           orientation_val) :
        orientation_(orientation_val),
        repr_(std::string())
    {
        switch (orientation_val) {
            case Orientation::OValue::VERTICAL: repr_ = "vertical"; break;
            case Orientation::OValue::HORIZONTAL: repr_ = "horizontal"; break;
            default: repr_ = "unknown"; break;
        }
    }

    StripesImage::Orientation::Orientation(const std::string &orientation_str) :
        orientation_(Orientation::OValue::UNKNOWN),
        // Store the input string so we can display it later if it's invalid.
        repr_(std::string(orientation_str))
    {
        if (orientation_str.compare("vertical") == 0) {
            orientation_ = Orientation::OValue::VERTICAL;
        } else if (orientation_str.compare("horizontal") == 0) {
            orientation_ = Orientation::OValue::HORIZONTAL;
        }
    }

    StripesImage::Orientation::OValue StripesImage::Orientation::get() const {
        return orientation_;
    }

    std::string StripesImage::Orientation::to_string() const {
        return repr_;
    }

    StripesImage::StripesImage() :
        stripe_width_(0),
        stripe_length_(0),
        stripe_orientation_(StripesImage::Orientation::UNKNOWN),
        stripe_colors_(ColorCollection())
    { }

    void StripesImage::insert_color(Magick::Color color) {
        stripe_colors_.insert(color);
    }

    void StripesImage::set_length(int stripe_length) {
        stripe_length_ = stripe_length;
    }

    void StripesImage::set_width(int stripe_width) {
        stripe_width_ = stripe_width;
    }

    void StripesImage::set_orientation(StripesImage::Orientation
                                       stripe_orientation)
    {
        stripe_orientation_ = stripe_orientation;
    }

    bool StripesImage::empty() const {
        return (stripe_colors_.size() == 0);
    }

    size_t StripesImage::size() const {
        return stripe_colors_.size();
    }

    StripesImage::ExportStatus StripesImage::export_image(
        const std::string file_name,
        std::stringstream &export_stream,
        std::stringstream &error_stream)
    {
        if (stripe_length_ <= 0) {
            error_stream << "Image was configured with stripe length "
                << stripe_length_ << " which is not a positive integer; "
                << "it must be a positive integer" << std::endl;
            return ExportStatus::FAIL;
        }

        if (stripe_width_ <= 0) {
            error_stream << "Image was configured with stripe width "
                << stripe_width_ << " which is not a positive integer; "
                << "it must be a positive integer" << std::endl;
            return ExportStatus::FAIL;
        }

        int image_width;
        int image_height;
        switch (stripe_orientation_.get()) {
            case Orientation::OValue::VERTICAL:
                image_width = stripe_width_ * stripe_colors_.size();
                image_height = stripe_length_;
                break;
            case Orientation::OValue::HORIZONTAL:
                image_width = stripe_length_;
                image_height = stripe_width_ * stripe_colors_.size();
                break;
            default:
                error_stream << "\"" << stripe_orientation_.to_string()
                    << "\" is not a valid orientation; it must be either "
                    << "\"vertical\" or \"horizontal\"" << std::endl;
                return ExportStatus::FAIL;
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
                case Orientation::OValue::VERTICAL:
                    stripes.draw(Magick::DrawableRectangle(
                            stripe_width_begin, 0,
                            image_width, image_height));
                    break;
                case Orientation::OValue::HORIZONTAL:
                    stripes.draw(Magick::DrawableRectangle(
                            0, stripe_width_begin,
                            image_width, image_height));
                    break;
                default:
                    // This variable should have already been checked for
                    // validity and should not have changed since the check.
                    assert(0);
            }
            ++stripe_idx;
        }

        try {
            stripes.write(file_name);
        } catch (Magick::Exception &error) {
            error_stream << "ImageMagick exception: "
                << error.what() << std::endl;
            return ExportStatus::FAIL;
        }
        export_stream << "Wrote " << std::dec <<
            image_width << "x" << image_height
            << " image to " << file_name << " with colors: "
            << stripe_colors_.to_string(", ") << std::endl;
        return ExportStatus::SUCCESS;
    }
} // namespace palette
