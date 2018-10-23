#include <ctime>
#include <cstdio>
#include <iostream>
#include <fstream>

#include "expr_ast.hpp"
#include "parser.hpp"
#include "MDP.hpp"
#include "typechecker.hpp"
#include "evaluator.hpp"
#include "simple_type.hpp"

#include "test.hpp"

void output_parse_error(std::string const& input, size_t pos) {
    std::string line;
    for (size_t i=0; i<pos; ++i) {
        line += input[i];
        if (input[i] == '\n')
            line = "";
    }
    std::cout << line << std::endl;
    std::cout << std::string(line.size(), ' ') << "^" << std::endl;
}

int main(int argc, const char* argv[]) {
    std::srand((unsigned int)time(NULL));
#ifdef PML_TEST_BUILD
    test::run(argc, argv);
    return 0;
#endif

    using namespace ast;
    if (argc != 2) {
        std::cout << "[filename] required!" << std::endl;
        return -1;
    }

    std::ifstream input{argv[1]};
    if (input.fail()) {
        std::cout << "file open error : " << argv[1] << std::endl;
        return -1;
    }

    std::istreambuf_iterator<char> it{input}, last;
    std::string input_str{it, last};
    // TODO: be more elegant
    std::cout << "parsing .. " << std::flush;
    parser::parse(input_str).case_of(
        ok >> [](ptr<ast::expr_t> const& expr){
            std::cout << "passed!" << std::endl;
            std::cout << "type checking .. " << std::endl;
            auto const& simty_result = simty::simple_typing(*expr);
            if (simty_result.is_error()) {
                std::cout << "failed at simple typing : " << simty_result.error() << std::endl;
                return;
            }
            if (!typechecker::typecheck(*expr)) {
                std::cout << "failed" << std::endl;
                return;
            }
            std::cout << "passed!" << std::endl;
            std::cout << "=> " << evaluator::eval(expr) << std::endl;
        },
        error >> [&](parser::error_t err) {
            std::cout <<
                "fail error at " << err.pos << " : " <<
                err.message << std::endl;
            output_parse_error(input_str, err.pos);
        }
    );
}

