tcc -pthread ./implementations/merge-sort.c  main.c  -o ./merge-tcc-bench.exe
clang -pthread ./implementations/merge-sort.c  main.c  -o ./merge-clang-bench.exe
gcc -pthread ./implementations/merge-sort.c  main.c  -o ./merge-gcc-bench.exe
#-Wall -g3 -fsanitize=address