gcc -pthread ./count-then-move.c ./concurrent.c ./data-gen.c ./utils.c ./partitioning.c -o ./partitioning.exe
# Add '-Wall -g3 -fsanitize=address' for address debugging