#!/bin/sh
# Original LUNA code: Danny Nunez (dnunezx) 2026
set -eu

make clean
make all copy LUNA_EMULATOR_HOST_IGR=1
