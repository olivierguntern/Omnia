
// omnia_interpreter.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <cctype>
#include <cassert>

// ----------------------------
// UTILITAIRES & TOKEN
// ----------------------------
enum class TokenType {
    // Caractères simples et opérateurs
    PLUS, MINUS, STAR, SLASH,
    LPAREN, RPAREN, LBRACE, RBRACE,
    COLON, COMMA,
    EQ,           // pour l'affectation
    EQUAL_EQUAL,  // ==
    BANG_EQUAL,   // !=
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    ARROW,
    // Mots-clés
    LET, VAR, CONST,
    FUN, IF, ELSE, WHILE, RETURN, PRINT,
    // Littéraux
    NUMBER, STRING, IDENT,
    EOF_TOKEN
};

std::string tokenTypeToString(TokenType type) {
    switch (type) {
    case TokenType::PLUS: return "PLUS";
    case TokenType::MINUS: return "MINUS";
    case TokenType::STAR: return "STAR";
    case TokenType::SLASH: return "SLASH";
    case TokenType::LPAREN: return "LPAREN";
    case TokenType::RPAREN: return "RPAREN";
    case TokenType::LBRACE: return "LBRACE";
    case TokenType::RBRACE: return "RBRACE";
    case TokenType::COLON: return "COLON";
    case TokenType::COMMA: return "COMMA";
    case TokenType::EQ: return "EQ";
    case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
    case TokenType::BANG_EQUAL: return "BANG_EQUAL";
    case TokenType::GREATER: return "GREATER";
    case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
    case TokenType::LESS: return "LESS";
    case TokenType::LESS_EQUAL: return "LESS_EQUAL";
    case TokenType::ARROW: return "ARROW";
    case TokenType::LET: return "LET";
    case TokenType::VAR: return "VAR";
    case TokenType::CONST: return "CONST";
    case TokenType::FUN: return "FUN";
    case TokenType::IF: return "IF";
    case TokenType::ELSE: return "ELSE";
    case TokenType::WHILE: return "WHILE";
    case TokenType::RETURN: return "RETURN";
    case TokenType::PRINT: return "PRINT";
    case TokenType::NUMBER: return "NUMBER";
    case TokenType::STRING: return "STRING";
    case TokenType::IDENT: return "IDENT";
    case TokenType::EOF_TOKEN: return "EOF";
    }
    return "UNKNOWN";
}

struct Token {
    TokenType type;
    std::string value;
    int line;
    Token(TokenType t, const std::string &val, int ln) : type(t), value(val), line(ln) {}
};



// ----------------------------
// LEXER
// ----------------------------
class Lexer {
    std::string text;
    size_t pos;
    int line;
public:
    Lexer(const std::string &src) : text(src), pos(0), line(1) {}

    char current() {
        return (pos < text.size()) ? text[pos] : '\0';
    }

    char peek() {
        return (pos + 1 < text.size()) ? text[pos + 1] : '\0';
    }

    void advance() {
        if (current() == '\n')
            line++;
        pos++;
    }

    void skipWhitespace() {
        while (std::isspace(current()))
            advance();
    }

    void skipComment() {
        if (current() == '/' && peek() == '/')
            while (current() != '\n' && current() != '\0')
                advance();
    }

    Token number() {
        std::string result;
        while (std::isdigit(current())) {
            result.push_back(current());
            advance();
        }
        if (current() == '.') {
            result.push_back('.');
            advance();
            while (std::isdigit(current())) {
                result.push_back(current());
                advance();
            }
        }
        return Token(TokenType::NUMBER, result, line);
    }

    Token string() {
        advance(); // saut du guillemet ouvrant
        std::string result;
        while (current() != '"' && current() != '\0') {
            result.push_back(current());
            advance();
        }
        if (current() != '"')
            throw std::runtime_error("Chaine non terminée à la ligne " + std::to_string(line));
        advance(); // saut du guillemet fermant
        return Token(TokenType::STRING, result, line);
    }

    Token identifier() {
        std::string result;
        while (std::isalnum(current()) || current() == '_') {
            result.push_back(current());
            advance();
        }
        // Mots-clés
        static std::unordered_map<std::string, TokenType> keywords = {
            {"let", TokenType::LET}, {"var", TokenType::VAR}, {"const", TokenType::CONST},
            {"fun", TokenType::FUN}, {"if", TokenType::IF}, {"else", TokenType::ELSE},
            {"while", TokenType::WHILE}, {"return", TokenType::RETURN}, {"print", TokenType::PRINT}
        };
        if (keywords.find(result) != keywords.end())
            return Token(keywords[result], result, line);
        return Token(TokenType::IDENT, result, line);
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (true) {
            skipWhitespace();
            skipComment();
            char c = current();
            if (c == '\0')
                break;
            if (std::isdigit(c)) {
                tokens.push_back(number());
                continue;
            }
            if (c == '"') {
                tokens.push_back(string());
                continue;
            }
            if (std::isalpha(c) || c == '_') {
                tokens.push_back(identifier());
                continue;
            }
            // Traitement des opérateurs et délimiteurs
            switch (c) {
            case '+': tokens.push_back(Token(TokenType::PLUS, "+", line)); advance(); break;
            case '-':
                advance();
                if (current() == '>') {
                    tokens.push_back(Token(TokenType::ARROW, "->", line));
                    advance();
                }
                else {
                    tokens.push_back(Token(TokenType::MINUS, "-", line));
                }
                break;
            case '*': tokens.push_back(Token(TokenType::STAR, "*", line)); advance(); break;
            case '/': tokens.push_back(Token(TokenType::SLASH, "/", line)); advance(); break;
            case '(': tokens.push_back(Token(TokenType::LPAREN, "(", line)); advance(); break;
            case ')': tokens.push_back(Token(TokenType::RPAREN, ")", line)); advance(); break;
            case '{': tokens.push_back(Token(TokenType::LBRACE, "{", line)); advance(); break;
            case '}': tokens.push_back(Token(TokenType::RBRACE, "}", line)); advance(); break;
            case ':': tokens.push_back(Token(TokenType::COLON, ":", line)); advance(); break;
            case ',': tokens.push_back(Token(TokenType::COMMA, ",", line)); advance(); break;
            case '=':
                advance();
                if (current() == '=') { tokens.push_back(Token(TokenType::EQUAL_EQUAL, "==", line)); advance(); }
                else { tokens.push_back(Token(TokenType::EQ, "=", line)); }
                break;
            case '!':
                advance();
                if (current() == '=') { tokens.push_back(Token(TokenType::BANG_EQUAL, "!=", line)); advance(); }
                else { throw std::runtime_error("Caractère '!' inattendu à la ligne " + std::to_string(line)); }
                break;
            case '>':
                advance();
                if (current() == '=') { tokens.push_back(Token(TokenType::GREATER_EQUAL, ">=", line)); advance(); }
                else { tokens.push_back(Token(TokenType::GREATER, ">", line)); }
                break;
            case '<':
                advance();
                if (current() == '=') { tokens.push_back(Token(TokenType::LESS_EQUAL, "<=", line)); advance(); }
                else { tokens.push_back(Token(TokenType::LESS, "<", line)); }
                break;
            default:
                throw std::runtime_error("Caractère inconnu '" + std::string(1, c) + "' à la ligne " + std::to_string(line));
            }
        }
        tokens.push_back(Token(TokenType::EOF_TOKEN, "", line));
        return tokens;
    }
};


// ----------------------------
// AST (Noeuds d'expressions et d'instructions)
// ----------------------------
struct Expr;
struct Stmt;

using ExprPtr = std::shared_ptr<Expr>;
using StmtPtr = std::shared_ptr<Stmt>;

struct Expr {
    virtual ~Expr() = default;
};

struct LiteralExpr : public Expr {
    std::variant<double, std::string> value;
    LiteralExpr(const std::variant<double, std::string>& val) : value(val) {}
};

struct VariableExpr : public Expr {
    std::string name;
    VariableExpr(const std::string &n) : name(n) {}
};

struct BinaryExpr : public Expr {
    ExprPtr left;
    Token op;
    ExprPtr right;
    BinaryExpr(ExprPtr l, const Token &o, ExprPtr r) : left(l), op(o), right(r) {}
};

struct UnaryExpr : public Expr {
    Token op;
    ExprPtr right;
    UnaryExpr(const Token &o, ExprPtr r) : op(o), right(r) {}
};

struct AssignExpr : public Expr {
    std::string name;
    ExprPtr value;
    AssignExpr(const std::string &n, ExprPtr v) : name(n), value(v) {}
};

struct CallExpr : public Expr {
    ExprPtr callee;
    std::vector<ExprPtr> arguments;
    CallExpr(ExprPtr c, const std::vector<ExprPtr>& args) : callee(c), arguments(args) {}
};

struct Stmt {
    virtual ~Stmt() = default;
};

struct ExprStmt : public Stmt {
    ExprPtr expression;
    ExprStmt(ExprPtr expr) : expression(expr) {}
};

struct PrintStmt : public Stmt {
    ExprPtr expression;
    PrintStmt(ExprPtr expr) : expression(expr) {}
};

struct VarStmt : public Stmt {
    std::string name;
    ExprPtr initializer;
    Token varType;  // let, var, const
    VarStmt(const std::string &n, const Token &vt, ExprPtr init)
        : name(n), varType(vt), initializer(init) {}
};

struct BlockStmt : public Stmt {
    std::vector<StmtPtr> statements;
    BlockStmt(const std::vector<StmtPtr>& stmts) : statements(stmts) {}
};


// ----------------------------
// Parser (très simplifié)
// ----------------------------
class Parser {
    std::vector<Token> tokens;
    size_t current;
public:
    Parser(const std::vector<Token>& t) : tokens(t), current(0) {}

    std::vector<StmtPtr> parse() {
        std::vector<StmtPtr> statements;
        while (!isAtEnd()) {
            statements.push_back(declaration());
        }
        return statements;
    }
private:
    bool isAtEnd() { return peek().type == TokenType::EOF_TOKEN; }
    const Token &peek() { return tokens[current]; }
    const Token &previous() { return tokens[current - 1]; }
    const Token &advance() { if (!isAtEnd()) current++; return previous(); }

    bool match(const std::vector<TokenType>& types) {
        for (TokenType t : types) {
            if (peek().type == t) { advance(); return true; }
        }
        return false;
    }

    Token consume(TokenType type, const std::string &message) {
        if (peek().type == type)
            return advance();
        throw std::runtime_error("Parser error: " + message + " at line " + std::to_string(peek().line));
    }

    StmtPtr declaration() {
        if (match({TokenType::LET, TokenType::VAR, TokenType::CONST})) {
            Token varType = previous();
            Token name = consume(TokenType::IDENT, "Expected variable name");
            consume(TokenType::EQ, "Expected '=' after variable name");
            ExprPtr init = expression();
            return std::make_shared<VarStmt>(name.value, varType, init);
        }
        return statement();
    }

    StmtPtr statement() {
        if (match({TokenType::PRINT})) {
            ExprPtr value = expression();
            return std::make_shared<PrintStmt>(value);
        }
        return expressionStatement();
    }

    StmtPtr expressionStatement() {
        ExprPtr expr = expression();
        return std::make_shared<ExprStmt>(expr);
    }

    ExprPtr expression() { return assignment(); }

    ExprPtr assignment() {
        ExprPtr expr = equality();
        if (match({TokenType::EQ})) {
            Token equals = previous();
            ExprPtr value = assignment();
            if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr))
                return std::make_shared<AssignExpr>(varExpr->name, value);
            throw std::runtime_error("Invalid assignment target at line " + std::to_string(equals.line));
        }
        return expr;
    }

    ExprPtr equality() {
        ExprPtr expr = comparison();
        while (match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL})) {
            Token op = previous();
            ExprPtr right = comparison();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }
        return expr;
    }

    ExprPtr comparison() { return term(); }
    ExprPtr term() {
        ExprPtr expr = factor();
        while (match({TokenType::PLUS, TokenType::MINUS})) {
            Token op = previous();
            ExprPtr right = factor();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }
        return expr;
    }
    ExprPtr factor() {
        ExprPtr expr = unary();
        while (match({TokenType::STAR, TokenType::SLASH})) {
            Token op = previous();
            ExprPtr right = unary();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }
        return expr;
    }
    ExprPtr unary() {
        if (match({TokenType::MINUS})) {
            Token op = previous();
            ExprPtr right = unary();
            return std::make_shared<UnaryExpr>(op, right);
        }
        return primary();
    }
    ExprPtr primary() {
        if (match({TokenType::NUMBER})) {
            return std::make_shared<LiteralExpr>(std::stod(previous().value));
        }
        if (match({TokenType::STRING})) {
            return std::make_shared<LiteralExpr>(previous().value);
        }
        if (match({TokenType::IDENT})) {
            return std::make_shared<VariableExpr>(previous().value);
        }
        if (match({TokenType::LPAREN})) {
            ExprPtr expr = expression();
            consume(TokenType::RPAREN, "Expected ')' after expression");
            return expr;
        }
        throw std::runtime_error("Unexpected token at line " + std::to_string(peek().line));
    }
};


// ----------------------------
// INTERPRETEUR
// ----------------------------
using Value = std::variant<double, std::string>;

std::string valueToString(const Value &v) {
    if (std::holds_alternative<double>(v))
        return std::to_string(std::get<double>(v));
    else
        return std::get<std::string>(v);
}

class Environment {
    std::unordered_map<std::string, Value> values;
public:
    void define(const std::string &name, Value val) {
        values[name] = val;
    }
    void assign(const std::string &name, Value val) {
        if (values.find(name) != values.end())
            values[name] = val;
        else
            throw std::runtime_error("Undefined variable: " + name);
    }
    Value get(const std::string &name) {
        if (values.find(name) != values.end())
            return values[name];
        throw std::runtime_error("Undefined variable: " + name);
    }
};

class Interpreter {
    Environment global;
public:
    Interpreter() {
        // Définition d'un builtin simple "print"
        global.define("print", std::string("[builtin print]"));
    }

    void interpret(const std::vector<StmtPtr> &statements) {
        try {
            for (auto stmt : statements)
                execute(stmt);
        }
        catch (const std::exception &e) {
            std::cerr << "Runtime error: " << e.what() << std::endl;
        }
    }

    void execute(const StmtPtr &stmt) {
        if (auto exprStmt = std::dynamic_pointer_cast<ExprStmt>(stmt)) {
            evaluate(exprStmt->expression);
        }
        else if (auto printStmt = std::dynamic_pointer_cast<PrintStmt>(stmt)) {
            Value val = evaluate(printStmt->expression);
            std::cout << valueToString(val) << std::endl;
        }
        else if (auto varStmt = std::dynamic_pointer_cast<VarStmt>(stmt)) {
            Value val = evaluate(varStmt->initializer);
            global.define(varStmt->name, val);
        }
        else if (auto blockStmt = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
            for (auto s : blockStmt->statements) {
                execute(s);
            }
        }
    }

    Value evaluate(const ExprPtr &expr) {
        if (auto lit = std::dynamic_pointer_cast<LiteralExpr>(expr))
            return lit->value;
        if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr))
            return global.get(var->name);
        if (auto bin = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
            Value left = evaluate(bin->left);
            Value right = evaluate(bin->right);
            switch (bin->op.type) {
            case TokenType::PLUS:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                    return std::get<double>(left) + std::get<double>(right);
                if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                    return std::get<std::string>(left) + std::get<std::string>(right);
                throw std::runtime_error("Type error in addition");
            case TokenType::MINUS:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                    return std::get<double>(left) - std::get<double>(right);
                throw std::runtime_error("Type error in subtraction");
            case TokenType::STAR:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                    return std::get<double>(left) * std::get<double>(right);
                throw std::runtime_error("Type error in multiplication");
            case TokenType::SLASH:
                if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                    double r = std::get<double>(right);
                    if (r == 0) throw std::runtime_error("Division by zero");
                    return std::get<double>(left) / r;
                }
                throw std::runtime_error("Type error in division");
            default:
                throw std::runtime_error("Unknown binary operator");
            }
        }
        if (auto unary = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
            Value right = evaluate(unary->right);
            if (unary->op.type == TokenType::MINUS) {
                if (std::holds_alternative<double>(right))
                    return -std::get<double>(right);
                throw std::runtime_error("Type error in unary minus");
            }
        }
        if (auto assign = std::dynamic_pointer_cast<AssignExpr>(expr)) {
            Value val = evaluate(assign->value);
            global.assign(assign->name, val);
            return val;
        }
        throw std::runtime_error("Unknown expression type");
    }
};


// ----------------------------
// MAIN
// ----------------------------
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " fichier.omn" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Erreur d'ouverture du fichier: " << argv[1] << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();
        Parser parser(tokens);
        std::vector<StmtPtr> statements = parser.parse();
        Interpreter interpreter;
        interpreter.interpret(statements);
    }
    catch (const std::exception &e) {
        std::cerr << "Erreur: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}


