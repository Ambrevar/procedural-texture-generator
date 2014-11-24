#!/bin/sh

res="$(realpath "$0")"
res="${res%/*}/res"

root=..

sumcheck() {
	output=$(sha1sum "$1" "$2" | cut -d' ' -f1)
	sum1=${output%
*}
	sum2=${output#*
}
	if [ "$sum1" = "$sum2" ]; then
		echo "SUCCESS: ${1##*/} ${2##*/}"
	else
		echo "FAIL: ${1##*/} ${2##*/}"
	fi
}

"$root"/src/ptg "$root"/data/wood.ptx 2>/dev/null
if [ $? -eq 0 ]; then
	sumcheck "$res"/result_RGB.bmp result_RGB.bmp
	sumcheck "$res"/result_RGB_smooth.bmp result_RGB_smooth.bmp
	sumcheck "$res"/result_alt_smooth.bmp result_alt_smooth.bmp
	rm *bmp
fi
