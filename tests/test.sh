#!/bin/sh

# tests program execution

$srcdir/landsat tests/red.tif tests/nir.tif -q > tests/test_output.txt
if test $(diff tests/test_output.txt tests/output_
