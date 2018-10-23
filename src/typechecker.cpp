#include <iostream>
#include <fstream>
#include <cstdlib>

#include "environment.hpp"
#include "expr_ast.hpp"
#include "simple_type.hpp"
#include "typechecker.hpp"
#include "translate.hpp"
#include "MDP.hpp"
#include "PCTL.hpp"

// TODO: output to temporary files
bool check_by_PRISM(mdp::mdp_t const& mdp, pctl::pctl_t const& pctl) {
    std::ofstream mdp_output{"./output.mdp"};
    if (mdp_output.fail())
        throw std::runtime_error{"can not open file: ./output.mdp"};
    mdp_output << mdp << std::endl;

    std::ofstream pctl_output{"./output.pctl"};
    if (pctl_output.fail())
        throw std::runtime_error{"can not open file: ./output.pctl"};
    pctl.output(pctl_output);

    if (std::system("prism ./output.mdp ./output.pctl > prism_output") != 0)
        throw std::runtime_error{"can not execute prism"};

    std::ifstream prism{"prism_output"};

    std::string line;
    while (std::getline(prism, line)) {
        if (line.find("Result: true") != std::string::npos)
            return true;
    }
    return false;
}

bool model_checking(ast::expr_t const& expr, ast::refinement_type_t const& type) {
    std::cout << "    converting the program to MDP .. " << std::flush;
    auto mdp_with_info = translate_to_mdp(expr);
    std::cout << "done!" << std::endl;
    std::cout << "    converting the type to PCTL .. " << std::flush;
    auto pctl = translate_to_pctl(type, mdp_with_info);
    std::cout << "done!" << std::endl;
    std::cout << "    checking with PRISM .. " << std::flush;
    auto result = check_by_PRISM(mdp_with_info.mdp, pctl);
    std::cout << "done!" << std::endl;
    return result;
}

using env_t = environment_t<ast::expr_t>;

ptr<ast::expr_t> add_bindings(ptr<ast::expr_t> const& expr, env_t const& env) {
    auto acc = expr;
    auto const& maps = env.elems;
    for (auto it = maps.rbegin(); it != maps.rend(); ++it)
        acc = make<ast::let_expr_t>(it->first, it->second, acc);
    return acc;
}

namespace typechecker {

bool typecheck(ast::expr_t const& expr, env_t const& env) {
    using namespace ast;
    switch (expr.kind()) {
    case expr_kind_t::LetFun:
    case expr_kind_t::App:
        throw std::runtime_error{"unimplemented yet"};
    case expr_kind_t::Typed:
        return model_checking(
                *add_bindings(cast<typed_expr_t>(expr).expr, env),
                cast<typed_expr_t>(expr).type);
    case expr_kind_t::Let: {
        auto init = cast<let_expr_t>(expr).init;
        if (!typecheck(*init, env))
            return false;
        std::string name = cast<let_expr_t>(expr).name;
        auto new_env = env.append(name, init);
        auto body = cast<let_expr_t>(expr).body;
        return typecheck(*body, new_env);
        }
    case expr_kind_t::If:
        return
            typecheck(*cast<if_expr_t>(expr).cond_expr, env) &&
            typecheck(*cast<if_expr_t>(expr).true_expr, env) &&
            typecheck(*cast<if_expr_t>(expr).false_expr, env);
    case expr_kind_t::Neg:
        return typecheck(*cast<neg_expr_t>(expr).inner, env);
    case expr_kind_t::Add: case expr_kind_t::Sub:
    case expr_kind_t::Mul: case expr_kind_t::Div:
    case expr_kind_t::Eq: case expr_kind_t::Neq:
    case expr_kind_t::Leq: case expr_kind_t::Geq:
    case expr_kind_t::And: case expr_kind_t::Or:
        return
            typecheck(*cast<binop_expr_t>(expr).lhs, env) &&
            typecheck(*cast<binop_expr_t>(expr).rhs, env);
    case expr_kind_t::Int: case expr_kind_t::Bool: case expr_kind_t::Fun:
    case expr_kind_t::Rand: case expr_kind_t::Var:
        return true; // primitives
    }
}

bool typecheck(ast::expr_t const& expr) {
    return typecheck(expr, env_t{});
}

}

