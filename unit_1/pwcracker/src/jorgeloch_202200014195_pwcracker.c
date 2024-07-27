#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

// Macro rand8
# define rand8 ((rand () >> 8) & 0b111)

typedef struct user_info {
  char login[8];
  uint8_t hash[8];
} user_info;

const static char possibilities[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\0";

void MAU_64 (uint8_t * hash, const char* senha);
bool task (user_info u, char* senha_resposta);
void stringToByte(char *string, uint8_t *byteArray);
void print_hash(uint8_t* hash);
bool compare_hash(uint8_t* x, uint8_t* y);
user_info read_from_file(FILE *file);

int main (int argc, char** argv)
{
  if (argc != 3)
  {
    printf("Usage: %s <input>.txt <output>.txt\n", argv[0]);
    return 1;
  }

  FILE *input = fopen(argv[1], "r");
  FILE *output = fopen(argv[2], "w");

  int n_user_infos;
  fscanf(input, "%d", &n_user_infos);

  user_info *user_infos = (user_info*) malloc(n_user_infos * sizeof(user_info));
  for (int i = 0; i < n_user_infos; i++)
  {
    user_infos[i] = read_from_file(input);
  }
  
  for (int i = 0; i < n_user_infos; i++)
  {
    char senha_quebrada[5];
    if (task(user_infos[i], senha_quebrada))
    {
      printf("encontramos a senha: %i\n", i + 1);
      printf("%s:%s\n", user_infos[i].login, senha_quebrada);
      fprintf(output, "%s:%s\n", user_infos[i].login, senha_quebrada);
    }
  }

  fclose(input);
  fclose(output);

  free(user_infos);

  return 0;
}

// Função de hash MAU -64
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

void stringToByte(char *string, uint8_t *byteArray)
{
  if (strlen(string) != 16)
    return;
  for (int i = 0; i < 8; i++) 
  {
    char byteStr[3] = {string[2 * i], string[2 * i + 1], '\0'};
    sscanf(byteStr, "%hhx", &byteArray[i]);
  }
}

bool compare_hash(uint8_t* x, uint8_t* y)
{
  for (int i = 0; i < 8; i++)
  {
    if (x[i] != y[i])
      return false;
  }
  return true;
}

bool task (user_info u, char* senha_resposta)
{
  for (int i = 0; i < 62; i++)
  {
    for (int j = 0; j < 62; j++)
    {
      for (int k = 62; k >= 0; k--)
      {
        for (int w = k == 62 ? 62 : 61; w >= 0; w++)
        {
          char senha[5];
          senha[0] = possibilities[i];
          senha[1] = possibilities[j];
          senha[2] = possibilities[w];
          senha[3] = possibilities[k];
          senha[4] = '\0';
          uint8_t *hash = (uint8_t*) calloc(8, sizeof(uint8_t));
          MAU_64(hash, senha);

          if (compare_hash(hash, u.hash))
          {
            printf("Senha encontrada!\n");
            strcpy(senha_resposta, senha);
            free(hash);
            return true;
          }
          free(hash);
        }
      }
    }
  }
  printf("Senha não encontrada\n");
  return false;
}

void print_hash(uint8_t* hash)
{
  for (int i = 0; i < 8; i++)
  {
    printf("%hhx", hash[i]);
  }
  printf("\n"); 
}

user_info convert_line_to_user_info(char *line)
{
  user_info result;
  int index = 0;
  char token = *line + index;
  while (token != ':')
  {

  }
}

user_info read_from_file(FILE *file)
{
  user_info result;
  char *buffer = (char*) calloc(14, sizeof(char));
  fscanf(file, "%s:", buffer);

  result = convert_line_to_user_info(buffer);
  printf("Login: %s\n", result.login);
  printf("Hash: ");
  print_hash(result.hash);

  free(buffer);

  return result;
}

