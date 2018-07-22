#bin/bash

mkdir -p ./build/images/cat/
cp resources/photographs/cat-face.png ./build/images/cat/cat.png
for i in {1..8}; do
    for j in {1..8}; do
        k=$((4 * $i))
        ./build/getcolors -n $k -d $j -I resources/photographs/cat-face.png \
            | ./build/mkstripes -O \
            ./build/images/cat/cat-face-$k-colors-tree-depth-$j.png;
    done;
done
