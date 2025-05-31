# 🔐 Cifra ADFGVX

Implementação em C do algoritmo de cifra **ADFGVX**, com suporte a testes automatizados, leitura de arquivos e decodificação reversa.

---

## 📥 Entradas e Saídas

### 📄 Arquivos de Entrada

* `./src/message.txt`: Contém a mensagem a ser cifrada com no máximo 2560 letras, contendo apenas letras maiúsculas, espaços, virgula e ponto. Letras inválidas serão consideradas ruídos.
* `./src/key.txt`: Contém a chave de transposição de no máximo 8 caracteres.

### 📄 Saída Gerada

* `./src/encrypted.txt`: Resultado da cifragem ADFGVX, um texto com o dobro do tamanho da mensagem original, contendo apenas os caracteres ADFGVX.

### 🧪 Exemplo

**Mensagem de entrada (`./src/message.txt`):**

```txt
LUCAS
```

**Chave (`/src/key.txt`):**

```txt
UM
```

**Saída cifrada (`./src/encrypted.txt`):**

```txt
XFFAADGAAG
```

---

## ✅ Procedimentos de Teste

O arquivo `./src/main_test.c` contém testes automatizados que validam o funcionamento da cifra e decifra.

### 🔬 Testes disponíveis

| Teste                                   | Descrição                                                           |
| --------------------------------------- | ------------------------------------------------------------------- |
| `test_decipher()`                       | Verifica se uma mensagem cifrada pode ser decifrada corretamente.   |
| `test_invalid_character()`              | Testa o comportamento com caracteres inválidos (ex: `@`, `!`, etc). |
| `test_get_adfgvx_symbols()`             | Verifica se a função retorna os pares de símbolos esperados.        |
| `test_insert_symbol_to_column()`        | Garante que os símbolos sejam inseridos corretamente nas colunas.   |
| `test_polybius_encode_to_columns()`     | Testa a distribuição dos pares ADFGVX entre as colunas.             |
| `test_transpose_columns_by_key_order()` | Verifica se as colunas são reordenadas corretamente.                |
| `test_execution_time()`                 | Mede o tempo de execução da cifra para avaliação de desempenho.     |


## 🧱 Estruturas de Dados Utilizadas

### 📌 Principais estruturas no código:

* `char square[6][6]`: Matriz Polybius usada para substituição de caracteres.
* `char encoded_symbol_matrix[key_length][MAX_MESSAGE_LENGTH]`: Armazena os pares ADFGVX organizados por coluna.
* `char message[MAX_MESSAGE_LENGTH]`: Armazena a messagem para ser criptografada.
* `int symbols_per_column[MAX_KEY_LENGTH]`: Guarda o número de elementos em cada coluna para ordenação.
* `char cipher_key[MAX_KEY_LENGTH]`: Armazena a chave de transposição lida do arquivo.

### 🔄 Fluxo de dados

1. **Leitura da mensagem e chave**.
2. **Substituição por pares ADFGVX (com base na matriz Polybius)**.
3. **Distribuição dos pares em colunas com base na chave**.
4. **Transposição das colunas por ordem alfabética da chave**.
5. **Escrita da mensagem cifrada no arquivo de saída**.

---
