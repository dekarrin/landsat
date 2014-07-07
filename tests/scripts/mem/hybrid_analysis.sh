#/bin/sh

. tests/test_functions.sh

test_memory_output valgrind --leak-check=yes $bindir/landsat -Hq tests/red.tif tests/nir.tif
