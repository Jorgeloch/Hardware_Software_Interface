#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Macro rand8
# define rand8 ((hrand() >> 8) & 0b111)
# define pw(p, i, n) ((i < n) ? (p[i]) : (p[i%n]*i))

// Número pseudo - aleatório
static uint32_t ri = 1;
// Função para ajuste de semente
void hsrand ( uint32_t seed ) 
{
  ri = seed ;
}
// Função de número pseudo - aleatório
uint16_t hrand () 
{
  // Calculando próximo número
  ri = (1103515245 * ri) + 12345;
  return (( ri >> 16) & 0x7FFF) ;
}

// Função de hash MAU -64
void MAU_64 (uint8_t* hash, const char* senha)
{
  // Declarando variáveis auxiliares
  uint32_t i, n = strlen(senha), nr = 256, s = 0;
  // Geração da semente a partir da senha ( sdbm )
  for (i = 0; i < 8; i++)
    hash[i] = s = pw(senha,i,n) + (s << 6) + (s << 16) - s;
  // Semente dos números pseudo - aleatórios
  hsrand (s);
  // Executando rodadas sobre os bytes do hash
  for (i = 0; i < nr; i++)
    hash[rand8] = hash[rand8] ^ hrand();
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
    printf("%02hhx", hash[i]);
  }
  printf("\n");
  free(hash);
  return 0;
}
