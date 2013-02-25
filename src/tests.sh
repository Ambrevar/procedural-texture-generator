#!/bin/sh
./procedural-textures ../data/wood.ptx && ls -l *bmp && \
    sha1sum res_new/result_RGB_smooth.bmp result_RGB_smooth.bmp && \
    sha1sum res_new/result_alt_smooth.bmp result_alt_smooth.bmp
