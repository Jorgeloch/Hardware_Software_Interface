#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/sysinfo.h>

// Macro rand8
# define rand8 ((hrand() >> 8) & 0b111)
# define pw(p, i, n) ((i < n) ? (p[i]) : (p[i%n]*i))

// Número pseudo - aleatório
static __thread uint32_t ri = 1;
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
  uint64_t hash;
  uint32_t original_index;
  char login[9];
  char senha[5];
} user;

static char possibilities[63] = "uC6Z4gJhzfRLPtXoNMIW8ajcykx3EY7bqKOTH10wmiQABeS9FnlG2drvsp5DUV\0";
static char *next_character = possibilities;
static user* users;
static uint32_t n_users;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* cracker_behavior ();
void crack_hashes();
uint64_t hash_to_number(uint8_t *hash);
int compare_users_by_hash(const void* a, const void* b);
int compare_user_by_index(const void* a, const void* b);

int main (int argc, char** argv)
{
  // garantindo número correto de argumentos
  if (argc != 3)
  {
    printf("usage: %s <input file> <output file>\n", argv[0]);
  }
  // abrindo arquivos de entrada e saída
  FILE *input = fopen(argv[1], "r");
  FILE *output = fopen(argv[2], "w");

  // lendo do arquivo de entrada o numero de usuarios a serem lidos
  fscanf(input, "%u ", &n_users);

  // alocando espaço para os usuários
  users = (user*) calloc(n_users, sizeof(user));
  for (int i = 0; i < n_users; i++)
  {
    // lendo o login do usuário do arquivo de entrada
    // regex para ler até 8 caracteres que não sejam ':'
    fscanf(input, "%8[^:]:", users[i].login);
    // lendo o hash do usuário do arquivo de entrada como um inteiro de 64 bits
    fscanf(input, "%lx ", &users[i].hash);
    users[i].original_index = i;
  }

  // verifica o número de cores disponiveis na maquina
  uint8_t n_threads = get_nprocs();
  // cria array de threads
  pthread_t crackers[n_threads];

  for (int i = 0; i < n_threads; i++)
  {
    // cria threads passando como parametro a rotina a ser executada
    pthread_create(&crackers[i], NULL, cracker_behavior, NULL);
  }

  // ordena array de usuários por hash para poder fazer busca binária
  qsort(users, n_users, sizeof(user), compare_users_by_hash);
  for (int i = 0; i < n_threads; i++)
  {
    pthread_join(crackers[i], NULL);
  }

  // ordena array de usuários por índice original para poder escrever no arquivo de saída
  qsort(users, n_users, sizeof(user), compare_user_by_index);
  for (int i = 0; i < n_users; i++)
  {
    fprintf(output, "%s:%s\n", users[i].login, users[i].senha);
  }

  // liberando memoria
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

// função para comparar usuários por hash
int compare_users_by_hash(const void* a, const void* b)
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

// fnção para comparar usuários por índice original
int compare_user_by_index(const void* a, const void* b)
{
  user *user_a = (user*) a;
  user *user_b = (user*) b;
  if (user_a->original_index > user_b->original_index)
  {
    return 1;
  }
  else if (user_b->original_index > user_a->original_index)
  {
    return -1;
  }
  else 
  {
    return 0;
  }
}

// função para buscar hash no array de usuários
bool search_hash(uint64_t target, uint32_t* index)
{
  // caso prefira utilizar busca sequencial
  /* for (int i = 0; i < n_users; i++) */
  /* { */
  /*   if (users[i].hash == target) */
  /*   { */
  /*     *index = i; */
  /*     return true; */
  /*   } */
  /* } */
  // caso prefira utilizar busca binaria
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

// rotina a ser implementada pelas threads
void* cracker_behavior ()
{
  while (next_character)
  {
    // iniciar mutex
		pthread_mutex_lock(&mutex);
    char cur_char = *next_character; 
    next_character = cur_char == 'V' ? NULL : next_character + 1 * sizeof(char);
    // liberar o mutex
		pthread_mutex_unlock(&mutex);
    uint8_t buffer[8];
    // gerar permutações
    for (int i = 63; i >= 0; i--)
    {
      // esse j dependendo do valor de i garante que todas as strings que tem 
      // tamanho 2 não possuam caracteres a direita do primeiro \0
      for (int j = i == 63 ? 63 : 62; j >= 0; j--)
      {
        for (int k = 0; k < 62; k++)
        {
          char test_password[5] = {
            cur_char,
            possibilities[k],
            possibilities[j],
            possibilities[i],
            '\0'
          };

          // aplica o hash MAU-64 a senha gerada
          MAU_64(buffer, test_password);
          // transforma o hash gerado em um numero de 64 bits
          uint64_t new_hash = hash_to_number(buffer);

          uint32_t index;
          // busca a existencia do hash na lista de usuários
          if (search_hash(new_hash, &index))
          {
            // iniciar mutex
            pthread_mutex_lock(&mutex);
            // salva a senha do usuário na struct de usuário na posição correta 
            strcpy(users[index].senha, test_password);
            // liberar mutex
            pthread_mutex_unlock(&mutex);
          }
        }
      }
    }
  }
  pthread_exit(NULL);
}

