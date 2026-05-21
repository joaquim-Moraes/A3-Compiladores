#pragma once
#include "ast.h"
#include "lexer.h"
#include <vector>
#include <memory>

// Parser recursivo simples que lê tokens e constrói a AST.
class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;
public:
    explicit Parser(std::vector<Token> t) : tokens(std::move(t)) {}
    std::unique_ptr<Node> parsePrograma();
private:
    Token current() const { return tokens[pos]; }
    Token peek() const { return pos + 1 < tokens.size() ? tokens[pos+1] : current(); }
    void advance() { pos++; }
    void expect(TokenType type);
    std::unique_ptr<Node> parseInstrucao();
    std::unique_ptr<Node> parseDeclaracao();
    std::unique_ptr<Node> parseAtribuicao();
    std::unique_ptr<Node> parseAtribuicaoSimples();
    std::unique_ptr<Node> parsePrintf();
    std::unique_ptr<Node> parsePrintfArg();
    std::unique_ptr<Node> parseComandoIfElse();
    std::unique_ptr<Node> parseComandoWhile();
    std::unique_ptr<Node> parseComandoFor();
    std::unique_ptr<Node> parseComandoDo();
    std::unique_ptr<Node> parseComandoReturn();
    std::unique_ptr<Node> parseBloco();
    std::unique_ptr<Node> parseExpressao();
    std::unique_ptr<Node> parseCondicao();
    std::unique_ptr<Node> parseSoma();
    std::unique_ptr<Node> parseTermo();
    std::unique_ptr<Node> parsePotencia();
    std::unique_ptr<Node> parseFator();
    std::string parseIdentificador();
};
