#!/bin/bash

OUTPUT_DIR=codegen-output
TARGET=$(basename $1)
OUTPUT_FILE=${TARGET%.*}

mkdir -p $OUTPUT_DIR

./bin/mongac $1 > $OUTPUT_DIR/$TARGET.llvm
llc $OUTPUT_DIR/$TARGET.llvm
as -o $OUTPUT_DIR/$TARGET.o $OUTPUT_DIR/$TARGET.llvm.s

gcc -c runtime.c -o $OUTPUT_DIR/runtime.o
gcc -o $OUTPUT_FILE $OUTPUT_DIR/runtime.o $OUTPUT_DIR/$TARGET.o

rm -rf $OUTPUT_DIR
