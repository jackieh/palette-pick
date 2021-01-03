#bin/bash

input_file_path=$1

if [ -z $input_file_path ]
then
    echo "No input file specified"
    exit 1
fi

input_file_name=$(basename $input_file_path)
input_file_base_name=${input_file_name%%.*}
input_file_extension=${input_file_name##*.}

output_directory_path=./ignored/images/stripes/$input_file_base_name
mkdir -p $output_directory_path

copy_file_name=$input_file_base_name-original.$input_file_extension
copy_file_path=$output_directory_path/$copy_file_name
cp $input_file_path $copy_file_path

for i in {1..8}; do
    n=$((4 * $i))

    printf "Creating posterized image with $n colors per channel..."
    posterize_file_name=$input_file_base_name-posterized-$n.png
    posterize_file_path=$output_directory_path/$posterize_file_name
    start_time=`date +%s`
    convert $input_file_path -colorspace HSL -posterize $n $posterize_file_path
    end_time=`date +%s`
    printf " done in `expr $end_time - $start_time` seconds\n"

    printf "Creating quantized image with $n colors..."
    quantize_file_name=$input_file_base_name-quantized-$n.png
    quantize_file_path=$output_directory_path/$quantize_file_name
    start_time=`date +%s`
    convert $input_file_path -colors $n $quantize_file_path
    end_time=`date +%s`
    printf " done in `expr $end_time - $start_time` seconds\n"

    printf "Creating stripes images for $n colors using kmeans..."
    start_time=`date +%s`
    stripes_file_name=$n-colors-stripes-kmeans-from-original.png
    stripes_file_path=$output_directory_path/$stripes_file_name
    ./build/getcolors -m kmeans -n $n -I $input_file_path \
        | ./build/mkstripes -O $stripes_file_path
    stripes_file_name=$n-colors-stripes-kmeans-from-posterized.png
    stripes_file_path=$output_directory_path/$stripes_file_name
    ./build/getcolors -m kmeans -n $n -I $posterize_file_path \
        | ./build/mkstripes -O $stripes_file_path
    end_time=`date +%s`
    printf " done in `expr $end_time - $start_time` seconds\n"

    for d in {1..8}; do
        printf "Creating stripes images for $n colors using quantization with tree depth $d..."
        start_time=`date +%s`
        stripes_file_name=$n-colors-stripes-quantized-from-original-tree-depth-$d.png
        stripes_file_path=$output_directory_path/$stripes_file_name
        ./build/getcolors -m quantize -n $n -d $d -I $input_file_path \
            | ./build/mkstripes -O $stripes_file_path
        stripes_file_name=$n-colors-stripes-quantized-from-posterized-tree-depth-$d.png
        stripes_file_path=$output_directory_path/$stripes_file_name
        ./build/getcolors -m quantize -n $n -d $d -I $posterize_file_path \
            | ./build/mkstripes -O $stripes_file_path
        end_time=`date +%s`
        printf " done in `expr $end_time - $start_time` seconds\n"
    done;
done
