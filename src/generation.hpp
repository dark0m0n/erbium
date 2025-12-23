#pragma once

#include <utility>

class Generator {
public:
    explicit Generator(node::Exit root)
            : root(std::move(root)) {
    }

    [[nodiscard]] std::string generate() const {
        std::stringstream output;
        output << "global _start\n_start:\n";
        output << "    mov rax, 60\n";
        output << "    mov rdi, " << *root.expr.token.value << "\n";
        output << "    syscall";
        return output.str();
    }

private:
    node::Exit root;
};
