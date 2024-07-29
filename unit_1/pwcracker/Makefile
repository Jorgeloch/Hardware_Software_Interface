.PHONY: all hash pwcracker

all: hash pwcracker

hash: ./src/hash.c
	gcc src/hash.c -o bin/hash

pwcracker: ./src/jorgeloch_202200014195_pwcracker.c
	gcc -Wall -g src/jorgeloch_202200014195_pwcracker.c -o bin/pwcracker

optimized: ./src/jorgeloch_202200014195_pwcracker.c
	gcc -Wall -O3 src/jorgeloch_202200014195_pwcracker.c -o bin/pwcracker
