#!/bin/sh

# tests program execution

if test ! -f tests/small_input_check.txt
then
	echo "Error: missing test file 'tests/small_input_check.txt'"
	return 2
fi

$srcdir/src/landsat tests/red.tif tests/nir.tif -q > tests/small_input_test.txt
status=0
if test "x$(diff tests/small_input_test.txt tests/small_input_check.txt)" != x
then
	status=1
fi
rm tests/small_input_test.txt
return $status
