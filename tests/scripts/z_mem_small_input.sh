#!/bin/sh

. tests/test_functions.sh

USING_VALGRIND=1
test_output valgrind --leak-check=yes $bindir/landsat tests/red.tif tests/nir.tif -q
