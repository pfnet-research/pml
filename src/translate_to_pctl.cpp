#include "type_ast.hpp"
#include "translate.hpp"
#include "PCTL.hpp"
#include "logic.hpp"

pctl::pctl_t translate_to_pctl(ast::refinement_type_t const& ty, mdp_with_info_t const& mdp_with_info) {
    using namespace logic;
    auto const& arg = mdp_with_info.value.name;
    auto formula = ty.domain == domain_kind_t::Int ?
        subst(ty.constraint, ty.name, make<var_term_t>(arg)) :
        subst(ty.constraint, ty.name, make<var_formula_t>(arg));
    return pctl::pctl_t {
        mdp_with_info.accept,
        formula
    };
}

