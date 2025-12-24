#pragma once

#include <utility>

enum class TokenType {
    _return,
    int_literal,
    semicolon,
    exit,
    open_parenthesis,
    close_parenthesis,
    identifier,
    let,
    equals,
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer {
public:
    explicit Tokenizer(std::string src)
        : src(std::move(src)) {
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        std::string buffer;
        while (peek().has_value()) {
            if (std::isalpha(*peek())) {
                buffer.push_back(consume());
                while (std::isalnum(*peek())) {
                    buffer.push_back(consume());
                }
                if (buffer == "exit") {
                    tokens.push_back(Token{TokenType::exit});
                    buffer.clear();
                } else if (buffer == "let") {
                    tokens.push_back(Token{TokenType::let});
                    buffer.clear();
                } else {
                    tokens.push_back(Token{TokenType::identifier, buffer});
                    buffer.clear();
                }
            } else if (std::isdigit(*peek())) {
                buffer.push_back(consume());
                while (std::isdigit(*peek())) {
                    buffer.push_back(consume());
                }
                tokens.push_back(Token{TokenType::int_literal, buffer});
                buffer.clear();
            } else if (*peek() == '(') {
                consume();
                tokens.push_back(Token{TokenType::open_parenthesis});
            } else if (*peek() == ')') {
                consume();
                tokens.push_back(Token{TokenType::close_parenthesis});
            } else if (*peek() == '=') {
                consume();
                tokens.push_back(Token{TokenType::equals});
            } else if (*peek() == ';') {
                consume();
                tokens.push_back(Token{TokenType::semicolon});
            } else if (std::isspace(*peek())) {
                consume();
            } else {
                std::cerr << "You messed up!" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        index = 0;
        return tokens;
    }

private:
    const std::string src;
    size_t index = 0;

    [[nodiscard]] std::optional<char> peek(const int offset = 0) const {
        if (index + offset >= src.length()) {
            return std::nullopt;
        }
        return src[index + offset];
    }

    char consume() {
        return src[index++];
    }
};
