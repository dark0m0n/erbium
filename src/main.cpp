#include <format>
#include <iostream>
#include <sstream>
#include <fstream>
#include <optional>
#include <vector>

#include "tokenization.hpp"
#include "parser.hpp"
#include "generation.hpp"

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

    Tokenizer tokenizer(contents);
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(tokens);
    std::optional<node::Exit> tree = parser.parse();

    if (!tree.has_value()) {
        std::cerr << "No exit statement found" << std::endl;
        return EXIT_FAILURE;
    }

    Generator generator(*tree);
    {
        std::fstream file("out.asm", std::ios::out);
        file << generator.generate();
    }

    system("nasm -felf64 out.asm");
    system("ld out.o -o out");

    return EXIT_SUCCESS;
}
