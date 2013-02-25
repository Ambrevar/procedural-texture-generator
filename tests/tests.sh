#!/bin/sh
../src/procedural-textures ../data/wood.ptx && ls -l *bmp && \
    sha1sum ../tests_res/result_RGB.bmp result_RGB.bmp && \
    sha1sum ../tests_res/result_RGB_smooth.bmp result_RGB_smooth.bmp && \
    sha1sum ../tests_res/result_alt_smooth.bmp result_alt_smooth.bmp && \
    rm -v *bmp
