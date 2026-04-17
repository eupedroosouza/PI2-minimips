<h1 align="center">Simulador Mini-MIPS (8 bits)</h1>

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
