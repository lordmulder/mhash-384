#!/bin/sh
readonly BASE_DIR=`readlink -f \`dirname "$0"\``
env PYTHONPATH="$BASE_DIR/library" pypy3 "$BASE_DIR/selftest.py"
