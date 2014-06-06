#/bin/sh

. tests/test_functions.sh

USING_VALGRIND=1

test_output valgrind --leak-check=yes $bindir/test
