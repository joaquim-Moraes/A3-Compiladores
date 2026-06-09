#include "lexer.h"
#include <cctype>
#include <stdexcept>

bool Lexer::isKeyword(const std::string& s) {
    if (s == "int") return true;
    if (s == "float") return true;
    if (s == "double") return true;
    if (s == "printf") return true;
    if (s == "if") return true;
    if (s == "else") return true;
    if (s == "while") return true;
    if (s == "for") return true;
    if (s == "do") return true;
    if (s == "return") return true;
    return false;
}

Token Lexer::nextToken() {
    while (true) {
        skipWhitespace();
        if (pos >= src.size()) return {TokenType::FIM, "", line};
        if (src[pos] == '/' && pos + 1 < src.size() && src[pos+1] == '/') {
            skipComment();
            continue;
        }
        break;
    }

    if (pos >= src.size()) return {TokenType::FIM, "", line};
    char c = src[pos];
    Token tok; tok.line = line;

    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        tok.value = readIdentifier();
        if (tok.value == "int") tok.type = TokenType::INT;
        else if (tok.value == "float") tok.type = TokenType::FLOAT;
        else if (tok.value == "double") tok.type = TokenType::DOUBLE;
        else if (tok.value == "printf") tok.type = TokenType::PRINTF;
        else if (tok.value == "if") tok.type = TokenType::IF;
        else if (tok.value == "else") tok.type = TokenType::ELSE;
        else if (tok.value == "while") tok.type = TokenType::WHILE;
        else if (tok.value == "for") tok.type = TokenType::FOR;
        else if (tok.value == "do") tok.type = TokenType::DO;
        else if (tok.value == "return") tok.type = TokenType::RETURN;
        else tok.type = TokenType::IDENTIFICADOR;
    } else if (std::isdigit(static_cast<unsigned char>(c))) {
        tok.value = readNumber();
        tok.type = TokenType::NUMERO;
    } else if (c == '"') {
        tok.value = readString();
        tok.type = TokenType::CADEIA;
    } else {
        switch (c) {
            case '+':
                if (pos + 1 < src.size() && src[pos+1] == '+') {
                    tok.type = TokenType::MAIS_MAIS;
                    tok.value = "++";
                    pos += 2;
                } else {
                    tok.type = TokenType::MAIS;
                    tok.value = "+";
                    pos++;
                }
                break;
            case '-':
                if (pos + 1 < src.size() && src[pos+1] == '-') {
                    tok.type = TokenType::MENOS_MENOS;
                    tok.value = "--";
                    pos += 2;
                } else {
                    tok.type = TokenType::MENOS;
                    tok.value = "-";
                    pos++;
                }
                break;
            case '*': tok.type = TokenType::ASTERISCO; tok.value = "*"; pos++; break;
            case '/': tok.type = TokenType::BARRA; tok.value = "/"; pos++; break;
            case '^': tok.type = TokenType::POTENCIA; tok.value = "^"; pos++; break;
            case ';': tok.type = TokenType::PONTO_VIRGULA; tok.value = ";"; pos++; break;
            case '(': tok.type = TokenType::ABRE_PARENTESE; tok.value = "("; pos++; break;
            case ')': tok.type = TokenType::FECHA_PARENTESE; tok.value = ")"; pos++; break;
            case '{': tok.type = TokenType::ABRE_CHAVES; tok.value = "{"; pos++; break;
            case '}': tok.type = TokenType::FECHA_CHAVES; tok.value = "}"; pos++; break;
            case '>':
                pos++;
                if (pos < src.size() && src[pos] == '=') {
                    tok.type = TokenType::MAIOR_IGUAL;
                    tok.value = ">=";
                    pos++;
                } else {
                    tok.type = TokenType::MAIOR;
                    tok.value = ">";
                }
                break;
            case '<':
                pos++;
                if (pos < src.size() && src[pos] == '=') {
                    tok.type = TokenType::MENOR_IGUAL;
                    tok.value = "<=";
                    pos++;
                } else {
                    tok.type = TokenType::MENOR;
                    tok.value = "<";
                }
                break;
            case '=':
                pos++;
                if (pos < src.size() && src[pos] == '=') {
                    tok.type = TokenType::IGUAL_IGUAL;
                    tok.value = "==";
                    pos++;
                } else {
                    tok.type = TokenType::ATRIBUICAO;
                    tok.value = "=";
                }
                break;
            case '!':
                pos++;
                if (pos < src.size() && src[pos] == '=') {
                    tok.type = TokenType::DIFERENTE;
                    tok.value = "!=";
                    pos++;
                } else {
                    throw std::runtime_error("Caractere invalido na linha " + std::to_string(line));
                }
                break;
            default:
                throw std::runtime_error("Caractere invalido na linha " + std::to_string(line));
        }
    }
    return tok;
}

void Lexer::skipWhitespace() {
    while (pos < src.size() && std::isspace(static_cast<unsigned char>(src[pos]))) {
        if (src[pos] == '\n') line++;
        pos++;
    }
}

void Lexer::skipComment() {
    pos += 2; // pula '//'
    while (pos < src.size() && src[pos] != '\n') pos++;
}

std::string Lexer::readIdentifier() {
    size_t start = pos;
    while (pos < src.size() && (std::isalnum(static_cast<unsigned char>(src[pos])) || src[pos] == '_')) pos++;
    return src.substr(start, pos - start);
}

std::string Lexer::readNumber() {
    size_t start = pos;
    bool hasDot = false;
    while (pos < src.size() && (std::isdigit(static_cast<unsigned char>(src[pos])) || (!hasDot && src[pos] == '.'))) {
        if (src[pos] == '.') hasDot = true;
        pos++;
    }
    return src.substr(start, pos - start);
}

std::string Lexer::readString() {
    pos++; // pula a aspa de abertura
    size_t start = pos;
    while (pos < src.size() && src[pos] != '"') pos++;
    std::string s = src.substr(start, pos - start);
    pos++; // pula a aspa de fechamento
    return s;
}