#!/bin/bash
# use "source" to execute this script
#src_dir=/home/wesleyteng/codeBase/osd_cmpr/cmpr_canvas
src_dir=./
#out_dir=/home/wesleyteng/codeBase/osd_cmpr
out_dir=./
#exe_dir=/home/wesleyteng/codeBase/osd_cmpr/cmpr_canvas/build/main
exe_dir=./sample_osdc
obj_csv=draw_obj_list.csv
width=1280
height=720
format=0  #0:OSD_ARGB8888, 1:OSD_ARGB1555, 2:OSD_ARGB4444, 3:OSD_LUT8, else: not support
wrt_bs=1
wrt_draw=1
echo $obj_csv

$exe_dir -w $width -h $height -format $format -in_dir ${src_dir} -out_dir ${out_dir} \
-f $obj_csv -wrt_bs $wrt_bs -wrt_draw $wrt_draw

