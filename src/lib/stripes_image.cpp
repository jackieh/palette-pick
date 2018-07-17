#include <assert.h>
#include <sstream>
#include <string>

#include <Magick++.h>

#include <color_collection.h>
#include <stripes_image.h>

namespace palette {

    StripesImage::Orientation::Orientation() {
        orientation_ = Orientation::OValue::VERTICAL;
        repr_ = "vertical";
    }

    StripesImage::Orientation::Orientation(std::string &orientation_str) {
        // Store the input string so we can display it later if it's invalid.
        repr_ = std::string(orientation_str);
        if (orientation_str.compare("vertical") == 0) {
            orientation_ = Orientation::OValue::VERTICAL;
        } else if (orientation_str.compare("horizontal") == 0) {
            orientation_ = Orientation::OValue::HORIZONTAL;
        } else {
            orientation_ = Orientation::OValue::UNKNOWN;
        }
    }

    StripesImage::Orientation::Orientation(StripesImage::Orientation::OValue
                                           orientation_val)
    {
        orientation_ = orientation_val;
        switch (orientation_val) {
            case Orientation::OValue::VERTICAL: repr_ = "vertical"; break;
            case Orientation::OValue::HORIZONTAL: repr_ = "horizontal"; break;
            default: repr_ = "unknown"; break;
        }
    }

    StripesImage::Orientation::OValue StripesImage::Orientation::get() const {
        return orientation_;
    }

    bool StripesImage::Orientation::is_valid() const {
        switch (orientation_) {
            case Orientation::OValue::VERTICAL: return true;
            case Orientation::OValue::HORIZONTAL: return true;
            default: return false;
        }
    }

    std::string StripesImage::Orientation::to_string() const {
        return repr_;
    }

    StripesImage::StripesImage() {
        stripe_orientation_ = Orientation(Orientation::OValue::VERTICAL);
        stripe_length_ = 0;
        stripe_width_ = 100;
        default_stripe_length_ = true;
    }

    void StripesImage::insert_color(Magick::Color color) {
        size_t prev_size = stripe_colors_.size();
        stripe_colors_.insert(color);
        size_t curr_size = stripe_colors_.size();
        if (prev_size != curr_size) {
            if (default_stripe_length_) {
                stripe_length_ = stripe_width_ * curr_size;
            }
        }
    }

    void StripesImage::set_orientation(StripesImage::Orientation
                                       stripe_orientation)
    {
        stripe_orientation_ = stripe_orientation;
    }

    void StripesImage::set_length(size_t stripe_length) {
        default_stripe_length_ = false;
        stripe_length_ = stripe_length;
    }

    void StripesImage::set_width(size_t stripe_width) {
        stripe_width_ = stripe_width;
        if (default_stripe_length_) {
            stripe_length_ = stripe_width_ * stripe_colors_.size();
        }
    }

    bool StripesImage::empty() const {
        return (stripe_colors_.size() == 0);
    }

    StripesImage::ExportStatus StripesImage::export_image(
        std::string const file_name,
        std::stringstream &export_stream,
        std::stringstream &error_stream)
    {
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

        // Initialize image as a fully transparent white canvas.
        Magick::Geometry const canvas_size(image_width, image_height);
        Magick::Color const canvas_color(/* red */ 0xff, /* green */ 0xff,
                                         /* blue */ 0xff, /* alpha */ 0xff);
        Magick::Image stripes(canvas_size, canvas_color);

        int stripe_idx = 0;
        for (auto const &stripe_color : stripe_colors_) {
            stripes.fillColor(stripe_color);
            int stripe_width_begin = stripe_idx * stripe_width_;
            int stripe_width_end = ((stripe_idx + 1) * stripe_width_) - 1;
            switch (stripe_orientation_.get()) {
                case Orientation::OValue::VERTICAL:
                    stripes.draw(Magick::DrawableRectangle(
                            stripe_width_begin, 0,
                            stripe_width_end, stripe_length_));
                    break;
                case Orientation::OValue::HORIZONTAL:
                    stripes.draw(Magick::DrawableRectangle(
                            0, stripe_width_begin,
                            stripe_length_, stripe_width_end));
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
            error_stream << error.what() << std::endl;
            return ExportStatus::FAIL;
        }
        export_stream << "Wrote " << std::dec <<
            image_width << "x" << image_height
            << " image to " << file_name << " with colors: "
            << stripe_colors_.to_string(", ") << std::endl;
        return ExportStatus::SUCCESS;
    }
}
