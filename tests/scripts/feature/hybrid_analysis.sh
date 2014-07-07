#/bin/sh

. tests/test_functions.sh

test_feature_output $bindir/landsat -q -H tests/red.tif tests/nir.tif
