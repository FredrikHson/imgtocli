#!/bin/bash
filename=$1

if [[ ! -f "$filename" ]]; then
    echo $1 file not found
    exit 1
fi

width=$(identify -ping -format '%w' $filename)
height=$(identify -ping -format '%h' $filename)
columns=$(tput cols)
lines=$(tput lines)
lines=$((lines*2))

new_height=$height
new_width=$width
if [[ $width -ge $columns ]]; then
    new_width=$columns
    new_height=$(( $height*$new_width/$width ))
fi
if [[ $new_height -ge $lines ]]; then
    new_height=$lines
    new_width=$(( $width*$new_height/$height ))
fi

width=new_width
height=new_height
colors=$(tput colors)
dither="Riemersma" # Can be None, FloydSteinberg or Riemersma
picture=($(convert $filename \
            -resize "${new_width}x${new_height}!" \
            -dither $dither -colors $colors \
            rgb:- | \
        hexdump -v -e '/1 "%d\n"'
))

for (( y=0; y<height/2;y++));do
    yoffset=$((y*2*width*3))
    yoffset2=$(((y*2+1)*width*3))
    for (( x=0; x<width;x++));do

        red=${picture[yoffset+x*3]}
        green=${picture[yoffset+x*3+1]}
        blue=${picture[yoffset+x*3+2]}

        echo -en "\x1b[48;2;${red};${green};${blue};38;2;${red};${green};${blue}m▄"
    done
    echo
done
echo -en "\x1b[0m"
