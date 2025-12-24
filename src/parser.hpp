#pragma once
#include <variant>

#include "tokenization.hpp"

namespace node {
    struct ExprIntLiteral {
        Token int_literal;
    };

    struct ExprIdentifier {
        Token identifier;
    };

    struct Expr {
        std::variant<ExprIntLiteral, ExprIdentifier> var;
    };

    struct StmtExit {
        Expr expr;
    };

    struct StmtLet {
        Token identifier;
        Expr expr;
    };

    struct Stmt {
        std::variant<StmtExit, StmtLet> var;
    };

    struct Program {
        std::vector<Stmt> stmts;
    };
}

class Parser {
public:
    explicit Parser(const std::vector<Token> &tokens)
        : tokens(tokens) {
    }

    std::optional<node::Expr> parse_expr() {
        if (peek()->type == TokenType::int_literal) {
            return node::Expr(node::ExprIntLiteral(consume()));
        }
        if (peek()->type == TokenType::identifier) {
            return node::Expr(node::ExprIdentifier(consume()));
        }
        return std::nullopt;
    }

    std::optional<node::Stmt> parse_stmt() {
        if (peek()->type == TokenType::exit && peek(1)->type == TokenType::open_parenthesis) {
            consume();
            consume();
            node::StmtExit stmt_exit;
            if (const auto node_expr = parse_expr()) {
                stmt_exit = node::StmtExit(*node_expr);
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek()->type == TokenType::close_parenthesis) {
                consume();
            } else {
                std::cerr << "Expected ')'" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek()->type == TokenType::semicolon) {
                consume();
            } else {
                std::cerr << "Expected ';'" << std::endl;
                exit(EXIT_FAILURE);
            }
            return node::Stmt(stmt_exit);
        }
        if (peek()->type == TokenType::let
            && peek(1)->type == TokenType::identifier
            && peek(2)->type == TokenType::equals) {
            consume();
            auto stmt_let = node::StmtLet{.identifier = consume()};
            consume();
            if (auto expr = parse_expr()) {
                stmt_let.expr = *expr;
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek()->type == TokenType::semicolon) {
                consume();
            } else {
                std::cerr << "Expected ';'" << std::endl;
                exit(EXIT_FAILURE);
            }
            return node::Stmt(stmt_let);
        }
        return std::nullopt;
    }

    std::optional<node::Program> parse_program() {
        node::Program program;
        while (peek().has_value()) {
            if (auto stmt = parse_stmt()) {
                program.stmts.push_back(*stmt);
            } else {
                std::cerr << "Invalid statement" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return program;
    }

private:
    const std::vector<Token> tokens;
    size_t index = 0;

    [[nodiscard]] std::optional<Token> peek(const int offset = 0) const {
        if (index + offset >= tokens.size()) {
            return std::nullopt;
        }
        return tokens[index + offset];
    }

    Token consume() {
        return tokens[index++];
    }
};
