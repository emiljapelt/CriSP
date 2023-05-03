clang -pthread ../utilities/pcg-c-0.94/extras/entropy.o ../utilities/pcg-c-0.94/src/pcg-advance-64.o ../utilities/pcg-c-0.94/src/pcg-advance-128.o ../utilities/pcg-c-0.94/src/pcg-rngs-128.o ../utilities/pcg-c-0.94/src/pcg-rngs-64.o ../utilities/utils.c ../utilities/data-gen.c ./implementations/merge-sort.c ./implementations/general-merge-sort.c ./merge.c -o ./merge-clang.exe
gcc -pthread ../utilities/pcg-c-0.94/extras/entropy.o ../utilities/pcg-c-0.94/src/pcg-advance-64.o ../utilities/pcg-c-0.94/src/pcg-advance-128.o ../utilities/pcg-c-0.94/src/pcg-rngs-128.o ../utilities/pcg-c-0.94/src/pcg-rngs-64.o ../utilities/utils.c ../utilities/data-gen.c ./implementations/merge-sort.c ./implementations/general-merge-sort.c ./merge.c -o ./merge-gcc.exe
# tcc -pthread ../utilities/pcg-c-0.94/extras/entropy.o ../utilities/pcg-c-0.94/src/pcg-advance-64.o ../utilities/pcg-c-0.94/src/pcg-advance-128.o ../utilities/pcg-c-0.94/src/pcg-rngs-128.o ../utilities/pcg-c-0.94/src/pcg-rngs-64.o ../utilities/utils.c ../utilities/data-gen.c ./implementations/merge-sort.c ./implementations/general-merge-sort.c ./merge.c -o ./merge-cc.exe
tcc -pthread ./implementations/merge-sort.c  main.c  -o ./merge-tcc.exe
#-Wall -g3 -fsanitize=address