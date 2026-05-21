#include "lexer.h"
#include "parser.h"
#include "evaluator.h"
#include "ast.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

// Programa principal do compilador simplificado.
// 1) Recebe o nome do arquivo fonte como argumento.
// 2) Lê todo o conteúdo do arquivo para uma string.
// 3) Cria o lexer para gerar os tokens.
// 4) Passa os tokens para o parser para construir a AST.
// 5) Executa a AST com o avaliador.
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.cml>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo: " << filename << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    try {
        Lexer lexer(buffer.str());

        std::vector<Token> tokens;
        Token t;
        do {
            t = lexer.nextToken();
            tokens.push_back(t);
        } while (t.type != TokenType::FIM);

        Parser parser(std::move(tokens));
        auto ast = parser.parsePrograma();

        Evaluator evaluator;
        evaluator.run(ast);

    } catch (const std::exception& e) {
        std::cerr << "ERRO: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
