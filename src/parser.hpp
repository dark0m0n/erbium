#pragma once

namespace node {
    struct Expr {
        Token token;
    };

    struct Exit {
        Expr expr;
    };
}

class Parser {
public:
    explicit Parser(const std::vector<Token> &tokens)
            : tokens(tokens) {
    }

    std::optional<node::Expr> parse_expr() {
        if (peek().has_value() && peek()->type == TokenType::int_literal) {
            return node::Expr(consume());
        }
        return std::nullopt;
    }

    std::optional<node::Exit> parse() {
        std::optional<node::Exit> exit_node = std::nullopt;
        while (peek().has_value()) {
            if (peek()->type == TokenType::exit) {
                consume();
                if (auto node_expr = parse_expr()) {
                    exit_node = node::Exit(*node_expr);
                } else {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if (peek().has_value() && peek()->type == TokenType::semicolon) {
                    consume();
                } else {
                    std::cerr << "Expected ';'" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
        index = 0;
        return exit_node;
    }

private:
    const std::vector<Token> tokens;
    size_t index = 0;

    [[nodiscard]] std::optional<Token> peek(const int ahead = 1) const {
        if (index + ahead > tokens.size()) {
            return std::nullopt;
        }
        return tokens[index];
    }

    Token consume() {
        return tokens[index++];
    }
};
