.PHONY: all hash pwtracker

all: hash pwtracker

hash: cmd/api/main.go
	gcc src/hash.c -o bin/hash

pwtracker: src/jorgeloch_202200014195_pwtracker.c
	gcc -Wall -g src/jorgeloch_202200014195_pwtracker.c -o bin/pwtracker

