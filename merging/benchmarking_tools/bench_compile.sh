dir="../executables/out"
if [ ! -d "$dir" ]; then
    mkdir "$dir"
fi
tcc -pthread ../implementations/merge-sort.c  main.c  -o "$dir/merge-tcc-bench.exe"
clang -pthread ../implementations/merge-sort.c  main.c  -o "$dir/merge-clang-default-bench.exe"
clang -O2 -pthread ../implementations/merge-sort.c  main.c  -o "$dir/merge-clang-O2-bench.exe"
gcc -pthread ../implementations/merge-sort.c  main.c  -o "$dir/merge-gcc-default-bench.exe"
gcc -O2 -pthread ../implementations/merge-sort.c  main.c  -o "$dir/merge-gcc-O2-bench.exe"
gcc -O3 -pthread ../implementations/merge-sort.c  main.c  -o "$dir/merge-gcc-O3-bench.exe"
gcc -Os -pthread ../implementations/merge-sort.c  main.c  -o "$dir/merge-gcc-Os-bench.exe"
#-Wall -g3 -fsanitize=address