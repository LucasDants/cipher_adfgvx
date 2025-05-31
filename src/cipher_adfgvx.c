#include <stdio.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 2560
#define KEY_LENGTH 2

const char cipher_key[KEY_LENGTH] = "UM";
const char symbols[6] = {'A', 'D', 'F', 'G', 'V', 'X'};
const char square[6][6] = {
    {'A','B','C','D','E','F'},
    {'G','H','I','J','K','L'},
    {'M','N','O','P','Q','R'},
    {'S','T','U','V','W','X'},
    {'Y','Z','0','1','2','3'},
    {'4','5','6','7','8','9'}
};

/**
 * @brief Encontra os símbolos ADFGVX correspondentes a um caractere.
 * 
 * @param c Caractere a ser cifrado.
 * @param row Ponteiro para armazenar o símbolo da linha.
 * @param col Ponteiro para armazenar o símbolo da coluna.
 * @return int Retorna 1 se o caractere foi encontrado, 0 caso contrário.
 */
int get_adfgvx_symbols(char c, char *row, char *col) {
  for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 6; j++) {
          if (square[i][j] == c) {
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
 *
 * @param symbol Símbolo a ser inserido (row ou col).
 * @param symbol_count Contador global de símbolos (será incrementado).
 * @param matrix Matriz de saída contendo os símbolos organizados por coluna.
 * @param column_lengths Vetor com a quantidade de símbolos por coluna (será atualizado).
 */
void insert_symbol_to_column(char symbol, int *symbol_count, char matrix[KEY_LENGTH][MAX_MESSAGE_LENGTH], int column_lengths[KEY_LENGTH]) {
  int col_index = (*symbol_count) % KEY_LENGTH;
  int write_pos = column_lengths[col_index];

  matrix[col_index][write_pos] = symbol;
  column_lengths[col_index]++;
  (*symbol_count)++;
}

/**
 * @brief Converte a mensagem em colunas de símbolos ADFGVX para cifra por transposição.
 * 
 * @param message Mensagem original a ser cifrada.
 * @param encoded_symbol_matrix Matriz onde os símbolos cifrados serão armazenados por coluna.
 * @param symbols_per_column Vetor que armazena o número de elementos em cada coluna.
 */
void polybius_encode_to_columns(char message[], char encoded_symbol_matrix[KEY_LENGTH][MAX_MESSAGE_LENGTH], int symbols_per_column[KEY_LENGTH]) {
  int i, symbol_count = 0;
    
  for (i = 0; message[i] != '\0'; i++) {
    char row, col;
      
    if (!get_adfgvx_symbols(message[i], &row, &col)) {
      continue; 
    }

    insert_symbol_to_column(row, &symbol_count, encoded_symbol_matrix, symbols_per_column);
    insert_symbol_to_column(col, &symbol_count, encoded_symbol_matrix, symbols_per_column);
  }
}

/**
 * @brief Reorganiza as colunas da matriz com base na ordem alfabética da chave.
 * 
 * @param columns Matriz com os dados cifrados por colunas.
 * @param symbols_per_column Vetor com o número de elementos em cada coluna.
 */
void transpose_columns_by_key_order(char encoded_symbol_matrix[KEY_LENGTH][MAX_MESSAGE_LENGTH], int symbols_per_column[KEY_LENGTH]) {
  int i, j, k;
  char sorted_key[KEY_LENGTH];
  int temp_count;

  // Copia a chave original para preservar sua ordem
  for (i = 0; i < KEY_LENGTH; i++) {
      sorted_key[i] = cipher_key[i];
  }

  // Ordenação da chave e reorganização das colunas
  for (i = 0; i < KEY_LENGTH - 1; i++) {
      for (j = 0; j < KEY_LENGTH - i - 1; j++) {
          if (sorted_key[j] > sorted_key[j + 1]) {
              // Troca os caracteres da chave ordenada
              char tmp = sorted_key[j];
              sorted_key[j] = sorted_key[j + 1];
              sorted_key[j + 1] = tmp;

              // Troca símbolo por símbolo das colunas associadas
              for (k = 0; k < MAX_MESSAGE_LENGTH; k++) {
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
 * 
 * @param message Mensagem de entrada.
 * @param encoded_symbol_matrix Matriz onde os símbolos codificados serão armazenados.
 * @param symbols_per_column Vetor com a contagem de elementos em cada coluna.
 */
void cipher_adfgvx(char message[], char encoded_symbol_matrix[KEY_LENGTH][MAX_MESSAGE_LENGTH], int symbols_per_column[KEY_LENGTH]) {
  polybius_encode_to_columns(message, encoded_symbol_matrix, symbols_per_column);
  transpose_columns_by_key_order(encoded_symbol_matrix, symbols_per_column);
}

/**
 * @brief Função principal do programa de cifragem ADFGVX.
 *
 * Esta função executa o fluxo completo de cifragem baseado no algoritmo ADFGVX:
 * 
 * - Abre o arquivo "./io/message.txt" que contém a mensagem original.
 * - Lê a mensagem em memória.
 * - Converte cada caractere da mensagem em um par de símbolos (A, D, F, G, V, X)
 *   usando a matriz Polybius definida.
 * - Organiza os pares de símbolos em colunas com base na chave fornecida.
 * - Transpõe as colunas de acordo com a ordem alfabética da chave.
 * - Escreve o resultado linearizado no arquivo "./io/encrypted.txt".
 *
 * A saída escrita representa a mensagem cifrada, codificada com a substituição e
 * transposição definida pelo algoritmo ADFGVX.
 *
 * @note A chave usada está definida como constante no código: `cipher_key`.
 * 
 * @return int Retorna 0 em caso de sucesso. Retorna 1 se houver erro na leitura ou escrita de arquivos.
 */
int main() {
    FILE *message_file, *encrypted_file;
    char message[MAX_MESSAGE_LENGTH];
    char encoded_symbol_matrix[KEY_LENGTH][MAX_MESSAGE_LENGTH];
    int symbols_per_column[KEY_LENGTH] = {0};
    int i, j;

    // Ler o arquivo da mensagem
    message_file = fopen("./io/message.txt", "r");
    if (message_file == NULL) {
      perror("Error opening './io/message.txt'.\n");
      return 1;
    }
    fgets(message, MAX_MESSAGE_LENGTH, message_file);
    fclose(message_file);

    // Realizar a cifra ADFGVX 
    cipher_adfgvx(message, encoded_symbol_matrix, symbols_per_column);

    // Salvar a mensagem cifrada em 'encrypted.txt'
    encrypted_file = fopen("./io/encrypted.txt", "w");
    if (encrypted_file == NULL) {
      perror("Error opening './io/encrypted.txt'.\n");
      return 1;
    }

    // Escrever as colunas ordenadas no arquivo cifrado
    for (i = 0; i < KEY_LENGTH; i++) {
      for (j = 0; j < symbols_per_column[i]; j++) {
        fputc(encoded_symbol_matrix[i][j], encrypted_file);
      }
    }
    fclose(encrypted_file);

    return 0;
}
