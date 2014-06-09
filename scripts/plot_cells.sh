#!/bin/bash

# Pipe in the raw data from a -q invocation of landsat.

if [ -z "$(gnuplot --version 2>/dev/null)" ]
then
	echo "Error: gnuplot must be installed to generate graphs."
fi

DATA_FILE="landsat_plot_data.txt"

SLOPES_TITLE="Landsat Red/Near-Infrared Cell Correlation Slopes"
SLOPES_YTITLE="slope"
SLOPES_DATA_Y=2
INTERCEPTS_TITLE="Landsat Red/Near-Infrared Cell Correlation Intercepts"
INTERCEPTS_YTITLE="intercept"
INTERCEPTS_DATA_Y=3
R2S_TITLE="Landsat Red/Near-Infrared Cell Correlation R^2s"
R2S_YTITLE="R^2"
R2S_DATA_Y=4

output_file=
title=$SLOPES_TITLE
ytitle=$SLOPES_YTITLE
data_y=$SLOPES_DATA_Y

while [ "$#" -gt 0 ]
do
	case "$1" in
		-*)
			if [ "$1" = '--slopes' -o "$1" = '-s' ]
			then
				title=$SLOPES_TITLE
				ytitle=$SLOPES_YTITLE
				data_y=$SLOPES_DATA_Y
			elif [ "$1" = '--intercepts' -o "$1" = '-i' ]
			then
				title=$INTERCEPTS_TITLE
				ytitle=$INTERCEPTS_YTITLE
				data_y=$INTERCEPTS_DATA_Y
			elif [ "$1" = '--r2s' -o "$1" = '-r' ]
			then
				title=$R2S_TITLE
				ytitle=$R2S_YTITLE
				data_y=$R2S_DATA_Y
			else
				echo "Error: unknown option '$1'" >&2
				exit 1
			fi
			;;

		*)
			if [ -z "$output_file" ]
			then
				output_file="$1"
			fi
			;;
	esac
	shift
done

if [ -z "$output_file" ]
then
	echo "Error: missing output file name" >&2
	exit 1
fi

rm -rf "$DATA_FILE"
while read line
do
	x="$(echo $line | cut -d ' ' -f 1)"
	y="$(echo $line | cut -d ' ' -f $data_y)"
	echo "$x $y" >> "$DATA_FILE"
done

gnuplot <<EOF
output_file="$output_file"
data_file="$DATA_FILE"
set terminal png
set output output_file
set title "$title"
set xlabel "window size (pixels)"
set ylabel "$ytitle"
set autoscale
set offsets graph 0.025, graph 0.025, graph 0.025, graph 0.025
set logscale x 2
set border back
set tics out
plot data_file with linespoints notitle
EOF

rm -rf "$DATA_FILE"
