#pragma once
#include <string>
#include <vector>

// Tipos de token reconhecidos pelo lexer.
enum class TokenType {
    INT, FLOAT, DOUBLE, PRINTF,
    IF, ELSE, WHILE, FOR, DO, RETURN,
    IDENTIFICADOR, NUMERO, CADEIA,
    MAIS, MENOS, ASTERISCO, BARRA, POTENCIA,
    MAIS_MAIS, MENOS_MENOS,
    ATRIBUICAO, PONTO_VIRGULA, ABRE_PARENTESE, FECHA_PARENTESE,
    ABRE_CHAVES, FECHA_CHAVES,
    MAIOR, MENOR, MAIOR_IGUAL, MENOR_IGUAL, IGUAL_IGUAL, DIFERENTE,
    FIM
};

struct Token {
    TokenType type;        // Tipo do token identificado.
    std::string value;     // Texto original do token.
    int line;              // Linha onde o token foi encontrado.
};

// Lexer simples que converte texto fonte em tokens.
// Ele descarta espaços em branco e comentários, reconhece palavras-chave,
// identificadores, números, strings, operadores e símbolos.
class Lexer {
    std::string src;
    size_t pos = 0;
    int line = 1;
public:
    explicit Lexer(std::string source) : src(std::move(source)) {}
    Token nextToken();
private:
    void skipWhitespace();
    void skipComment();
    std::string readIdentifier();
    std::string readNumber();
    std::string readString();
    bool isKeyword(const std::string& s);
};
