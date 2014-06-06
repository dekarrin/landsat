#!/bin/sh

# tests program execution
test_output ()
{
	name="$1"
	shift
	execute="$1"
	shift
	if test ! -f "tests/check/${name}.txt"
	then
		echo "Error: missing test file 'tests/check/${name}.txt'"
		return 2
	fi

	"$srcdir/src/$execute" "$@" > "tests/${name}_output.txt"
	status=0
	if test "x$(diff "tests/${name}_output.txt" "tests/check/${name}.txt")" != x
	then
		status=1
	fi
	rm "tests/${name}_output.txt"
	return $status
}
