#ifndef LIBRARY_STRIPES_IMAGE_H
#define LIBRARY_STRIPES_IMAGE_H

#include <sstream>
#include <string>

#include <Magick++.h>

#include <color_collection.h>

namespace palette {

    class StripesImage {
    public:
        enum ExportStatus { SUCCESS, FAIL };

        class Orientation {
        public:
            enum OValue { VERTICAL, HORIZONTAL, UNKNOWN };

            Orientation();
            Orientation(std::string &orientation_str);
            Orientation(OValue orientation_val);
            OValue get() const;
            bool is_valid() const;
            std::string to_string() const;

        private:
            OValue orientation_;
            std::string repr_;
        };

        StripesImage();

        void insert_color(Magick::Color color);
        void set_orientation(Orientation stripe_orientation);
        void set_length(size_t stripe_length);
        void set_width(size_t stripe_width);

        bool empty() const;
        ExportStatus export_image(std::string const file_name,
                                  std::stringstream &export_stream,
                                  std::stringstream &error_stream);

    private:
        Orientation stripe_orientation_;
        bool default_stripe_length_;
        size_t stripe_length_;
        size_t stripe_width_;
        std::string file_name_;
        ColorCollection stripe_colors_;
    };
}

#endif // LIBRARY_STRIPES_IMAGE_H
