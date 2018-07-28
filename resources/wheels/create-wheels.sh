#/bin/bash

# https://www.imagemagick.org/script/command-line-options.php#colorspace
# `convert -list colorspace` gives the following:
# CIELab CMY CMYK Gray HCL HCLp HSB HSI HSL HSV HWB Lab LCH LCHab LCHuv
# LinearGray LMS Log Luv OHTA Rec601YCbCr Rec709YCbCr RGB scRGB sRGB
# Transparent xyY XYZ YCbCr YDbDr YCC YIQ YPbPr YUV

# Have the channel hardcoded to a constant value.
FX_000="0.00"
FX_025="0.25"
FX_050="0.50"
FX_075="0.75"
FX_100="1.00"

# Horizontal gradient.
FX_H="i/w"

# Vertical gradient.
FX_V="(h-j)/h"

# Generate an image displaying a two-dimensional color space.
#
# Parameter 1: Name of the color space.
# Parameter 2: Length/width of the square image generated.
# Parameter 3: ImageMagick fx expression for each pixel of image channel 1.
# Parameter 4: ImageMagick fx expression for each pixel of image channel 2.
# Parameter 5: ImageMagick fx expression for each pixel of image channel 3.
# Parameter 6: String expressing which image channel is held constant and at
#              which value it is held.
run_magick_square() {
    convert \
        -size "$2"x"$2" xc: \
        -colorspace $1 \
        -channel R -fx "$3" \
        -channel G -fx "$4" \
        -channel B -fx "$5" \
        out/${1,,}/$2/${1,,}-${6,,}-$2x$2.png
}

# Generate all images each displaying a two-dimensional color spaces selected
# from subsets of the given color space (e.g. display 15 subsets of the RGB
# color space).
#
# Parameter 1: Name of the color space.
run_magick_square_colorspace() {
    echo "Creating $1 images..."
    SIZES="2 4 8 16 32 64 128 256 512 1024"
    mkdir -p out/${1,,}
    for size in $SIZES; do
        mkdir -p out/${1,,}/$size
        run_magick_square "$1" $size "$FX_000" "$FX_H" "$FX_V" "${1:0:1}000"
        run_magick_square "$1" $size "$FX_025" "$FX_H" "$FX_V" "${1:0:1}025"
        run_magick_square "$1" $size "$FX_050" "$FX_H" "$FX_V" "${1:0:1}050"
        run_magick_square "$1" $size "$FX_075" "$FX_H" "$FX_V" "${1:0:1}075"
        run_magick_square "$1" $size "$FX_100" "$FX_H" "$FX_V" "${1:0:1}100"

        run_magick_square "$1" $size "$FX_H" "$FX_000" "$FX_V" "${1:1:1}000"
        run_magick_square "$1" $size "$FX_H" "$FX_025" "$FX_V" "${1:1:1}025"
        run_magick_square "$1" $size "$FX_H" "$FX_050" "$FX_V" "${1:1:1}050"
        run_magick_square "$1" $size "$FX_H" "$FX_075" "$FX_V" "${1:1:1}075"
        run_magick_square "$1" $size "$FX_H" "$FX_100" "$FX_V" "${1:1:1}100"

        run_magick_square "$1" $size "$FX_H" "$FX_V" "$FX_000" "${1:2:1}000"
        run_magick_square "$1" $size "$FX_H" "$FX_V" "$FX_025" "${1:2:1}025"
        run_magick_square "$1" $size "$FX_H" "$FX_V" "$FX_050" "${1:2:1}050"
        run_magick_square "$1" $size "$FX_H" "$FX_V" "$FX_075" "${1:2:1}075"
        run_magick_square "$1" $size "$FX_H" "$FX_V" "$FX_100" "${1:2:1}100"
    done
}

run_magick_square_colorspace RGB
run_magick_square_colorspace HCL
run_magick_square_colorspace HSB
run_magick_square_colorspace HSI
run_magick_square_colorspace HSL
run_magick_square_colorspace HSV
run_magick_square_colorspace HWB
run_magick_square_colorspace LCH
