#include <stdio.h>
#include <string.h>
#include <time.h>

// ! COMMON DEFINES
#define MAX_MESSAGE_LENGTH 2560
#define MAX_KEY_LENGTH 9

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

const char symbols[6] = {'A', 'D', 'F', 'G', 'V', 'X'};
const char square[6][6] = {
    {'A', 'B', 'C', 'D', 'E', 'F'},
    {'G', 'H', 'I', 'J', 'K', 'L'},
    {'M', 'N', 'O', 'P', 'Q', 'R'},
    {'S', 'T', 'U', 'V', 'W', 'X'},
    {'Y', 'Z', ' ', ',', '.', '1'},
    {'2', '3', '4', '5', '6', '7'}};
// ! END COMMON DEFINES

// ! CIPHER FUNCTIONS
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
 * @param max_per_column Número máximo de posições em cada coluna (para VLA).
 * @param symbol Símbolo a ser inserido (row ou col).
 * @param symbol_count Contador global de símbolos (será incrementado).
 * @param encoded_symbol_matrix Matriz de saída contendo os símbolos organizados por coluna.
 * @param symbols_per_column Vetor com a quantidade de símbolos por coluna (será atualizado).
 */
void insert_symbol_to_column(int key_length, int max_per_column, char symbol, int *symbol_count, char encoded_symbol_matrix[key_length][max_per_column], int symbols_per_column[])
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
 * @param max_per_column Número máximo de posições em cada coluna (para VLA).
 * @param message Mensagem original a ser cifrada.
 * @param encoded_symbol_matrix Matriz onde os símbolos cifrados serão armazenados por coluna.
 * @param symbols_per_column Vetor que armazena o número de elementos em cada coluna.
 */
void polybius_encode_to_columns(int key_length, int max_per_column, char message[], char encoded_symbol_matrix[key_length][max_per_column], int symbols_per_column[])
{
  int i, symbol_count = 0;

  for (i = 0; message[i] != '\0'; i++)
  {
    char row, col;

    // Ignora caracteres que não estão na matriz Polybius
    if (!get_adfgvx_symbols(message[i], &row, &col))
    {
      continue;
    }

    insert_symbol_to_column(key_length, max_per_column, row, &symbol_count, encoded_symbol_matrix, symbols_per_column);
    insert_symbol_to_column(key_length, max_per_column, col, &symbol_count, encoded_symbol_matrix, symbols_per_column);
  }
}

/**
 * @brief Reorganiza as colunas da matriz com base na ordem alfabética da chave.
 * @param key A chave usada na transposição (array de caracteres).
 * @param key_length Comprimento da chave.
 * @param max_per_column Número máximo de posições em cada coluna (para VLA).
 * @param encoded_symbol_matrix Matriz com os dados cifrados por colunas.
 * @param symbols_per_column Vetor com o número de elementos em cada coluna.
 */
void transpose_columns_by_key_order(char key[], int key_length, int max_per_column, char encoded_symbol_matrix[key_length][max_per_column], int symbols_per_column[])
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

        // Troca símbolo por símbolo das colunas associadas (até max_per_column)
        for (k = 0; k < max_per_column; k++)
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
 * @param max_per_column Número máximo de posições em cada coluna (para VLA).
 * @param message Mensagem de entrada.
 * @param encoded_symbol_matrix Matriz onde os símbolos codificados serão armazenados.
 * @param symbols_per_column Vetor com a contagem de elementos em cada coluna.
 */
void cipher_adfgvx(char key[], int key_length, int max_per_column, char message[], char encoded_symbol_matrix[key_length][max_per_column], int symbols_per_column[])
{
  polybius_encode_to_columns(key_length, max_per_column, message, encoded_symbol_matrix, symbols_per_column);
  transpose_columns_by_key_order(key, key_length, max_per_column, encoded_symbol_matrix, symbols_per_column);
}
// ! END CIPHER FUNCTIONS

// ! DECIPHER FUNCTIONS
/**
 * @brief Retorna o índice de um símbolo ADFGVX dentro do vetor `symbols`.
 *
 * @param c Caractere a ser buscado no vetor de símbolos.
 * @return int Índice do símbolo no vetor `symbols`, ou -1 se não for encontrado.
 */
int symbol_index(char c)
{
    for (int i = 0; i < 6; i++)
    {
        if (symbols[i] == c)
            return i;
    }
    return -1;
}

/**
 * @brief Reconstrói as colunas originais da cifra com base na chave de transposição.
 *
 * Esta função realiza o processo inverso da transposição, determinando a quantidade de
 * caracteres por coluna e reorganizando o texto cifrado linearizado nas colunas originais.
 *
 * @param input Texto cifrado linearizado.
 * @param key Chave usada na cifra.
 * @param key_length Comprimento da chave.
 * @param columns Matriz que armazenará as colunas reconstruídas.
 * @param col_counts Vetor que armazena o número de caracteres em cada coluna.
 */
void reverse_transposition(char *input, char *key, int key_length, char columns[][MAX_MESSAGE_LENGTH], int col_counts[])
{
    int len = strlen(input);
    int rows = len / key_length;
    int extra = len % key_length;

    // 1) monta o vetor “order” com os índices 0,1,2,...,key_length-1
    int order[key_length];
    for (int i = 0; i < key_length; i++)
    {
        order[i] = i;
    }

    // 2) ordena esse vetor “order” segundo key[i], para saber a ordem alfabética da chave
    for (int i = 0; i < key_length - 1; i++)
    {
        for (int j = 0; j < key_length - i - 1; j++)
        {
            if (key[order[j]] > key[order[j + 1]])
            {
                int tmp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = tmp;
            }
        }
    }

    // 3) determina quantos símbolos cada coluna (no índice original) vai ter
    //    — a coluna “orig_index” recebeu rows+1 símbolos se orig_index < extra; senão, só rows.
    for (int i = 0; i < key_length; i++)
    {
        int orig_index = order[i];
        col_counts[orig_index] = rows + (orig_index < extra ? 1 : 0);
    }

    // 4) preenche cada “columns[orig_index]” lendo do texto cifrado linearizado
    int pos = 0;
    for (int i = 0; i < key_length; i++)
    {
        int col_index = order[i]; // a i-ésima coluna (na ordem alfabética) corresponde a columns[col_index]
        for (int j = 0; j < col_counts[col_index]; j++)
        {
            columns[col_index][j] = input[pos++];
        }
    }
}

/**
 * @brief Reverte a organização em colunas, reconstruindo a sequência de símbolos linha a linha.
 *
 * Esta função recria a sequência de símbolos ADFGVX antes da transposição, percorrendo as colunas
 * reconstruídas linha por linha.
 *
 * @param columns Matriz contendo os dados em colunas.
 * @param col_counts Vetor com o número de elementos por coluna.
 * @param key_length Comprimento da chave.
 * @param output Buffer para armazenar a sequência reconstruída de símbolos ADFGVX.
 */
void reverse_polybius(char columns[][MAX_MESSAGE_LENGTH], int col_counts[], int key_length, char *output)
{
    int max_rows = 0, pos = 0;

    for (int i = 0; i < key_length; i++)
    {
        if (col_counts[i] > max_rows)
            max_rows = col_counts[i];
    }

    for (int r = 0; r < max_rows; r++)
    {
        for (int c = 0; c < key_length; c++)
        {
            if (r < col_counts[c])
            {
                output[pos++] = columns[c][r];
            }
        }
    }
    output[pos] = '\0';
}

/**
 * @brief Decodifica pares de símbolos ADFGVX em caracteres da matriz Polybius.
 *
 * Para cada par de símbolos ADFGVX, encontra as coordenadas correspondentes na matriz
 * Polybius e extrai o caractere original.
 *
 * @param pairs Sequência de pares de símbolos ADFGVX.
 * @param message Buffer onde será armazenada a mensagem decodificada.
 */
void decode_symbols(char *pairs, char *message)
{
    int len = strlen(pairs);
    int msg_index = 0;

    for (int i = 0; i < len; i += 2)
    {
        int row = symbol_index(pairs[i]);
        int col = symbol_index(pairs[i + 1]);
        if (row >= 0 && col >= 0)
        {
            message[msg_index++] = square[row][col];
        }
    }
    message[msg_index] = '\0';
}

/**
 * @brief Função principal para decodificar a cifra ADFGVX.
 *
 * Executa a sequência de etapas para decifrar o texto cifrado:
 * 1. Reverte a transposição com base na chave.
 * 2. Reagrupa os símbolos em sua ordem original.
 * 3. Decodifica os pares ADFGVX na matriz Polybius.
 *
 * @param encrypted_text Texto cifrado.
 * @param key Chave de cifra.
 * @param key_length Comprimento da chave.
 * @param output Buffer onde a mensagem decodificada será armazenada.
 */
void decipher_adfgvx(char *encrypted_text, char *key, int key_length, char *output)
{
    char columns[key_length][MAX_MESSAGE_LENGTH];
    int col_counts[MAX_KEY_LENGTH] = {0};
    char rearranged[2 * MAX_MESSAGE_LENGTH];

    reverse_transposition(encrypted_text, key, key_length, columns, col_counts);
    reverse_polybius(columns, col_counts, key_length, rearranged);
    decode_symbols(rearranged, output);
}
// ! END DECIPHER FUNCTIONS

// * Testes para o algoritmo ADFGVX
/**
 * @brief Testa a função get_adfgvx_symbols com caracteres válidos e inválidos.
 *
 * Verifica se a função retorna corretamente os símbolos ADFGVX correspondentes
 * a um caractere presente na matriz, e se ignora corretamente os caracteres inválidos.
 */
void test_get_adfgvx_symbols()
{
    char row, col;
    int success = get_adfgvx_symbols('C', &row, &col);

    if (success && row == 'A' && col == 'F')
    {
        printf("\tSucesso: 'C' convertido para [%c, %c]\n", row, col);
    }
    else
    {
        printf("\tErro: Obteve [%c, %c]\n", row, col);
    }

    success = get_adfgvx_symbols('#', &row, &col);
    if (!success)
    {
        printf("\tSucesso: Caractere # corretamente rejeitado.\n");
    }
    else
    {
        printf("\tErro: Caractere # foi aceito!\n");
    }
}

/**
 * @brief Testa a função insert_symbol_to_column para inserção circular nas colunas.
 *
 * Verifica se os símbolos são inseridos corretamente nas colunas com base no contador global.
 */
void test_insert_symbol_to_column()
{
    int key_length = 3;
    int max_per_column = MAX_MESSAGE_LENGTH;
    char matrix[3][MAX_MESSAGE_LENGTH] = {{0}};
    int symbols_per_column[3] = {0};
    int symbol_count = 0;

    insert_symbol_to_column(key_length, max_per_column, 'X', &symbol_count, matrix, symbols_per_column);
    insert_symbol_to_column(key_length, max_per_column, 'F', &symbol_count, matrix, symbols_per_column);
    insert_symbol_to_column(key_length, max_per_column, 'A', &symbol_count, matrix, symbols_per_column);
    insert_symbol_to_column(key_length, max_per_column, 'G', &symbol_count, matrix, symbols_per_column);

    if (matrix[0][0] == 'X' && matrix[1][0] == 'F' && matrix[2][0] == 'A' && matrix[0][1] == 'G')
    {
        printf("\tSucesso: Symbols inseridos nas colunas corretas.\n");
    }
    else
    {
        printf("\tErro: Symbols incorretos nas colunas.\n");
    }
}

/**
 * @brief Testa a função polybius_encode_to_columns com uma string simples.
 *
 * Garante que os pares de símbolos gerados estão sendo corretamente organizados por coluna.
 */
void test_polybius_encode_to_columns()
{
    int key_length = 2;
    int max_per_column = MAX_MESSAGE_LENGTH;
    char message[] = "AB";
    char matrix[2][MAX_MESSAGE_LENGTH] = {{0}};
    int symbols_per_column[2] = {0};

    polybius_encode_to_columns(key_length, max_per_column, message, matrix, symbols_per_column);

    if (symbols_per_column[0] == 2 && symbols_per_column[1] == 2)
    {
        printf("\tSucesso: Mensagem 'AB' cifrada corretamente.\n");
    }
    else
    {
        printf("\tErro: Quantidade de symbols incorreta.\n");
    }
}

/**
 * @brief Testa a função transpose_columns_by_key_order.
 *
 * Usa uma chave fora de ordem alfabética para verificar se as colunas são corretamente reordenadas.
 */
void test_transpose_columns_by_key_order()
{
    char key[] = "CAB"; // ordem alfabética: A B C -> índices 2 1 0
    int key_length = 3;
    int max_per_column = MAX_MESSAGE_LENGTH;

    char matrix[3][MAX_MESSAGE_LENGTH] = {
        {'C', '1', '2'},
        {'A', '3', '4'},
        {'B', '5', '6'}};
    int symbols_per_column[3] = {3, 3, 3};

    transpose_columns_by_key_order(key, key_length, max_per_column, matrix, symbols_per_column);

    if (matrix[0][0] == 'A' && matrix[1][0] == 'B' && matrix[2][0] == 'C')
    {
        printf("\tSucesso: Colunas transpostas conforme ordem da chave.\n");
    }
    else
    {
        printf("\tErro: Esperado: A-B-C, Obtido: %c-%c-%c\n",
               matrix[0][0], matrix[1][0], matrix[2][0]);
    }
}

/**
 * @brief Testa a função de decifragem comparando com a mensagem original.
 *
 * @note Usamos key UM e a messagem LUCAS previamente testadas tanto no site https://www.dcode.fr/adfgvx-cipher, quanto realizando a cifragem manualmente.
    Cifra: Lucas -> XF FA AD GA AG
 */
void test_decipher(char key[], char original_message[])
{
    int key_length = strlen(key);
    int max_per_column = (2 * MAX_MESSAGE_LENGTH + key_length - 1) / key_length;

    // Declara VLA para armazenar os símbolos por coluna
    char encoded_symbol_matrix[key_length][max_per_column];
    int symbols_per_column[MAX_KEY_LENGTH] = {0};

    // Cifrar a mensagem
    cipher_adfgvx(key, key_length, max_per_column, original_message, encoded_symbol_matrix, symbols_per_column);

    // Linearizar mensagem cifrada
    char encrypted[2 * MAX_MESSAGE_LENGTH];
    int pos = 0;
    for (int i = 0; i < key_length; i++)
    {
        for (int j = 0; j < symbols_per_column[i]; j++)
        {
            encrypted[pos] = encoded_symbol_matrix[i][j];
            pos++;
        }
    }
    encrypted[pos] = '\0';

    // Decifrar
    char decrypted[MAX_MESSAGE_LENGTH];
    decipher_adfgvx(encrypted, key, key_length, decrypted);

    printf("\t\tMensagem original:   %.20s\n", original_message);
    printf("\t\tMensagem cifrada:    %.20s\n", encrypted);
    printf("\t\tMensagem decifrada:  %.20s\n", decrypted);

    if (strcmp(original_message, decrypted) == 0)
    {
        printf("\tSucesso: Mensagem decriptada == mensagem original!\n");
    }
    else
    {
        printf("\tErro: A decifragem falhou.\n");
    }
}

/**
 * @brief Mede o tempo de execução máximo da cifragem de uma mensagem longa e chave de tamanho máximo, tendo que ser menor que 0.5 segundos.
 */
void test_execution_time()
{
    char key[] = "CHAVE123";
    int key_length = strlen(key);
    char long_message[MAX_MESSAGE_LENGTH];
    memset(long_message, 'A', MAX_MESSAGE_LENGTH - 1);
    long_message[MAX_MESSAGE_LENGTH - 1] = '\0';

    int max_per_column = MAX_MESSAGE_LENGTH;
    char encoded_symbol_matrix[MAX_KEY_LENGTH][MAX_MESSAGE_LENGTH];
    int symbols_per_column[MAX_KEY_LENGTH] = {0};

    clock_t start = clock();
    cipher_adfgvx(key, key_length, max_per_column, long_message, encoded_symbol_matrix, symbols_per_column);
    clock_t end = clock();

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    if (elapsed > 0.5)
    {
        printf("\tErro: Execution Time excedeu 0.5 segundos! Tempo: %.6f segundos\n", elapsed);
        return;
    }
    else
    {
        printf("\tSucesso: Execution time dentro do limite de 0.5 segundos! Tempo: %.6f segundos\n", elapsed);
    }
}

/**
 * @brief Verifica se caracteres inválidos são ignorados durante a cifragem e valida a mensagem cifrada.
 */
void test_invalid_character()
{
    char key[] = "UM";
    int key_length = strlen(key);
    char message[] = "L#UC%AS@!d";
    int max_per_column = MAX_MESSAGE_LENGTH;
    char encoded_symbol_matrix[MAX_KEY_LENGTH][MAX_MESSAGE_LENGTH];
    int symbols_per_column[MAX_KEY_LENGTH] = {0};

    const char expected_cipher[] = "XFFAADGAAG";

    cipher_adfgvx(key, key_length, max_per_column, message, encoded_symbol_matrix, symbols_per_column);

    // Construir a mensagem cifrada linearizada
    char actual_cipher[MAX_MESSAGE_LENGTH] = {0};
    int pos = 0;
    for (int i = 0; i < key_length; i++)
    {
        for (int j = 0; j < symbols_per_column[i]; j++)
        {
            actual_cipher[pos++] = encoded_symbol_matrix[i][j];
        }
    }
    actual_cipher[pos] = '\0';

    printf("\t\tMensagem original: %s\n", message);
    printf("\t\tMensagem cifrada obtida: %s\n", actual_cipher);
    printf("\t\tMensagem cifrada esperada: %s\n", expected_cipher);

    if (strcmp(actual_cipher, expected_cipher) == 0)
    {
        printf("\tSucesso: Caracteres foram ignorados e a cifragem está correta.\n");
    }
    else
    {
        printf("\tErro: A mensagem cifrada está errada.\n");
    }
}

/**
 * @brief Função principal que executa os testes.
 */
int main()
{
    printf("Executando testes do algoritmo ADFGVX...\n");

    printf("\n-> Teste: get_adfgvx_symbols get C and reject get #\n");
    test_get_adfgvx_symbols();

    printf("\n-> Teste: insert_symbol_to_column insert symbol correctly in the column\n");
    test_insert_symbol_to_column();

    printf("\n-> Teste: polybius_encode_to_columns\n");
    test_polybius_encode_to_columns();

    printf("\n-> Teste: transpose_columns_by_key_order\n");
    test_transpose_columns_by_key_order();

    printf("\n-> Teste: Decrypting with known encrypting XFFAADGAAG \n");
    test_decipher("UM", "LUCAS");

    char message[MAX_MESSAGE_LENGTH] = {0};
    // Ler a messagem
    int is_file_read = read_file("./message.txt", message, MAX_MESSAGE_LENGTH);
    if (is_file_read != 0)
    {
        perror("Error reading file './message.txt'.");
        return 1;
    }

    printf("\n-> Teste: Decrypting encrypted file \n");
    test_decipher("SEMB2025", message);

    printf("\n-> Teste: Decrypting with medium text \n");
    test_decipher("SEMB2025", "LOREM IPSUM DOLOR SIT AMET, COMMODO VOLUTPAT. CURABITUR HENDRERIT CURSUS JUSTO, EGET PHARETRA TELLUS VULPUTATE QUIS. PELLENTESQUE ET JUSTO LEO. MAECENAS A EGESTAS ENIM, AC ULTRICES RISUS. UT ET PLACERAT MASSA. LOREM IPSUM DOLOR SIT AMET, CONSECTETUR ADIPISCING ELIT. INTEGER FRINGILLA FINIBUS AUGUE ID SODALES. NULLAM NON FAUCIBUS ANTE. IN PORTTITOR, NIBH ET MATTIS FERMENTUM, VELIT SAPIEN ULLAMCORPER AUGUE, NEC EGESTAS EROS ARCU ID SEM. PELLENTESQUE EU FRINGILLA EX, ID BLANDIT TURPIS. QUISQUE ELIT DOLOR, PORTTITOR A SAPIEN VITAE, MOLESTIE DICTUM TELLUS. SED CONSECTETUR EST NIBH, UT DICTUM EROS EGESTAS SIT AMET. SUSPENDISSE GRAVIDA NEQUE NISL, AT PORTTITOR URNA PORTTITOR ID. NUNC SIT AMET SAPIEN MI. SED POSUERE BLANDIT ENIM AC LUCTUS. PHASELLUS FACILISIS EGET ODIO AC POSUERE. DUIS RUTRUM BIBENDUM ODIO, VITAE VARIUS IPSUM LACINIA A. CRAS QUIS PRETIUM ANTE. DUIS AT AUGUE UT DUI ORNARE MAXIMUS. UT ID LIGULA SED ELIT CONSEQUAT PRETIUM PULVINAR A NISI. PELLENTESQUE DAPIBUS FEUGIAT MAURIS, VEL EGESTAS TORTOR IMPERDIET NON. DONEC TRISTIQUE MASSA NEC EX ELEIFEND VESTIBULUM. VIVAMUS MATTIS SIT AMET VELIT VEL FACILISIS. NULLA FACILISI. DONEC COMMODO QUAM EGET TINCIDUNT HENDRERIT. PROIN MASSA PURUS, CONSECTETUR AC EGESTAS ET, FINIBUS A NEQUE. MAURIS VEL GRAVIDA NISI, ID ELEMENTUM DIAM. SED UT MI LECTUS. AENEAN SCELERISQUE IPSUM MAURIS, NON EUISMOD EST VEHICULA SIT AMET. ALIQUAM NON MAURIS LOREM. NULLA EGESTAS ID MI AC TEMPOR. MORBI A QUAM NON NUNC TEMPUS HENDRERIT. MORBI AT URNA IPSUM. PROIN RHONCUS AUCTOR PURUS AT VESTIBULUM. ETIAM ENIM IPSUM, TEMPUS VEL ELEMENTUM ET, FERMENTUM UT DUI. ETIAM AT QUAM SIT AMET NUNC TEMPUS CONSEQUAT IN ID IPSUM. INTEGER IN TEMPOR LACUS. QUISQUE TINCIDUNT LACINIA ERAT, SED TEMPOR VELIT LOBORTIS IN. PROIN LACINIA DOLOR ANTE, ET ULLAMCORPER ERAT PULVINAR A. MORBI SUSCIPIT DIGNISSIM EROS, UT EFFICITUR DIAM CONVALLIS NEC. INTEGER LAOREET MAURIS VEL TELLUS ELEMENTUM, QUIS PORTA FELIS GRAVIDA. UT AC PURUS QUIS NISI DICTUM CURSUS IN NEC PURUS. PELLENTESQUE A RUTRUM TURPIS, LAOREET LAOREET URNA. DONEC A TELLUS EGET LACUS ALIQUAM VOLUTPAT ID LAOREET SEM. MAURIS UT NEQUE FINIBUS, MATTIS LECTUS AT, VOLUTPAT ORCI. ALIQUAM ERAT VOLUTPAT. UT TINCIDUNT LIBERO IN ANTE PORTA, VITAE TEMPOR EROS RHONCUS. MAURIS ENIM TORTOR, PRETIUM IN ORCI ID, ULTRICES ALIQUET PURUS. NULLAM VEL CURSUS DUI. NAM PRETIUM ULLAMCORPER IPSUM ID CONSEQUAT. INTEGER A QUAM HENDRERIT, DAPIBUS METUS NEC.");

    printf("\n-> Teste: Decrypting with long text \n");
    test_decipher("SEMB2025", "LOREM IPSUM DOLOR SIT AMET, CONSECTETUR ADIPISCING ELIT. CURABITUR NISI EROS, MAXIMUS A FACILISIS ID, ACCUMSAN NEC TORTOR. MORBI FACILISIS MAGNA SIT AMET TURPIS COMMODO VOLUTPAT. CURABITUR HENDRERIT CURSUS JUSTO, EGET PHARETRA TELLUS VULPUTATE QUIS. PELLENTESQUE ET JUSTO LEO. MAECENAS A EGESTAS ENIM, AC ULTRICES RISUS. UT ET PLACERAT MASSA. LOREM IPSUM DOLOR SIT AMET, CONSECTETUR ADIPISCING ELIT. INTEGER FRINGILLA FINIBUS AUGUE ID SODALES. NULLAM NON FAUCIBUS ANTE. IN PORTTITOR, NIBH ET MATTIS FERMENTUM, VELIT SAPIEN ULLAMCORPER AUGUE, NEC EGESTAS EROS ARCU ID SEM. PELLENTESQUE EU FRINGILLA EX, ID BLANDIT TURPIS. QUISQUE ELIT DOLOR, PORTTITOR A SAPIEN VITAE, MOLESTIE DICTUM TELLUS. SED CONSECTETUR EST NIBH, UT DICTUM EROS EGESTAS SIT AMET. SUSPENDISSE GRAVIDA NEQUE NISL, AT PORTTITOR URNA PORTTITOR ID. NUNC SIT AMET SAPIEN MI. SED POSUERE BLANDIT ENIM AC LUCTUS. PHASELLUS FACILISIS EGET ODIO AC POSUERE. DUIS RUTRUM BIBENDUM ODIO, VITAE VARIUS IPSUM LACINIA A. CRAS QUIS PRETIUM ANTE. DUIS AT AUGUE UT DUI ORNARE MAXIMUS. UT ID LIGULA SED ELIT CONSEQUAT PRETIUM PULVINAR A NISI. PELLENTESQUE DAPIBUS FEUGIAT MAURIS, VEL EGESTAS TORTOR IMPERDIET NON. DONEC TRISTIQUE MASSA NEC EX ELEIFEND VESTIBULUM. VIVAMUS MATTIS SIT AMET VELIT VEL FACILISIS. NULLA FACILISI. DONEC COMMODO QUAM EGET TINCIDUNT HENDRERIT. PROIN MASSA PURUS, CONSECTETUR AC EGESTAS ET, FINIBUS A NEQUE. MAURIS VEL GRAVIDA NISI, ID ELEMENTUM DIAM. SED UT MI LECTUS. AENEAN SCELERISQUE IPSUM MAURIS, NON EUISMOD EST VEHICULA SIT AMET. ALIQUAM NON MAURIS LOREM. NULLA EGESTAS ID MI AC TEMPOR. MORBI A QUAM NON NUNC TEMPUS HENDRERIT. MORBI AT URNA IPSUM. PROIN RHONCUS AUCTOR PURUS AT VESTIBULUM. ETIAM ENIM IPSUM, TEMPUS VEL ELEMENTUM ET, FERMENTUM UT DUI. ETIAM AT QUAM SIT AMET NUNC TEMPUS CONSEQUAT IN ID IPSUM. INTEGER IN TEMPOR LACUS. QUISQUE TINCIDUNT LACINIA ERAT, SED TEMPOR VELIT LOBORTIS IN. PROIN LACINIA DOLOR ANTE, ET ULLAMCORPER ERAT PULVINAR A. MORBI SUSCIPIT DIGNISSIM EROS, UT EFFICITUR DIAM CONVALLIS NEC. INTEGER LAOREET MAURIS VEL TELLUS ELEMENTUM, QUIS PORTA FELIS GRAVIDA. UT AC PURUS QUIS NISI DICTUM CURSUS IN NEC PURUS. PELLENTESQUE A RUTRUM TURPIS, LAOREET LAOREET URNA. DONEC A TELLUS EGET LACUS ALIQUAM VOLUTPAT ID LAOREET SEM. MAURIS UT NEQUE FINIBUS, MATTIS LECTUS AT, VOLUTPAT ORCI. ALIQUAM ERAT VOLUTPAT. UT TINCIDUNT LIBERO IN ANTE PORTA, VITAE TEMPOR EROS RHONCUS. MAURIS ENIM TORTOR, PRETIUM IN ORCI ID, ULTRICES ALIQUET PURUS. NULLAM VEL CURSUS DUI. NAM PRETIUM ULLAMCORPER IPSUM ID CONSEQUAT. INTEGER A QUAM HENDRERIT, DAPIBUS METUS NEC.");

    printf("\n-> Teste: Execution Time\n");
    test_execution_time();

    printf("\n-> Teste: Invalid characters \n");
    test_invalid_character();

    return 0;
}