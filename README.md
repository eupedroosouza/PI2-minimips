<h1 align="center">Simulador Mini-MIPS (8 bits)</h1>

> Esta documentação também pode ser encontrada no [reposítorio](https://github.com/eupedroosouza/PI2-minimips) do git.


## Build

Este guia descreve os passos para compilar o simulador utilizando o **CMake**.

### Pré-requisitos

* Compilador C (GCC, Clang ou MSVC)
* CMake (versão 3.10 ou superior)

### Processo de Build

Siga os comandos abaixo no seu terminal a partir da raiz do projeto:

1. **Crie o diretório de build:**
   ```bash
   mkdir build
   ```
2. **Acesse o diretório:**
   ```bash
   cd build
   ```
3. **Configure o projeto com o CMake:**
   ```bash
   cmake ..
   ```
4. **Compile o executável:**
   ```bash
   cmake --build .
   ```

## Execução

Após a compilação bem-sucedida, você pode executar o simulador com:

### Windows:

```bash
simulador-mini-mips-8bits.exe
```

### Linux:

Se precisar, dê as devidas permissões de execução:

 ```bash
chmod +x ./simulador-mini-mips-8bits
 ```

Execute o simulador:

```bash
./simulador-mini-mips-8bits
```

## Documentação

### Opções

1. **Carregar memória de instruções (.mem)**\
   Carrega a memória instruções via arquivo .mem informado pelo usuário. O arquivo deve conter as instruções no formato
   do Mini-MIPS com 16 bits e uma instrução por linha como:\
   **memoria2.mem:**
   ```
   0100000010000101
   0100000001000011
   0000010001011000
   0100000100001010
   1111000010000000
   1111000100000100
   1011000001000100
   1000000010001000
   1000000101000001
   1000011010001010
   0010000000000000
   ```
   Você pode encontrar algumas memórias de instruções
   em [static/memoria_instrucoes](https://github.com/eupedroosouza/PI2-minimips/tree/main/static/memorias_instrucoes).\
   Para mais informações sobre o formato de instruções
   acesse [MiniMIPS: An 8-Bit MIPS in an FPGA for Educational Purposes](https://ieeexplore.ieee.org/document/6128570/).
2. **Carregar memória de dados (.dat)**\
   Carrega a memória de dados via arquivo .dat informado pelo usuário. O arquivo deve conter um dado de até 8 bits (com
   sinal) de -128 até 127, linha por linha e obrigatoriamente os 256 valores da memória de dados, como:\
   **programa_k_vezes_n:**
   ```
   0
   0
   0
   0
   0
   0
   0
   0
   0
   0
   2
   3
   0
   0
   0
   .
   .
   .
   0 -> (posição 256) (texto demonstrativo, retire)
   ```
   Você pode encontrar algumas memórias de dados
   em [static/memoria_dados](https://github.com/eupedroosouza/PI2-minimips/tree/main/static/memoria_dados).
3. **Imprimir memórias (instruções e dados)**\
   Imprime a memória de instruções e dados.
4. **Imprimir banco de registradores**\
   Imprime o banco de registradores (inclusive o Progrma Counter (PC)).
5. **Imprimir todo o simulador (registradores e memórias)**\
   Imprime as memórias de instruções e dados e os registradores.
6. **Salvar instruçôes em mnemônicos - assembly (.asm)**\
   Salva as instruções da memória de dados em mnemônicos assembly do MIPS em caminho informado pelo usuário. (salva as 256 instruções - mesmo não "carregadas").
7. **Salvar memória de dados (.dat)**\
   Salva o estado atual da memória de dados em um arquivo .dat (todos os 256 dados).
8. **Executa programa (run)**\
   Executa o programa em sequência, a execução é infinita (o PC é redefinido para 0 quando o seu limite de 256 é atingido), você pode parar a execução do programa a qualquer momento pressionando a letra 'P'.
9. **Executa uma instrução (step)**\
   Executa apenas um ciclo de instrução.
10. **Voltar uma instrução (back)**\
   Volta a um ciclo anterior (apenas um) - reestabelece os registradores (inclusive PC) e a memória de dados para a última instrução executada.
11. **Exibir estatísticas de desempenho**\
   Exibe estatística de desempenho (número de instruções executadas, tempo gasto (assumindo 4ns como tempo de ciclo), entre outros).
12. **Resetar memória de instruções**\
   Limpa a memória de instruções.
13. **Resetar registradores**\
   Limpa os registradores, inclusive o PC.
14. **Resetar memória de dados**\
   Limpa a memória de dados.
15. **Ativar/desativar depuração (debug)**\
   Ativa/desativa a depuração do programa (pouco usado neste momento pelo simulador, pois a sua função foi substituída pelas opções de exibição)