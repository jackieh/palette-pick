Palette Pick
============

Color picker and color scheme manager

### Current state

This repository currently contains the following:

- A minimal C++ library that is mostly a wrapper around ImageMagick's C++
  library.
- Command-line tool `mkstripes`, which generates an image with stripes of
  specified colors.
- Command-line tool `getcolors`, which prints a list of hex colors from a
  specified image file, optionally after colors in the image are reduced via
  quantization.

### Reasons for creating Palette Pick

I created this project because I wanted to use Linux desktop software for
management of color palettes that had the features I wanted. At the time I
began developing this project I actively searched for such software and found
the existing alternatives did not satisfy my needs. Long story short
[Gpick](https://github.com/thezbyg/gpick) is one of the most feature-rich
previously existing software tools approaching the ideal I was able to find,
but I found its user interface clumsy and limited and I found a few features
either lacking or difficult to find.

Ideally the project will end up with a C++ library, a collection of
command-line tools, and a GUI tool that covers various features relevant to
color picking, generating color schemes, and generally manipulating collections
of colors in various formats.

Ideally some of the featues of this software will include things such as:

- Import an image from which to generate a color palette, with control over
  which parts of the image or colors are prioritized and best-fitting the
  palette to a palette configuration (such as an analogous palette, or a split
  complementary palette).

- Import an arbitrary text file with color hex values, such as a color scheme
  configuration file (e.g. Vim color scheme, window manager theme colors),
  display the colors and infer their names from their associated keywords in
  the configuration file.

- Export a color palette to a configuration file (e.g. a Vim color scheme file
  or a window manager theme file), or export as a pretty-printed list of hex
  colors, or export as a color-indexed .png file for which the colors and their
  names can be easily referenced in an image viewer.

- Generate a color scheme of various configurations with fine-grained control
  over number of colors generated.

### Dependencies

- [ImageMagick](http://www.imagemagick.org)

### Building and testing

To build the C++ library run `make lib`.

To build command-line tools run `make tools`.

GUI software is not yet implemented.

Testing is not yet implemented.

### Documentation

Documentation beyond code comments is not yet arranged.
