NAME=$1
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -S $NAME.c -o $NAME.s
if [ $? -eq 0 ]; then
    echo "Compilation successful! Output file: $NAME.s"
else
    echo "Compilation failed!"
    exit 1
fi