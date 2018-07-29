To Do
=====

## Command line tools

Consider using just getopts instead of boost program options to minimize build
dependencies.

### Sort-colors tool

Create a tool that takes a set of colors through command line options and/or
stdin and sorts them according to a specified property (e.g. sort by the amount
of red in the RGB color space in the image).

### Make-gradient tool

Create a tool that generates a discrete gradient between two given colors,
listing a specified number of RGB hex values to stdout, the first and last
being the two given colors.

### Make-grid tool

Consider creating a make-gradient-grid tool which takes a pair of colors and
generates a grid image in which the top left and bottom right cells are the two
colors and the other cells represent a subset of a color space.

Picture something like a square color of the RGB color space with the B channel
held constant at 100%, scaled down to 4x4 pixels, then scaled back up to
something like 128x128 pixels so it displays 4x4 cells representing an
approximate subset of the RGB color space.

![4x4 subset of RGB color space](http://jackieh.net/colors/rgb-blue-4x4.png)

### Make-stripes tool

Create an option to annotate stripes in exported image with hex values.

Make sure this tool eventually disables reordering of input colors and has an
option for removal of duplicate colors.

### Make-wheel tool

ImageMagick documentation claims that evaluating FX expressions is very slow.
Consider increasing speed of make-wheel by taking a 1024x1024 pixels input
image representing the color space of the wheel (e.g. an image representing the
subset of the HSB color space with a red hue), scaling it down to the specified
size, and modulating the specified color space channel (e.g. modulate the hue).
Test by comparing the generated image with an image generated directly by
ImageMagick.

Playing around with the code it appears to indeed speed up creation of a
1024x1024 image.

### Parse-colors tool

Consider having a separate tool for taking an arbitrary text file and listing
to stdout any colors mentioned in a specified format or regular expression.
Probably if no other features are included this is just a glorified wrapper
around grep and might be redundant for most people.

### Name-colors tool

Create a tool that takes colors as inputs and assigns names to them by judging
their closest color in a given dictionary. Consider using
[Pick](https://github.com/stuartlangridge/ColourPicker)'s dictionary by
default.

---

## GUI tool

Fix toolbar icons to have a more consistent look with the color dropper icon.

Arrange signals and slots.

Figure out how to create save-file and open-file and import-file dialogs.

---

## Testing

Create something that can run automated sanity testing of command line tools.
