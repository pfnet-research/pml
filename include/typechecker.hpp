#ifndef PML_TYPECHECKER_HPP
#define PML_TYPECHECKER_HPP

#include "result.hpp"
#include "translate.hpp"

namespace typechecker {

bool typecheck(ast::expr_t const&);

}

#endif
