#!/bin/bash

# Generates all graphs for an analysis

if [ -z "$(gnuplot --version 2>/dev/null)" ]
then
	echo "Error: gnuplot must be installed to generate graphs."
fi

whereami ()
{
	src="${BASH_SOURCE[0]}"
	while [ -h "$src" ]
	do
		dir="$(cd -P "$(dirname "$src")" && pwd)"
		src="$(readlink "$src")"
		[[ $src != /* ]] && src="$dir/$src"
	done
	cd -P "$(dirname "$src")" && pwd
}

script_loc="$(whereami)"
usage_statement="Usage: plot_all.sh [landsat_bin] [red_tif] [near-infrared_tif]\
 [output_dir] [base_name] <args_to_landsat>"

if [ "$#" -lt 1 ]
then
	echo "Error: missing landsat binary location" >&2
	echo "$usage_statement"
	exit 1
elif [ "$#" -lt 2 ]
then
	echo "Error: missing red file name" >&2
	echo "$usage_statement"
	exit 1
elif [ "$#" -lt 3 ]
then
	echo "Error: missing near-infrared file name" >&2
	echo "$usage_statement"
	exit 1
elif [ "$#" -lt 4 ]
then
	echo "Error: missing output directory" >&2
	echo "$usage_statement"
	exit 1
elif [ "$#" -lt 5 ]
then
	echo "Error: missing output file base name" >&2
	echo "$usage_statement"
	exit 1
elif [ ! -x "$1" ]
then
	echo "Error: cannot execute '$1'" >&2
	exit 1
elif [ ! -f "$2" ]
then
	echo "Error: cannot open '$1'" >&2
	exit 1
elif [ ! -f "$3" ]
then
	echo "Error: cannot open '$2'" >&2
	exit 1
elif [ ! -x "$script_loc/plot.sh" ]
then
	echo "Error: cannot execute '$script_loc/plot.sh" >&2
	exit 1
elif [ ! -x "$script_loc/plot_cells.sh" ]
then
	echo "Error: cannot execute '$script_loc/plot_cells.sh" >&2
	exit 1
fi

landsat_bin="$1"
shift
red_file="$1"
shift
nir_file="$1"
shift
output_dir="$1"
shift
output_base="$1"
shift

mkdir -p "$output_dir"
file="$output_dir/$output_base"

analysis_start="$(date +%s)"
"$landsat_bin" -q "$red_file" "$nir_file" "$@" > "${file}_results.txt"
"$landsat_bin" -qc "$red_file" "$nir_file" "$@" > "${file}_cell_results.txt"
analysis_end="$(date +%s)"

cat "${file}_results.txt" | "$script_loc/plot.sh" -s "${file}_slopes.png"
cat "${file}_results.txt" | "$script_loc/plot.sh" -i "${file}_intercepts.png"
cat "${file}_results.txt" | "$script_loc/plot.sh" -r "${file}_r2s.png"
cat "${file}_results.txt" | \
 "$script_loc/plot.sh" -s -n "${file}_slopes_noerr.png"
cat "${file}_results.txt" | \
 "$script_loc/plot.sh" -i -n "${file}_intercepts_noerr.png"
cat "${file}_results.txt" | "$script_loc/plot.sh" -r -n "${file}_r2s_noerr.png"
cat "${file}_cell_results.txt" | \
 "$script_loc/plot_cells.sh" -s "${file}_cell_slopes.png"
cat "${file}_cell_results.txt" | \
 "$script_loc/plot_cells.sh" -i "${file}_cell_intercepts.png"
cat "${file}_cell_results.txt" | \
 "$script_loc/plot_cells.sh" -r "${file}_cell_r2s.png"

echo "Total time in analysis: $(date -ud @$(expr $analysis_end \- $analysis_start) +%T)"
