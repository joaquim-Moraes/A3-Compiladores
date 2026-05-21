# Tutorial de Instalação do MSYS2 no Windows para C/C++

## 1. Objetivo

Este tutorial apresenta, em sequência, o processo de instalação e configuração do **MSYS2** no Windows para desenvolvimento em **C/C++**, com foco no uso do compilador **g++**, depuração com **gdb** e suporte a projetos maiores com **CMake** e **Ninja**.

## 2. O que é o MSYS2

O **MSYS2** é um ambiente de desenvolvimento para Windows que fornece:

- terminal estilo Unix
- gerenciador de pacotes `pacman`
- compiladores GCC/G++
- ferramentas como `gdb`, `git`, `cmake` e `ninja`

Para desenvolvimento em C/C++ no Windows.

## 3. Requisitos

Antes da instalação, verifique se o sistema é compatível:

- **Windows 10 64 bits** ou superior
- conexão com a internet para baixar pacotes

## 4. Download do instalador

Acesse o site oficial do MSYS2 e baixe o instalador mais recente.

Durante a instalação, use o caminho padrão:

```text
C:\msys64
```

## 5. Instalação

Execute o instalador e avance normalmente.

Recomendação:

* mantenha o diretório padrão `C:\msys64`
* conclua a instalação sem alterar opções desnecessárias

## 6. Abrir o terminal correto

Após a instalação, no menu Iniciar, procure e abra o terminal MSYS2 UCRT64.

Esse será o ambiente recomendado para desenvolvimento em C/C++.

## 7. Atualização inicial do sistema

No terminal **MSYS2 UCRT64**, execute:

```bash
pacman -Syu
```

Se o terminal pedir para ser fechado após a atualização inicial:

1. feche o terminal
2. abra novamente o **MSYS2 UCRT64**
3. rode novamente o comando:

```bash
pacman -Syu
```

Esse passo é importante porque o MSYS2 costuma exigir uma atualização em etapas na primeira execução.

## 8. Instalação do compilador C/C++

Depois da atualização, instale o compilador GCC/G++ com o comando:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
```

Esse pacote instala o compilador C e C++ do ambiente UCRT64.

## 9. Instalação de ferramentas úteis

Para deixar o ambiente mais completo, instale também:

```bash
pacman -S mingw-w64-ucrt-x86_64-gdb mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja make git
```

Essas ferramentas serão úteis para:

* depuração com `gdb`
* build de projetos com `cmake`
* builds rápidos com `ninja`
* controle de versão com `git`

## 10. Verificação da instalação

Depois da instalação, teste os comandos:

```bash
g++ --version
where g++
```

Resultado esperado:

* exibição da versão atual do `g++`
* caminho semelhante a:

```text
C:\msys64\ucrt64\bin\g++.exe
```

## 11. Ajuste do PATH no Windows

Se o executável compilado funcionar no terminal do MSYS2, mas falhar ao ser executado diretamente pelo Windows com erro de DLL, adicione ao `PATH` do Windows:

```text
C:\msys64\ucrt64\bin
```

### Como fazer

1. pesquise por **Variáveis de Ambiente**
2. abra **Editar as variáveis de ambiente do sistema**
3. clique em **Variáveis de Ambiente**
4. selecione a variável **Path**
5. adicione:

```text
C:\msys64\ucrt64\bin
```

6. confirme e reinicie o terminal ou o VS Code


## 12. Sequência completa de instalação no MSYS2

```bash
pacman -Syu
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-gdb mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja make git
g++ --version
where g++
```
