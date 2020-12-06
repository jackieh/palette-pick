#bin/bash

mkdir -p ./build/images/cat/
cp resources/photographs/cat-face.png ./build/images/cat/cat.png
for i in {1..8}; do
    for j in {1..8}; do
        k=$((4 * $i))
        printf "Creating stripes image for $k colors with tree depth $j..."
        start_time=`date +%s`
        ./build/getcolors -n $k -d $j -I resources/photographs/cat-face.png \
            | ./build/mkstripes -O \
            ./build/images/cat/cat-face-$k-colors-tree-depth-$j.png;
        end_time=`date +%s`
        printf " done in `expr $end_time - $start_time` seconds.\n"
    done;
done
