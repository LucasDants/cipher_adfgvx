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
    {'Y','Z',' ','1','2','3'},
    {'4','5','6','7','8','9'}
};

int symbol_index(char c) {
    for (int i = 0; i < 6; i++) {
        if (symbols[i] == c) return i;
    }
    return -1;
}

void reverse_transposition(char *input, char *key, char columns[KEY_LENGTH][MAX_MESSAGE_LENGTH], int col_counts[KEY_LENGTH]) {
  int len = strlen(input);
  int rows = len / KEY_LENGTH;
  int extra = len % KEY_LENGTH;

  // Obter a ordem da chave (como índices ordenados alfabeticamente)
  int order[KEY_LENGTH];
  for (int i = 0; i < KEY_LENGTH; i++) order[i] = i;

  // Ordenar ordem[] com base nos caracteres da chave
  for (int i = 0; i < KEY_LENGTH - 1; i++) {
      for (int j = 0; j < KEY_LENGTH - i - 1; j++) {
          if (key[order[j]] > key[order[j + 1]]) {
              int tmp = order[j];
              order[j] = order[j + 1];
              order[j + 1] = tmp;
          }
      }
  }

  // Calcular quantos caracteres em cada coluna ordenada
  for (int i = 0; i < KEY_LENGTH; i++) {
      col_counts[order[i]] = rows + (i < extra ? 1 : 0);
  }

  // Preencher as colunas na ordem alfabética da chave
  int pos = 0;
  for (int i = 0; i < KEY_LENGTH; i++) {
      int col_index = order[i];  // posição original da coluna
      for (int j = 0; j < col_counts[col_index]; j++) {
          columns[col_index][j] = input[pos++];
      }
  }
}

void reverse_polybius(char columns[KEY_LENGTH][MAX_MESSAGE_LENGTH], int col_counts[KEY_LENGTH], char *output) {
    int total = 0;
    for (int i = 0; i < KEY_LENGTH; i++) {
        total += col_counts[i];
    }

    int max_rows = 0;
    for (int i = 0; i < KEY_LENGTH; i++) {
        if (col_counts[i] > max_rows) max_rows = col_counts[i];
    }

    int pos = 0;
    for (int r = 0; r < max_rows; r++) {
        for (int c = 0; c < KEY_LENGTH; c++) {
            if (r < col_counts[c]) {
                output[pos++] = columns[c][r];
            }
        }
    }
    output[pos] = '\0';
}

void decode_symbols(char *pairs, char *message) {
    int len = strlen(pairs);
    int msg_index = 0;

    for (int i = 0; i < len; i += 2) {
        int row = symbol_index(pairs[i]);
        int col = symbol_index(pairs[i + 1]);
        if (row >= 0 && col >= 0) {
            message[msg_index++] = square[row][col];
        }
    }
    message[msg_index] = '\0';
}

void decipher_adfgvx(char *ciphered_text, char *key, char *output) {
    char columns[KEY_LENGTH][MAX_MESSAGE_LENGTH] = {{0}};
    int col_counts[KEY_LENGTH] = {0};
    char rearranged[MAX_MESSAGE_LENGTH] = {0};

    reverse_transposition(ciphered_text, key, columns, col_counts);
    reverse_polybius(columns, col_counts, rearranged);
    decode_symbols(rearranged, output);
}

int main() {
  FILE *encrypted_file;
  char encrypted[MAX_MESSAGE_LENGTH];
  char decrypted[MAX_MESSAGE_LENGTH];

  // Abrir o arquivo com o texto cifrado
  encrypted_file = fopen("./io/encrypted.txt", "r");
  if (encrypted_file == NULL) {
      perror("Erro ao abrir 'encrypted.txt'");
      return 1;
  }

  // Ler a mensagem cifrada
  fgets(encrypted, MAX_MESSAGE_LENGTH, encrypted_file);
  fclose(encrypted_file);

  // Decifrar a mensagem
  decipher_adfgvx(encrypted, cipher_key, decrypted);

  // Imprimir a mensagem decifrada
  printf("Mensagem decifrada:\n%s\n", decrypted);

  return 0;
}
