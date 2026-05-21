#pragma once
#include "ast.h"
#include <map>
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>

struct ReturnException {
    bool hasValue;
    double value;
};

// Avaliador simples que executa a AST em memória.
// Ele armazena valores em escopos aninhados e suporta controle de fluxo.
class Evaluator {
    std::vector<std::map<std::string, double>> envStack;
    std::vector<std::map<std::string, std::string>> typeEnvStack;
public:
    Evaluator();
    void run(const std::unique_ptr<Node>& root);
    double eval(const Node* node);
private:
    void runInternal(const std::unique_ptr<Node>& root);
    void executeDeclaracao(const Declaracao* stmt);
    void executeAtribuicao(const Atribuicao* stmt);
    void executePrintf(const PrintfStmt* stmt);
    void executeIf(const ComandoIfElse* stmt);
    void executeWhile(const ComandoWhile* stmt);
    void executeFor(const ComandoFor* stmt);
    void executeDo(const ComandoDo* stmt);
    void executeReturn(const ComandoReturn* stmt);
    void pushScope();
    void popScope();
    double getVariable(const std::string& name) const;
    std::map<std::string, double>* findScope(const std::string& name);
};
