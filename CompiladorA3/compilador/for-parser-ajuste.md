# Ajuste no parser do `for`

## Problema atual

A função `Parser::parseComandoFor()` no `src/parser.cpp` analisa o laço `for` com uma regra rígida:

- `for`
- `(`
- `init` obrigatoriamente como declaração simples (`int identificador = expressão`) ou, em implementações menos flexíveis, como atribuição simples (`identificador = expressão`)
- `;`
- `cond` como expressão
- `;`
- `post` como atribuição simples (`identificador = expressão`), incremento (`i++` / `i--`) ou vazio
- `)`
- `body` como bloco `{ ... }`

Isso significa que o parser só aceita `for` de uma forma muito limitada, como:

```c
for (i = 0; i < 10; i = i + 1) {
    ...
}
```

## Limitações do código atual

1. `for (int i = 0; i < 10; i = i + 1)` não é aceito quando o parser espera apenas atribuição simples no `init` e não reconhece declarações.
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
    if (current().type == TokenType::INT || current().type == TokenType::FLOAT || current().type == TokenType::DOUBLE) {
        // parseDeclaracao consome o ';' interno do for
        init = parseDeclaracao();
    } else {
        throw std::runtime_error("Esperado declaracao no init do for na linha " + std::to_string(current().line));
    }

    auto cond = parseExpressao();
    expect(TokenType::PONTO_VIRGULA);

    std::unique_ptr<Node> post = nullptr;
    if (current().type == TokenType::FECHA_PARENTESE) {
        // post vazio
    } else if (current().type == TokenType::IDENTIFICADOR && peek().type == TokenType::MAIS_MAIS) {
        // converte i++ para i = i + 1
        std::string name = current().value;
        advance(); // consome identificador
        advance(); // consome ++
        post = std::make_unique<Atribuicao>(name,
            std::make_unique<OperacaoBinaria>("+",
                std::make_unique<Identificador>(name),
                std::make_unique<NumeroLiteral>(1.0)
            )
        );
    } else if (current().type == TokenType::IDENTIFICADOR && peek().type == TokenType::MENOS_MENOS) {
        // converte i-- para i = i - 1
        std::string name = current().value;
        advance(); // consome identificador
        advance(); // consome --
        post = std::make_unique<Atribuicao>(name,
            std::make_unique<OperacaoBinaria>("-",
                std::make_unique<Identificador>(name),
                std::make_unique<NumeroLiteral>(1.0)
            )
        );
    } else {
        post = parseAtribuicaoSimples();
    }
    expect(TokenType::FECHA_PARENTESE);

    auto body = parseBloco();
    return std::make_unique<ComandoFor>(std::move(init), std::move(cond), std::move(post), std::move(body));
}
```

> Observação: o trecho acima mostra como o `init` pode ser uma declaração do tipo `int`, `float` ou `double` dentro do `for`. O parser atual já precisa de `parseDeclaracao()` para consumir o ponto e vírgula interno do `for`.

> Observação: o trecho acima é um exemplo de como tornar a sintaxe do `for` menos restritiva. Ele não lida com `i++` nem com outros operadores além de atribuição simples.


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