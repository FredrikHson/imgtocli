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


pal_r=(0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 95 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 135 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 175 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 215 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 255 8 18 28 38 48 58 68 78 88 96 102 118 128 138 148 158 168 178 188 198 208 218 228 238)
pal_g=(0 0 0 0 0 0 95 95 95 95 95 95 135 135 135 135 135 135 175 175 175 175 175 175 215 215 215 215 215 215 255 255 255 255 255 255 0 0 0 0 0 0 95 95 95 95 95 95 135 135 135 135 135 135 175 175 175 175 175 175 215 215 215 215 215 215 255 255 255 255 255 255 0 0 0 0 0 0 95 95 95 95 95 95 135 135 135 135 135 135 175 175 175 175 175 175 215 215 215 215 215 215 255 255 255 255 255 255 0 0 0 0 0 0 95 95 95 95 95 95 135 135 135 135 135 135 175 175 175 175 175 175 215 215 215 215 215 215 255 255 255 255 255 255 0 0 0 0 0 0 95 95 95 95 95 95 135 135 135 135 135 135 175 175 175 175 175 175 215 215 215 215 215 215 255 255 255 255 255 255 0 0 0 0 0 0 95 95 95 95 95 95 135 135 135 135 135 135 175 175 175 175 175 175 215 215 215 215 215 215 255 255 255 255 255 255 8 18 28 38 48 58 68 78 88 96 102 118 128 138 148 158 168 178 188 198 208 218 228 238)
pal_b=(0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 0 95 135 175 215 255 8 18 28 38 48 58 68 78 88 96 102 118 128 138 148 158 168 178 188 198 208 218 228 238)
lastmatch=0
function get_color()
{

    local red=$1
    local green=$2
    local blue=$3
    local rdist=$(( ${pal_r[$lastmatch]}-$red ))
    local gdist=$(( ${pal_g[$lastmatch]}-$green ))
    local bdist=$(( ${pal_b[$lastmatch]}-$blue ))
    local distance=$(( ($rdist*$rdist+$gdist*$gdist+$bdist*bdist) ))
    local closest=$lastmatch
    if [[ $distance -le 600 ]]; then
        echo $(($closest+16))
        return
    fi
    for (( i=0;i<240;i++)); do
        local rdist=$(( ${pal_r[$i]}-$red ))
        local gdist=$(( ${pal_g[$i]}-$green ))
        local bdist=$(( ${pal_b[$i]}-$blue ))
        local tmpdist=$(( ($rdist*$rdist+$gdist*$gdist+$bdist*bdist) ))

        if [[ $tmpdist -le $distance ]]; then
            distance=$tmpdist
            closest=$i
            if [[ $distance -le 600 ]]; then
                echo $(($closest+16))
                lastmatch=$closest
                return
            fi
        fi
    done
    lastmatch=$closest
    echo $(($closest+16))
}
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
tmpfile=$(mktemp tmp.XXXX.rgb)

convert $filename -resize "${new_width}x${new_height}!" $tmpfile
width=new_width
height=new_height
hexfile=$(xxd -c 1 -ps  $tmpfile)

rm $tmpfile
for line in ${hexfile[@]}; do
    picture+=($((16#$line)))
done
pixels=width*height
for (( y=0; y<height/2;y++));do
    yoffset=$((y*2*width*3))
    yoffset2=$(((y*2+1)*width*3))
    for (( x=0; x<width;x++));do

        topcolor=$(get_color ${picture[yoffset+x*3]} ${picture[yoffset+x*3+1]} ${picture[yoffset+x*3+2]})
        bottomcolor=$(get_color ${picture[yoffset2+x*3]} ${picture[yoffset2+x*3+1]} ${picture[yoffset2+x*3+2]})
        echo -en "\033[48;5;$topcolor;38;5;${bottomcolor}m▄"
    done
    echo
done
echo -en "\033[0m"
