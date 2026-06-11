#include "parser.h"
#include <stdexcept>

void Parser::expect(TokenType type) {
    if (current().type != type) {
        std::string expected;
        switch(type) {
            case TokenType::ABRE_PARENTESE: expected = "'('"; break;
            case TokenType::FECHA_PARENTESE: expected = "')'"; break;
            case TokenType::PONTO_VIRGULA: expected = "';'"; break;
            case TokenType::ABRE_CHAVES: expected = "'{'"; break;
            case TokenType::FECHA_CHAVES: expected = "'}'"; break;
            case TokenType::INT: expected = "'int'"; break;
            case TokenType::FLOAT: expected = "'float'"; break;
            case TokenType::DOUBLE: expected = "'double'"; break;
            case TokenType::PRINTF: expected = "'printf'"; break;
            case TokenType::IF: expected = "'if'"; break;
            case TokenType::WHILE: expected = "'while'"; break;
            case TokenType::FOR: expected = "'for'"; break;
            case TokenType::DO: expected = "'do'"; break;
            case TokenType::RETURN: expected = "'return'"; break;
            default: expected = std::to_string((int)type);
        }
        throw std::runtime_error("Erro de sintaxe na linha " + std::to_string(current().line) +
                                 ": esperado " + expected +
                                 " mas encontrou '" + current().value + "'");
    }
    advance();
}

std::unique_ptr<Node> Parser::parsePrograma() {
    std::vector<std::unique_ptr<Node>> stmts;
    while (current().type != TokenType::FIM) {
        stmts.push_back(parseInstrucao());
    }
    return std::make_unique<Programa>(std::move(stmts));
}

std::unique_ptr<Node> Parser::parseInstrucao() {
    if (current().type == TokenType::INT || current().type == TokenType::FLOAT || current().type == TokenType::DOUBLE)
        return parseDeclaracao();
    if (current().type == TokenType::IDENTIFICADOR)
        return parseAtribuicao();
    if (current().type == TokenType::PRINTF)
        return parsePrintf();
    if (current().type == TokenType::IF)
        return parseComandoIfElse();
    if (current().type == TokenType::WHILE)
        return parseComandoWhile();
    if (current().type == TokenType::FOR)
        return parseComandoFor();
    if (current().type == TokenType::DO)
        return parseComandoDo();
    if (current().type == TokenType::RETURN)
        return parseComandoReturn();
    throw std::runtime_error("Instrucao invalida na linha " + std::to_string(current().line));
}

std::unique_ptr<Node> Parser::parseDeclaracao() {
    std::string typeStr;
    if (current().type == TokenType::INT) { typeStr = "int"; advance(); }
    else if (current().type == TokenType::FLOAT) { typeStr = "float"; advance(); }
    else if (current().type == TokenType::DOUBLE) { typeStr = "double"; advance(); }
    else throw std::runtime_error("Tipo invalido na linha " + std::to_string(current().line));

    auto name = parseIdentificador();
    expect(TokenType::ATRIBUICAO);
    auto init = parseExpressao();
    expect(TokenType::PONTO_VIRGULA);
    return std::make_unique<Declaracao>(typeStr, name, std::move(init));
}

std::unique_ptr<Node> Parser::parseAtribuicao() {
    auto name = parseIdentificador();
    expect(TokenType::ATRIBUICAO);
    auto expr = parseExpressao();
    expect(TokenType::PONTO_VIRGULA);
    return std::make_unique<Atribuicao>(name, std::move(expr));
}

std::unique_ptr<Node> Parser::parsePrintf() {
    expect(TokenType::PRINTF);
    expect(TokenType::ABRE_PARENTESE);
    auto arg = parsePrintfArg();
    expect(TokenType::FECHA_PARENTESE);
    expect(TokenType::PONTO_VIRGULA);
    return std::make_unique<PrintfStmt>(std::move(arg));
}

std::unique_ptr<Node> Parser::parsePrintfArg() {
    if (current().type == TokenType::CADEIA) {
        auto s = std::make_unique<CadeiaLiteral>(current().value);
        advance();
        return s;
    }
    return parseExpressao();
}

std::unique_ptr<Node> Parser::parseComandoIfElse() {
    expect(TokenType::IF);
    expect(TokenType::ABRE_PARENTESE);
    auto cond = parseExpressao();
    expect(TokenType::FECHA_PARENTESE);
    auto thenBlock = parseBloco();
    std::unique_ptr<Node> elseBlock = nullptr;
    if (current().type == TokenType::ELSE) {
        advance();
        elseBlock = parseBloco();
    }
    return std::make_unique<ComandoIfElse>(std::move(cond), std::move(thenBlock), std::move(elseBlock));
}

std::unique_ptr<Node> Parser::parseComandoWhile() {
    expect(TokenType::WHILE);
    expect(TokenType::ABRE_PARENTESE);
    auto cond = parseExpressao();
    expect(TokenType::FECHA_PARENTESE);
    auto body = parseBloco();
    return std::make_unique<ComandoWhile>(std::move(cond), std::move(body));
}

std::unique_ptr<Node> Parser::parseAtribuicaoSimples() {
    auto name = parseIdentificador();
    expect(TokenType::ATRIBUICAO);
    auto expr = parseExpressao();
    return std::make_unique<Atribuicao>(name, std::move(expr));
}

std::unique_ptr<Node> Parser::parseComandoFor() {
    expect(TokenType::FOR);
    expect(TokenType::ABRE_PARENTESE);
    std::unique_ptr<Node> init = nullptr;
    // init: apenas declaracao
    if (current().type == TokenType::INT || current().type == TokenType::FLOAT || current().type == TokenType::DOUBLE) {
        init = parseDeclaracao();
    } else {
        throw std::runtime_error("Esperado declaracao no init do for na linha " + std::to_string(current().line));
    }

    // condition: obrigatoria (expressao)
    auto cond = parseExpressao();
    expect(TokenType::PONTO_VIRGULA);

    // post: can be assignment, ++/-- on identifier, or empty
    std::unique_ptr<Node> post = nullptr;
    if (current().type == TokenType::FECHA_PARENTESE) {
        // empty post
    } else {
        if (current().type == TokenType::IDENTIFICADOR && peek().type == TokenType::MAIS_MAIS) {
            // convert i++ -> i = i + 1
            std::string name = current().value;
            advance(); // consume identifier
            advance(); // consume ++
            post = std::make_unique<Atribuicao>(name,
                std::make_unique<OperacaoBinaria>("+",
                    std::make_unique<Identificador>(name),
                    std::make_unique<NumeroLiteral>(1.0)
                )
            );
        } else if (current().type == TokenType::IDENTIFICADOR && peek().type == TokenType::MENOS_MENOS) {
            // convert i-- -> i = i - 1
            std::string name = current().value;
            advance(); // consume identifier
            advance(); // consume --
            post = std::make_unique<Atribuicao>(name,
                std::make_unique<OperacaoBinaria>("-",
                    std::make_unique<Identificador>(name),
                    std::make_unique<NumeroLiteral>(1.0)
                )
            );
        } else {
            post = parseAtribuicaoSimples();
        }
    }
    expect(TokenType::FECHA_PARENTESE);
    auto body = parseBloco();
    return std::make_unique<ComandoFor>(std::move(init), std::move(cond), std::move(post), std::move(body));
}

std::unique_ptr<Node> Parser::parseComandoDo() {
    expect(TokenType::DO);
    auto body = parseBloco();
    expect(TokenType::WHILE);
    expect(TokenType::ABRE_PARENTESE);
    auto cond = parseExpressao();
    expect(TokenType::FECHA_PARENTESE);
    expect(TokenType::PONTO_VIRGULA);
    return std::make_unique<ComandoDo>(std::move(body), std::move(cond));
}

std::unique_ptr<Node> Parser::parseComandoReturn() {
    expect(TokenType::RETURN);
    std::unique_ptr<Node> expr = nullptr;
    if (current().type != TokenType::PONTO_VIRGULA) {
        expr = parseExpressao();
    }
    expect(TokenType::PONTO_VIRGULA);
    return std::make_unique<ComandoReturn>(std::move(expr));
}

std::unique_ptr<Node> Parser::parseBloco() {
    expect(TokenType::ABRE_CHAVES);
    std::vector<std::unique_ptr<Node>> stmts;
    while (current().type != TokenType::FECHA_CHAVES && current().type != TokenType::FIM) {
        stmts.push_back(parseInstrucao());
    }
    expect(TokenType::FECHA_CHAVES);
    return std::make_unique<Bloco>(std::move(stmts));
}

std::unique_ptr<Node> Parser::parseExpressao() {
    return parseCondicao();
}

std::unique_ptr<Node> Parser::parseCondicao() {
    auto left = parseSoma();
    while (current().type == TokenType::MAIOR || current().type == TokenType::MENOR ||
           current().type == TokenType::MAIOR_IGUAL || current().type == TokenType::MENOR_IGUAL ||
           current().type == TokenType::IGUAL_IGUAL || current().type == TokenType::DIFERENTE) {
        auto op = current().value;
        advance();
        auto right = parseSoma();
        left = std::make_unique<OperacaoBinaria>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::parseSoma() {
    auto left = parseTermo();
    while (current().type == TokenType::MAIS || current().type == TokenType::MENOS) {
        auto op = current().value;
        advance();
        auto right = parseTermo();
        left = std::make_unique<OperacaoBinaria>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::parseTermo() {
    auto left = parsePotencia();
    while (current().type == TokenType::ASTERISCO || current().type == TokenType::BARRA) {
        auto op = current().value;
        advance();
        auto right = parsePotencia();
        left = std::make_unique<OperacaoBinaria>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::parsePotencia() {
    auto left = parseFator();
    if (current().type == TokenType::POTENCIA) {
        auto op = current().value;
        advance();
        auto right = parsePotencia();
        return std::make_unique<OperacaoBinaria>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::parseFator() {
    if (current().type == TokenType::NUMERO) {
        auto lit = std::make_unique<NumeroLiteral>(std::stod(current().value));
        advance();
        return lit;
    }
    if (current().type == TokenType::IDENTIFICADOR) {
        auto ref = std::make_unique<Identificador>(current().value);
        advance();
        return ref;
    }
    if (current().type == TokenType::ABRE_PARENTESE) {
        advance();
        auto expr = parseExpressao();
        expect(TokenType::FECHA_PARENTESE);
        return expr;
    }
    throw std::runtime_error("Fator invalido na linha " + std::to_string(current().line));
}

std::string Parser::parseIdentificador() {
    if (current().type != TokenType::IDENTIFICADOR)
        throw std::runtime_error("Esperado identificador na linha " + std::to_string(current().line));
    auto name = current().value;
    advance();
    return name;
}
