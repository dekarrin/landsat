#!/bin/sh

. tests/test_functions.sh

test_memory_output valgrind --leak-check=yes $bindir/landsat tests/red.tif tests/nir.tif -q
