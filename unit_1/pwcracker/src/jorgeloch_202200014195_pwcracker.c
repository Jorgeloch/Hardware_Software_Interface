#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
  for (i = 0; i < 8; i++) hash[i] = s = pw(senha,i,n) + (s << 6) + (s << 16) - s;
  // Semente dos números pseudo - aleatórios
  hsrand (s);
  // Executando rodadas sobre os bytes do hash
  for (i = 0; i < nr; i++)
    hash[rand8] = hash[rand8] ^ hrand();
}

typedef struct user {
  char login[9];
  uint64_t hash;
  uint32_t original_index;
  char senha[5]; 
} user;

static char possibilities[63] = "\0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

void crack_hashes(user* users, uint32_t n_users);
uint64_t hash_to_number(uint8_t *hash);
int compare_users(const void* a, const void* b);

int main (int argc, char** argv)
{
  if (argc != 3)
  {
    printf("usage: %s <input file> <output file>\n", argv[0]);
  }
  FILE *input = fopen(argv[1], "r");
  FILE *output = fopen(argv[2], "w");

  uint32_t n_users;
  fscanf(input, "%u ", &n_users);

  user *users = (user*) calloc(n_users, sizeof(user));
  for (int i = 0; i < n_users; i++)
  {
    fscanf(input, "%8[^:]:", users[i].login);
    fscanf(input, "%lx ", &users[i].hash);
    users[i].original_index = i;
  }

  qsort(users, n_users, sizeof(user), compare_users);

  crack_hashes(users, n_users);

  for (int i = 0; i < n_users; i++)
  {
    printf("login: %s\n", users[i].login);
    printf("senha: %s\n", users[i].senha);
  }

  fclose(input);
  fclose(output);
  free(users);
  return 0;
}

uint64_t hash_to_number(uint8_t* array)
{
  uint64_t result = 0;
  for (int i = 0; i < 8; i++)
  {
    result = (result << 8) + array[i];
  }
  return result;
}

int compare_users(const void* a, const void* b)
{
  user *user_a = (user*) a;
  user *user_b = (user*) b;
  if (user_a->hash > user_b->hash)
  {
    return 1;
  }
  else if (user_b->hash > user_a->hash)
  {
    return -1;
  }
  else 
  {
    return 0;
  }
}

bool search_hash(user* users, uint32_t n_users, uint64_t target, uint32_t* index)
{
  uint32_t left = 0, right = n_users,  middle;
  while (left < right)
  {
    middle = left + (right - left) / 2;
    if (users[middle].hash == target)
    {
      *index = middle;
      return true;
    }
    else if (users[middle].hash > target)
    {
      right = middle;
    }
    else {
      left = middle + 1;
    }
  }
  return false;
}

void crack_hashes(user* users, uint32_t n_users)
{
  uint32_t counter = 0;
  uint8_t buffer[8];
  for (int i = 0; i < 63; i++)
  {
    for (int j = i == 0 ? 0 : 1; j < 63; j++)
    {
      for (int k = 1; k < 63; k++)
      {
        for (int l = 1; l < 63; l++)
        {
          char password[5] = {
            possibilities[k],
            possibilities[l],
            possibilities[j],
            possibilities[i],
            '\0'
          };

          MAU_64(buffer, password);
          uint64_t new_hash = hash_to_number(buffer);
          
          uint32_t index;
          if (search_hash(users, n_users, new_hash, &index))
          {
            counter++;
            strcpy(users[index].senha, password);
            if (counter == n_users) return; 
          }
        }
      }
    }
  }
  return;
}

