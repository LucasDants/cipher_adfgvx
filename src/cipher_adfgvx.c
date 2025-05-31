#include <stdio.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 2560
#define MAX_KEY_LENGTH 9

const char symbols[6] = {'A', 'D', 'F', 'G', 'V', 'X'};
const char square[6][6] = {
    {'A', 'B', 'C', 'D', 'E', 'F'},
    {'G', 'H', 'I', 'J', 'K', 'L'},
    {'M', 'N', 'O', 'P', 'Q', 'R'},
    {'S', 'T', 'U', 'V', 'W', 'X'},
    {'Y', 'Z', ' ', ',', '.', '1'},
    {'2', '3', '4', '5', '6', '7'}};

/**
 * @brief Lê o conteúdo de um arquivo em um buffer.
 *
 * @param filename Caminho para o arquivo a ser lido.
 * @param buffer Buffer onde o conteúdo será armazenado.
 * @param max_length Tamanho máximo permitido do buffer.
 * @return int 0 em caso de sucesso, 1 em caso de erro.
 */
int read_file(const char *filename, char *buffer, int max_length)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    return 1;
  }

  fgets(buffer, max_length, file);

  fclose(file);
  return 0;
}

/**
 * @brief Encontra os símbolos ADFGVX correspondentes a um caractere.
 *
 * @param c Caractere a ser cifrado.
 * @param row Ponteiro para armazenar o símbolo da linha.
 * @param col Ponteiro para armazenar o símbolo da coluna.
 * @return int Retorna 1 se o caractere foi encontrado, 0 caso contrário.
 */
int get_adfgvx_symbols(char c, char *row, char *col)
{
  for (int i = 0; i < 6; i++)
  {
    for (int j = 0; j < 6; j++)
    {
      if (square[i][j] == c)
      {
        *row = symbols[i];
        *col = symbols[j];
        return 1;
      }
    }
  }
  return 0;
}

/**
 * @brief Insere um símbolo ADFGVX na matriz de colunas.
 * @param key_length Comprimento da chave.
 * @param symbol Símbolo a ser inserido (row ou col).
 * @param symbol_count Contador global de símbolos (será incrementado).
 * @param encoded_symbol_matrix Matriz de saída contendo os símbolos organizados por coluna.
 * @param symbols_per_column Vetor com a quantidade de símbolos por coluna (será atualizado).
 */
void insert_symbol_to_column(int key_length, char symbol, int *symbol_count, char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH], int symbols_per_column[])
{
  int col_index = (*symbol_count) % key_length;
  int write_pos = symbols_per_column[col_index];

  encoded_symbol_matrix[col_index][write_pos] = symbol;
  symbols_per_column[col_index]++;
  (*symbol_count)++;
}

/**
 * @brief Converte a mensagem em colunas de símbolos ADFGVX para cifra por transposição.
 * @param key_length Comprimento da chave.
 * @param message Mensagem original a ser cifrada.
 * @param encoded_symbol_matrix Matriz onde os símbolos cifrados serão armazenados por coluna.
 * @param symbols_per_column Vetor que armazena o número de elementos em cada coluna.
 */
void polybius_encode_to_columns(int key_length, char message[], char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH], int symbols_per_column[])
{
  int i, symbol_count = 0;

  for (i = 0; message[i] != '\0'; i++)
  {
    char row, col;

    if (!get_adfgvx_symbols(message[i], &row, &col))
    {
      continue;
    }

    insert_symbol_to_column(key_length, row, &symbol_count, encoded_symbol_matrix, symbols_per_column);
    insert_symbol_to_column(key_length, col, &symbol_count, encoded_symbol_matrix, symbols_per_column);
  }
}

/**
 * @brief Reorganiza as colunas da matriz com base na ordem alfabética da chave.
 * @param key A chave usada na transposição (array de caracteres).
 * @param key_length Comprimento da chave.
 * @param encoded_symbol_matrix Matriz com os dados cifrados por colunas.
 * @param symbols_per_column Vetor com o número de elementos em cada coluna.
 */
void transpose_columns_by_key_order(char key[], int key_length, char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH], int symbols_per_column[])
{
  int i, j, k;
  char sorted_key[key_length];
  int temp_count;

  // Copia a chave original para preservar sua ordem
  for (i = 0; i < key_length; i++)
  {
    sorted_key[i] = key[i];
  }

  // Ordenação da chave e reorganização das colunas
  for (i = 0; i < key_length - 1; i++)
  {
    for (j = 0; j < key_length - i - 1; j++)
    {
      if (sorted_key[j] > sorted_key[j + 1])
      {
        // Troca os caracteres da chave ordenada
        char tmp = sorted_key[j];
        sorted_key[j] = sorted_key[j + 1];
        sorted_key[j + 1] = tmp;

        // Troca símbolo por símbolo das colunas associadas
        for (k = 0; k < MAX_MESSAGE_LENGTH; k++)
        {
          char temp = encoded_symbol_matrix[j][k];
          encoded_symbol_matrix[j][k] = encoded_symbol_matrix[j + 1][k];
          encoded_symbol_matrix[j + 1][k] = temp;
        }

        // Troca o contador de elementos de cada coluna
        temp_count = symbols_per_column[j];
        symbols_per_column[j] = symbols_per_column[j + 1];
        symbols_per_column[j + 1] = temp_count;
      }
    }
  }
}

/**
 * @brief Aplica a cifra ADFGVX: codifica os símbolos e faz a transposição das colunas.
 * @param key A chave usada na transposição (array de caracteres).
 * @param key_length Comprimento da chave.
 * @param message Mensagem de entrada.
 * @param encoded_symbol_matrix Matriz onde os símbolos codificados serão armazenados.
 * @param symbols_per_column Vetor com a contagem de elementos em cada coluna.
 */
void cipher_adfgvx(char key[], int key_length, char message[], char encoded_symbol_matrix[][MAX_MESSAGE_LENGTH], int symbols_per_column[])
{
  polybius_encode_to_columns(key_length, message, encoded_symbol_matrix, symbols_per_column);
  transpose_columns_by_key_order(key, key_length, encoded_symbol_matrix, symbols_per_column);
}

/**
 * @brief Função principal do programa de cifragem ADFGVX.
 *
 * Esta função executa todo o processo de cifragem baseado no algoritmo ADFGVX,
 * que consiste nas seguintes etapas:
 *
 * 1. Leitura da chave de transposição a partir do arquivo "./key.txt".
 * 2. Leitura da mensagem a ser cifrada a partir do arquivo "./message.txt".
 * 3. Conversão da mensagem em pares de símbolos (A, D, F, G, V, X) usando a matriz Polybius.
 * 4. Distribuição dos símbolos em colunas com base na chave.
 * 5. Transposição das colunas conforme a ordem alfabética da chave.
 * 6. Escrita da mensagem cifrada linearizada no arquivo "./encrypted.txt".
 *
 * O algoritmo garante que a reorganização dos símbolos preserve a estrutura
 * reversível do processo de decodificação.
 *
 * @note O tamanho da chave é determinado dinamicamente a partir do conteúdo de "./key.txt".
 *       A chave deve estar em uma única linha, sem espaços extras ou quebras de linha desnecessárias.
 *
 * @return int Retorna 0 em caso de sucesso, ou 1 se houver erro na leitura ou escrita de arquivos.
 */
int main()
{
  FILE *message_file, *encrypted_file, *key_file;
  char cipher_key[MAX_KEY_LENGTH], message[MAX_MESSAGE_LENGTH];
  int symbols_per_column[MAX_KEY_LENGTH] = {0};
  int KEY_LENGTH = 0;
  int is_file_read, i, j;

  // Lê a chave de cifra do arquivo
  is_file_read = read_file("./key.txt", cipher_key, MAX_KEY_LENGTH);
  if (is_file_read != 0)
  {
    perror("Error reading file './key.txt'.\n");
    return 1;
  }

  // Define o tamanho da chave com base no conteúdo lido
  KEY_LENGTH = strlen(cipher_key);

  // Define variáveis que dependem do tamanho da chave
  char encoded_symbol_matrix[KEY_LENGTH][MAX_MESSAGE_LENGTH];

  // Ler a messagem
  is_file_read = read_file("./message.txt", message, MAX_MESSAGE_LENGTH);
  if (is_file_read != 0)
  {
    perror("Error reading file './message.txt'.\n");
    return 1;
  }

  // Realizar a cifra ADFGVX
  cipher_adfgvx(cipher_key, KEY_LENGTH, message, encoded_symbol_matrix, symbols_per_column);

  // Salvar a mensagem cifrada em 'encrypted.txt'
  encrypted_file = fopen("./encrypted.txt", "w");
  if (encrypted_file == NULL)
  {
    perror("Error opening './encrypted.txt'.\n");
    return 1;
  }

  // Escrever as colunas ordenadas no arquivo cifrado
  for (i = 0; i < KEY_LENGTH; i++)
  {
    for (j = 0; j < symbols_per_column[i]; j++)
    {
      fputc(encoded_symbol_matrix[i][j], encrypted_file);
    }
  }
  fclose(encrypted_file);

  return 0;
}
