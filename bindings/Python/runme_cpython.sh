#!/bin/sh
readonly BASE_DIR=`readlink -f \`dirname "$0"\``
env PYTHONPATH="$BASE_DIR/library" python3 "$BASE_DIR/start.py"

