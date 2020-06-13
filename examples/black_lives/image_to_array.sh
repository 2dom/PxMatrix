#!/bin/bash
rm *.rgb
for i in *.gif;
do
base_name=$(basename "$i" .gif)
#ffmpeg -i $i -vf 'lutyuv=y=gammaval(1.4)' -vcodec rawvideo -pix_fmt rgb24 $base_name.rgb
#ffmpeg -i $i -vf 'lutyuv=y=gammaval(1.4)' -vcodec rawvideo -pix_fmt rgb565 $base_name.rgb
ffmpeg -i $i -vf 'scale=64:32,lutyuv=y=gammaval(1.4)' -vcodec rawvideo -f rawvideo -pix_fmt rgb24 $base_name.rgb

#echo $base_name;

done
