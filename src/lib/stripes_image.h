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

            Orientation(OValue orientation_val);
            Orientation(const std::string &orientation_str);
            OValue get() const;
            std::string to_string() const;

        private:
            OValue orientation_;
            std::string repr_;
        };

        StripesImage();

        void insert_color(Magick::Color color);
        void set_length(int stripe_length);
        void set_width(int stripe_width);
        void set_orientation(Orientation stripe_orientation);

        bool empty() const;
        size_t size() const;
        ExportStatus export_image(const std::string file_name,
                                  std::stringstream &export_stream,
                                  std::stringstream &error_stream);

    private:
        int stripe_width_;
        int stripe_length_;
        Orientation stripe_orientation_;
        ColorCollection stripe_colors_;
    };
}

#endif // LIBRARY_STRIPES_IMAGE_H
