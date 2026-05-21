#pragma once
#include <string>
#include <memory>
#include <vector>

// Base polimórfica para todos os nós da AST.
struct Node { virtual ~Node() = default; };

// Nó raiz do programa, contendo todas as instruções top-level.
struct Programa : Node {
    std::vector<std::unique_ptr<Node>> stmts;
    Programa(std::vector<std::unique_ptr<Node>> s) : stmts(std::move(s)) {}
};

// Literais
struct NumeroLiteral : Node { 
    double value; 
    NumeroLiteral(double v) : value(v) {} 
};

struct CadeiaLiteral : Node { 
    std::string value; 
    CadeiaLiteral(std::string v) : value(std::move(v)) {} 
};

// Nó de referência a variável.
struct Identificador : Node { 
    std::string name; 
    Identificador(std::string n) : name(std::move(n)) {} 
};

// Operadores binários: left op right
struct OperacaoBinaria : Node {
    std::string op;
    std::unique_ptr<Node> left, right;
    OperacaoBinaria(std::string o, std::unique_ptr<Node> l, std::unique_ptr<Node> r) 
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {}
};

// Estruturas de comando
struct PrintfStmt : Node { 
    std::unique_ptr<Node> arg; 
    PrintfStmt(std::unique_ptr<Node> a) : arg(std::move(a)) {} 
};

struct Declaracao : Node {
    std::string type, name;
    std::unique_ptr<Node> init;
    Declaracao(std::string t, std::string n, std::unique_ptr<Node> i) 
        : type(std::move(t)), name(std::move(n)), init(std::move(i)) {}
};

struct Atribuicao : Node {
    std::string name;
    std::unique_ptr<Node> expr;
    Atribuicao(std::string n, std::unique_ptr<Node> e) 
        : name(std::move(n)), expr(std::move(e)) {}
};

struct Bloco : Node {
    std::vector<std::unique_ptr<Node>> stmts;
    Bloco(std::vector<std::unique_ptr<Node>> s) : stmts(std::move(s)) {}
};

struct ComandoIfElse : Node {
    std::unique_ptr<Node> cond;
    std::unique_ptr<Node> thenBlock;
    std::unique_ptr<Node> elseBlock;
    ComandoIfElse(std::unique_ptr<Node> c, std::unique_ptr<Node> t, std::unique_ptr<Node> e)
        : cond(std::move(c)), thenBlock(std::move(t)), elseBlock(std::move(e)) {}
};

struct ComandoWhile : Node {
    std::unique_ptr<Node> cond;
    std::unique_ptr<Node> body;
    ComandoWhile(std::unique_ptr<Node> c, std::unique_ptr<Node> b)
        : cond(std::move(c)), body(std::move(b)) {}
};

struct ComandoFor : Node {
    std::unique_ptr<Node> init;
    std::unique_ptr<Node> cond;
    std::unique_ptr<Node> post;
    std::unique_ptr<Node> body;
    ComandoFor(std::unique_ptr<Node> i, std::unique_ptr<Node> c, std::unique_ptr<Node> p, std::unique_ptr<Node> b)
        : init(std::move(i)), cond(std::move(c)), post(std::move(p)), body(std::move(b)) {}
};

struct ComandoDo : Node {
    std::unique_ptr<Node> body;
    std::unique_ptr<Node> cond;
    ComandoDo(std::unique_ptr<Node> b, std::unique_ptr<Node> c)
        : body(std::move(b)), cond(std::move(c)) {}
};

struct ComandoReturn : Node {
    std::unique_ptr<Node> expr;
    ComandoReturn(std::unique_ptr<Node> e) : expr(std::move(e)) {}
};