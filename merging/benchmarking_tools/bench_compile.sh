tcc -pthread ../implementations/merge-sort.c  main.c  -o ./out/merge-tcc-bench.exe
clang -pthread ../implementations/merge-sort.c  main.c  -o ./out/merge-clang-default-bench.exe
clang -O2 -pthread ../implementations/merge-sort.c  main.c  -o ./out/merge-clang-O2-bench.exe
gcc -pthread ../implementations/merge-sort.c  main.c  -o ./out/merge-gcc-default-bench.exe
gcc -O2 -pthread ../implementations/merge-sort.c  main.c  -o ./out/merge-gcc-O2-bench.exe
gcc -O3 -pthread ../implementations/merge-sort.c  main.c  -o ./out/merge-gcc-O3-bench.exe
gcc -Os -pthread ../implementations/merge-sort.c  main.c  -o ./out/merge-gcc-Os-bench.exe
#-Wall -g3 -fsanitize=address