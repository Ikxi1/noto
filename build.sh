#!/bin/sh
set -e

./clean.sh

cd source

OUT="noto"

asm_files=$(find . -type f -name '*.asm')
c_files=$(find . -type f -name '*.c')

for src in $asm_files; do
    obj="${src%.asm}.o"
    echo "Assembling $src -> $obj"
    nasm -g -F dwarf -f elf64 -Iasm/ "$src" -o "$obj"
done

for src in $c_files; do
    obj="${src%.c}.o"
    echo "Compiling $src -> $obj"
    gcc -g -O0 -c "$src" -I../include -o "$obj"
done

obj_files=$(find . -type f -name '*.o')

echo "Linking -> $OUT"
gcc -g -O0 -no-pie -o ../"$OUT" $obj_files -lncurses

cd ..
