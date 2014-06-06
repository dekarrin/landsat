#!/bin/sh

# tests program execution
test_output ()
{
	name="$1"
	shift
	execute="$1"
	shift
	if test ! -f "tests/${name}_check.txt"
	then
		echo "Error: missing test file 'tests/${name}_check.txt'"
		return 2
	fi

	"$srcdir/src/$execute" "$@" > "tests/${name}_test.txt"
	status=0
	if test "x$(diff "tests/${name}_test.txt" "tests/${name}_check.txt")" != x
	then
		status=1
	fi
	rm "tests/${name}_test.txt"
	return $status
}
