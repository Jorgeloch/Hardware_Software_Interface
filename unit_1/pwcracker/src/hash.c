#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Macro rand8
# define rand8 ((rand () >> 8) & 0b111)

void MAU_64 (uint8_t * hash, const char* senha) {
  // Declarando variáveis auxiliares
  uint32_t i, n = strlen (senha) , nr = 256 , s = 0;
  // Geração da semente a partir da senha (sdbm)
  for (i = 0; i < n; i ++)
    s = senha [i] + (s << 6) + (s << 16) - s;
  // Semente dos números pseudo - aleatórios
  srand (s) ;
  // Executando rodadas sobre os bytes do hash
  for (i = 0; i < nr; i ++)
    hash [rand8] = hash [rand8] ^ rand () ;
}

int main (int argc, char** argv)
{
  if (argc != 2)
  {
    printf("usage: %s <string to be hashed>\n", argv[0]);
  }

  uint8_t* hash = (uint8_t*) calloc(8, sizeof(uint8_t));
  MAU_64(hash, argv[1]);
  for (int i = 0; i < 8; i++)
  {
    printf("%hhx", hash[i]);
  }
  printf("\n");
  free(hash);
  return 0;
}
