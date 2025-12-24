#pragma once

#include <unordered_map>
#include <utility>

class Generator {
public:
    explicit Generator(node::Program program)
        : program(std::move(program)) {
    }

    void gen_expr(const node::Expr &expr) {
        struct ExprVisitor {
            Generator &gen;

            void operator()(const node::ExprIntLiteral &expr_int_literal) const {
                gen.output << "\tmov rax, " << *expr_int_literal.int_literal.value << '\n';
                gen.push("rax");
            }

            void operator()(const node::ExprIdentifier &expr_identifier) const {
                const auto &name = *expr_identifier.identifier.value;

                if (!gen.vars.contains(name)) {
                    std::cerr << "Undeclared identifier: " << name << std::endl;
                    std::exit(EXIT_FAILURE);
                }

                const auto stack_loc = gen.vars[name].stack_loc;
                const auto offset = (gen.stack_size - stack_loc - 1) * 8;

                gen.output << "\tmov rax, [rsp + " << offset << "]\n";
                gen.push("rax");
            }
        };

        ExprVisitor visitor{*this};
        std::visit(visitor, expr.var);
    }

    void gen_stmt(const node::Stmt &stmt) {
        struct StmtVisitor {
            Generator &gen;

            void operator()(const node::StmtExit &stmt_exit) const {
                gen.gen_expr(stmt_exit.expr);

                gen.output << "\tmov rax, 60\n";
                gen.pop("rdi");

                gen.align_stack_for_call();
                gen.output << "\tsyscall\n";
                gen.restore_stack_after_call();
            }

            void operator()(const node::StmtLet &stmt_let) const {
                if (gen.vars.contains(*stmt_let.identifier.value)) {
                    std::cerr << "Identifier already used: " << *stmt_let.identifier.value << std::endl;
                    exit(EXIT_FAILURE);
                }
                // stack_loc = stack_size before pushing variable value
                gen.vars.insert({*stmt_let.identifier.value, Var{gen.stack_size}});
                gen.gen_expr(stmt_let.expr);
            }
        };

        StmtVisitor visitor{*this};
        std::visit(visitor, stmt.var);
    }

    std::string gen_prog() {
        output << "global _start\n_start:\n";

        for (const auto &stmt: program.stmts) {
            gen_stmt(stmt);
        }

        output << "\tmov rax, 60\n";
        output << "\tmov rdi, 0\n";
        output << "\tsyscall\n";

        return output.str();
    }

private:
    struct Var {
        size_t stack_loc;
    };

    node::Program program;
    std::stringstream output;
    size_t stack_size = 0;
    std::unordered_map<std::string, Var> vars;

    void push(const std::string &reg) {
        output << "\tpush " << reg << "\n";
        stack_size++;
    }

    void pop(const std::string &reg) {
        output << "\tpop " << reg << "\n";
        stack_size--;
    }

    void align_stack_for_call() {
        if (stack_size % 2 != 0) {
            output << "\tsub rsp, 8\n";
            stack_size++;
        }
    }

    void restore_stack_after_call() {
        if (stack_size % 2 != 0) {
            output << "\tadd rsp, 8\n";
            stack_size--;
        }
    }
};
