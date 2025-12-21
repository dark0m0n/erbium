#include <format>
#include <iostream>
#include <sstream>
#include <fstream>
#include <optional>
#include <vector>

enum class TokenType {
    _return,
    int_literal,
    semicolon,
};

struct Token {
    TokenType type;
    std::optional<std::string> value;
};

std::vector<Token> tokenize(const std::string& str) {
    std::vector<Token> tokens;
    std::string buffer;
    for (int i = 0; i < str.length(); i++) {
        if (char c = str[i]; isalpha(c)) {
            buffer += c;
            i++;
            while (isalnum(c = str[i])) {
                buffer += c;
                i++;
            }
            i--;
            if (buffer == "return") {
                tokens.push_back(Token{TokenType::_return});
                buffer.clear();
            } else {
                std::cerr << "You messed up!" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (isdigit(c)) {
            buffer += c;
            i++;
            while (isdigit(c = str[i])) {
                buffer += c;
                i++;
            }
            i--;
            tokens.push_back(Token{TokenType::int_literal, buffer});
            buffer.clear();
        } else if (c == ';') {
            tokens.push_back(Token{TokenType::semicolon});
        } else if (isspace(c)) {
        } else {
            std::cerr << "You messed up!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}

std::string tokens_to_asm(std::vector<Token> tokens) {
    std::stringstream output;
    output << "global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens[i];
        if (token.type == TokenType::_return) {
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::int_literal) {
                if (i + 2 < tokens.size() && tokens[i + 2].type == TokenType::semicolon) {
                    output << "    mov rax, 60\n";
                    output << "    mov rdi, " << tokens[i + 1].value.value() << "\n";
                    output << "    syscall";
                }
            }
        }
    }
    return output.str();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect usage" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1]);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    std::vector<Token> tokens = tokenize(contents);

    {
        std::fstream file("out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }

    system("nasm -felf64 out.asm");
    system("ld out.o -o out");

    return EXIT_SUCCESS;
}