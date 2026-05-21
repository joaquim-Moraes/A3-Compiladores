#include "evaluator.h"
#include <stdexcept>
#include <cmath>
#include <iostream>

Evaluator::Evaluator() {
    pushScope();
}

void Evaluator::run(const std::unique_ptr<Node>& root) {
    if (!root) return;
    try {
        runInternal(root);
    } catch (const ReturnException&) {
        // Return no nível superior apenas interrompe a execução.
    }
}

void Evaluator::runInternal(const std::unique_ptr<Node>& root) {
    if (!root) return;

    if (auto* prog = dynamic_cast<Programa*>(root.get())) {
        for (auto& stmt : prog->stmts) {
            runInternal(stmt);
        }
        return;
    }

    if (auto* block = dynamic_cast<Bloco*>(root.get())) {
        pushScope();
        try {
            for (auto& stmt : block->stmts) {
                runInternal(stmt);
            }
        } catch (const ReturnException&) {
            popScope();
            throw;
        }
        popScope();
        return;
    }

    if (auto* decl = dynamic_cast<Declaracao*>(root.get())) { executeDeclaracao(decl); return; }
    if (auto* assign = dynamic_cast<Atribuicao*>(root.get())) { executeAtribuicao(assign); return; }
    if (auto* print = dynamic_cast<PrintfStmt*>(root.get())) { executePrintf(print); return; }
    if (auto* ifStmt = dynamic_cast<ComandoIfElse*>(root.get())) { executeIf(ifStmt); return; }
    if (auto* whileStmt = dynamic_cast<ComandoWhile*>(root.get())) { executeWhile(whileStmt); return; }
    if (auto* forStmt = dynamic_cast<ComandoFor*>(root.get())) { executeFor(forStmt); return; }
    if (auto* doStmt = dynamic_cast<ComandoDo*>(root.get())) { executeDo(doStmt); return; }
    if (auto* ret = dynamic_cast<ComandoReturn*>(root.get())) { executeReturn(ret); return; }

    std::cerr << "Aviso: No nao suportado na execucao.\n";
}


double Evaluator::eval(const Node* node) {
    if (!node) throw std::runtime_error("AST nulo na avaliacao");
    
    if (auto* lit = dynamic_cast<const NumeroLiteral*>(node)) return lit->value;
    
    if (auto* str = dynamic_cast<const CadeiaLiteral*>(node)) {
        throw std::runtime_error("Erro: Cadeia literal nao pode ser avaliada como numero");
    }
    
    if (auto* ref = dynamic_cast<const Identificador*>(node)) {
        return getVariable(ref->name);
    }
    
    if (auto* bin = dynamic_cast<const OperacaoBinaria*>(node)) {
        double left = eval(bin->left.get());
        double right = eval(bin->right.get());
        if (bin->op == "+") return left + right;
        if (bin->op == "-") return left - right;
        if (bin->op == "*") return left * right;
        if (bin->op == "/") {
            if (std::abs(right) < 1e-9) throw std::runtime_error("Divisao por zero");
            return left / right;
        }
        if (bin->op == ">") return left > right ? 1.0 : 0.0;
        if (bin->op == "<") return left < right ? 1.0 : 0.0;
        if (bin->op == ">=") return left >= right ? 1.0 : 0.0;
        if (bin->op == "<=") return left <= right ? 1.0 : 0.0;
        if (bin->op == "==") return left == right ? 1.0 : 0.0;
        if (bin->op == "!=") return left != right ? 1.0 : 0.0;
        if (bin->op == "^") return std::pow(left, right);
    }
    throw std::runtime_error("Tipo de no nao suportado na avaliacao");
}

void Evaluator::pushScope() {
    envStack.emplace_back();
    typeEnvStack.emplace_back();
}

void Evaluator::popScope() {
    if (!envStack.empty()) envStack.pop_back();
    if (!typeEnvStack.empty()) typeEnvStack.pop_back();
}

std::map<std::string, double>* Evaluator::findScope(const std::string& name) {
    for (auto it = envStack.rbegin(); it != envStack.rend(); ++it) {
        if (it->find(name) != it->end()) return &*it;
    }
    return nullptr;
}

double Evaluator::getVariable(const std::string& name) const {
    for (auto it = envStack.rbegin(); it != envStack.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return found->second;
    }
    throw std::runtime_error("Variavel nao declarada: " + name);
}

void Evaluator::executeDeclaracao(const Declaracao* stmt) {
    double val = stmt->init ? eval(stmt->init.get()) : 0.0;
    auto& currentEnv = envStack.back();
    if (currentEnv.find(stmt->name) != currentEnv.end())
        throw std::runtime_error("Variavel ja declarada no mesmo escopo: " + stmt->name);
    currentEnv[stmt->name] = val;
    typeEnvStack.back()[stmt->name] = stmt->type;
}

void Evaluator::executeAtribuicao(const Atribuicao* stmt) {
    double val = eval(stmt->expr.get());
    auto* scope = findScope(stmt->name);
    if (!scope) throw std::runtime_error("Atribuicao a variavel nao declarada: " + stmt->name);
    (*scope)[stmt->name] = val;
}

void Evaluator::executePrintf(const PrintfStmt* stmt) {
    if (auto* str = dynamic_cast<const CadeiaLiteral*>(stmt->arg.get())) {
        std::cout << str->value << "\n";
        return;
    }
    double val = eval(stmt->arg.get());
    std::cout << val << "\n";
}

void Evaluator::executeIf(const ComandoIfElse* stmt) {
    double cond = eval(stmt->cond.get());
    if (cond != 0.0) {
        runInternal(stmt->thenBlock);
    } else if (stmt->elseBlock) {
        runInternal(stmt->elseBlock);
    }
}

void Evaluator::executeWhile(const ComandoWhile* stmt) {
    while (eval(stmt->cond.get()) != 0.0) {
        runInternal(stmt->body);
    }
}

void Evaluator::executeFor(const ComandoFor* stmt) {
    pushScope();
    try {
        runInternal(stmt->init);
        while (eval(stmt->cond.get()) != 0.0) {
            runInternal(stmt->body);
            runInternal(stmt->post);
        }
    } catch (const ReturnException&) {
        popScope();
        throw;
    }
    popScope();
}

void Evaluator::executeDo(const ComandoDo* stmt) {
    do {
        runInternal(stmt->body);
    } while (eval(stmt->cond.get()) != 0.0);
}

void Evaluator::executeReturn(const ComandoReturn* stmt) {
    double value = stmt->expr ? eval(stmt->expr.get()) : 0.0;
    throw ReturnException{stmt->expr != nullptr, value};
}
