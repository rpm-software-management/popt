#!/bin/sh

if ! make distcheck; then
	echo "===== tests/testit.sh.log ====="
	cat popt-*/_build/sub/tests/testit.sh.log
	exit 1
fi
