# Compilador Simples

Este projeto contém um compilador/interpreter didático em C++ que lê um arquivo fonte, realiza análise léxica, parsing e avalia a AST gerada.

## Como compilar

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

## Estrutura de arquivos

- `src/main.cpp`
  - Ponto de entrada do programa.
  - Lê o arquivo fonte, cria o lexer, gera tokens, chama o parser e executa o avaliador.

- `src/lexer.h`
  - Define o tipo `Token` e `TokenType`.
  - Declara o `Lexer` responsável por converter o texto fonte em tokens.

- `src/lexer.cpp`
  - Implementa o `Lexer`.
  - Identifica palavras-chave, identificadores, números, strings, operadores, delimitadores e comentários.

- `src/parser.h`
  - Declara a classe `Parser`.
  - Define os métodos de parsing para construir a AST a partir da lista de tokens.

- `src/parser.cpp`
  - Implementa o parser recursivo.
  - Constrói nós da AST para declarações, atribuições, `printf`, controle de fluxo e blocos.

- `src/ast.h`
  - Define os nós da árvore sintática abstrata (AST).
  - Inclui literais, referências a variáveis, operações binárias, declarações, blocos e estruturas de controle.

- `src/evaluator.h`
  - Declara o avaliador que executa a AST.
  - Mantém escopos aninhados para variáveis e tipos.

- `src/evaluator.cpp`
  - Implementa a execução da AST.
  - Avalia expressões, declarações, atribuições, `printf`, `if/else`, `while`, `for`, `do/while` e `return`.

- `src/teste.cpp`
  - Programa de exemplo separado que imprime "Olá, mundo!".

## Gramática suportada

- `programa ::= listaInstrucao`
- `listaInstrucao ::= instrucao+`
- `instrucao ::= declaracao | atribuicao | printf | comando`
- `declaracao ::= tipo identificador "=" expressao ";"`
- `atribuicao ::= identificador "=" expressao ";"`
- `tipo ::= "int" | "double" | "float"`
- `printf ::= "printf" "(" printf_arg ")" ";"`
- `printf_arg ::= expressao | identificador`
- `bloco ::= "{" instrucao* "}"`
- `comando ::= comandoIfElse | comandoWhile | comandoFor | comandoDo | comandoReturn`
- `comandoIfElse ::= "if" "(" expressao ")" bloco ("else" bloco)?`
- `comandoWhile ::= "while" "(" expressao ")" bloco`
- `comandoFor ::= "for" "(" atribuicao expressao ";" atribuicao ")" bloco`
- `comandoDo ::= "do" bloco "while" "(" expressao ")" ";"`
- `comandoReturn ::= "return" expressao? ";"`
- `expressao ::= soma | condicao`
- `condicao ::= expressao operadorRelacional expressao`
- `soma ::= termo (("+" | "-") termo)*`
- `termo ::= fator (("*" | "/") fator)*`
- `fator ::= numeroReal | identificador | "(" expressao ")"`

## Recursos disponíveis

- Declaração de variáveis com tipos `int`, `float` e `double`
- Atribuição de expressões a variáveis
- Expressões aritméticas com `+`, `-`, `*`, `/`
- Operadores relacionais `>`, `<`, `>=`, `<=`, `==`, `!=`
- `printf(...)` para exibir números ou strings
- `if/else`, `while`, `for`, `do/while`
- `return` para interromper a execução
- Comentários `//` até o fim da linha

## Exemplo de código

```c++
int i = 0;
for (i = 0; i < 5; i = i + 1) {
    printf(i);
}

if (i >= 5) {
    printf(i);
} else {
    printf(0);
}
```

## Observações

- O avaliador atual executa a AST em memória, sem gerar código intermediário.
- A análise de tipos é básica: variáveis são armazenadas por nome e as operações são avaliadas numericamente.
- A implementação suporta escopos de bloco simples para variáveis.

