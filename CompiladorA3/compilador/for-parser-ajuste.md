# Ajuste no parser do `for`

## Problema atual

A função `Parser::parseComandoFor()` no `src/parser.cpp` analisa o laço `for` com uma regra rígida:

- `for`
- `(`
- `init` obrigatoriamente como atribuição simples (`identificador = expressão`)
- `;`
- `cond` como expressão
- `;`
- `post` obrigatoriamente como atribuição simples (`identificador = expressão`)
- `)`
- `body` como bloco `{ ... }`

Isso significa que o parser só aceita `for` de uma forma muito limitada, como:

```c
for (i = 0; i < 10; i = i + 1) {
    ...
}
```

## Limitações do código atual

1. `for (int i = 0; i < 10; i = i + 1)` não é aceito porque o init do `for` é uma declaração, não uma atribuição simples.
2. `for (i = 0; i < 10; i++)` não é aceito porque o parser espera uma atribuição com `=` e o lexer não reconhece `++`.
3. `for (; i < 10; i = i + 1)` não é aceito porque o init não pode ser vazio.
4. `for (i = 0; ; i = i + 1)` não é aceito porque a condição não pode ser vazia.

## Por que isso acontece

A função `parseAtribuicaoSimples()` tem esta lógica:

```cpp
std::unique_ptr<Node> Parser::parseAtribuicaoSimples() {
    auto name = parseIdentificador();
    expect(TokenType::ATRIBUICAO);
    auto expr = parseExpressao();
    return std::make_unique<Atribuicao>(name, std::move(expr));
}
```

Ou seja, ela aceita apenas `nome = expressão`.

## Ajuste recomendado

Para suportar mais formas de `for`, você pode alterar `parseComandoFor()` e o parser/lexer associados de duas maneiras principais:

1. Permitir `init` e `post` vazios.
2. Aceitar declarações no `init`.
3. Reconhecer `++` e `--` no lexer e no parser, se quiser suporte a incrementos pós e pré.
4. Permitir que a condição seja opcional.

### Exemplo de `parseComandoFor()` mais flexível

```cpp
std::unique_ptr<Node> Parser::parseComandoFor() {
    expect(TokenType::FOR);
    expect(TokenType::ABRE_PARENTESE);

    std::unique_ptr<Node> init = nullptr;
    if (current().type != TokenType::PONTO_VIRGULA) {
        if (current().type == TokenType::INT || current().type == TokenType::FLOAT || current().type == TokenType::DOUBLE) {
            init = parseDeclaracao();
        } else {
            init = parseAtribuicaoSimples();
            expect(TokenType::PONTO_VIRGULA);
        }
    } else {
        expect(TokenType::PONTO_VIRGULA);
    }

    std::unique_ptr<Node> cond = nullptr;
    if (current().type != TokenType::PONTO_VIRGULA) {
        cond = parseExpressao();
    }
    expect(TokenType::PONTO_VIRGULA);

    std::unique_ptr<Node> post = nullptr;
    if (current().type != TokenType::FECHA_PARENTESE) {
        post = parseAtribuicaoSimples();
    }
    expect(TokenType::FECHA_PARENTESE);

    auto body = parseBloco();
    return std::make_unique<ComandoFor>(std::move(init), std::move(cond), std::move(post), std::move(body));
}
```

> Observação: o trecho acima é um exemplo de como tornar a sintaxe do `for` menos restritiva. Ele não lida com `i++` nem com outros operadores além de atribuição simples.

## Passos seguintes

- Se quiser suporte a `for` com declaração inicial (`int i = 0`), mantenha `parseDeclaracao()` disponível para o `init`.
- Se quiser suporte a `for` com `i++` ou `i--`, adicione os tokens `++` e `--` no lexer e um nó AST para incrementos.
- Se quiser permitir `for (; ; )`, mantenha `init`, `cond` e `post` como ponteiros nulos.

## Conclusão

O problema não está na palavra-chave `for`, mas no fato de que `parseComandoFor()` só aceita `init` e `post` como atribuições simples, e não permite init/cond/post vazios ou declaração dentro do `for`.

Para avançar, o ajuste deve ser feito no parser (`parseComandoFor()`) e, se necessário, no lexer para reconhecer operadores de incremento.


## For antigo 
```cpp
std::unique_ptr<Node> Parser::parseComandoFor() {
    expect(TokenType::FOR);
    expect(TokenType::ABRE_PARENTESE);
    auto init = parseAtribuicaoSimples();
    expect(TokenType::PONTO_VIRGULA);
    auto cond = parseExpressao();
    expect(TokenType::PONTO_VIRGULA);
    auto post = parseAtribuicaoSimples();
    expect(TokenType::FECHA_PARENTESE);
    auto body = parseBloco();
    return std::make_unique<ComandoFor>(std::move(init), std::move(cond), std::move(post), std::move(body));
}
```