#ifndef PML_EVALUATOR_HPP
#define PML_EVALUATOR_HPP

#include "expr_ast.hpp"

namespace evaluator {

struct eval_error_t {
    enum class type {
        Debug
    };
    type ty;
    std::string message;
};

ptr<ast::expr_t> eval(ptr<ast::expr_t>);

}

#endif
