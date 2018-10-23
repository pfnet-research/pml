#include "type_ast.hpp"
#include "expr_ast.hpp"
#include "logic.hpp"
#include "environment.hpp"
#include "simple_type.hpp"

namespace simty {

ptr<type_t> type_t::from(ast::refinement_type_t const& ty) {
    switch (ty.domain) {
    case logic::domain_kind_t::Int:
        return make<int_type_t>();
    case logic::domain_kind_t::Bool:
        return make<bool_type_t>();
    }
}

ptr<type_t> type_t::from(ast::dependent_type_t const& ty) {
    std::vector<ptr<type_t>> args;
    for (auto const& arg : ty.args)
        args.push_back(type_t::from(arg));
    auto ret_ty = type_t::from(ty.ret_type);
    return make<fun_type_t>(args, ret_ty);
}

using type_env_t = environment_t<type_t>;
result_t simple_typing(ast::expr_t const& expr, type_env_t const& env) {
    using namespace ast;
    switch (expr.kind()) {
    case expr_kind_t::LetFun: {
        auto const& letfun = cast<letfun_expr_t>(expr);
        auto const& simty = type_t::from(letfun.type);
        auto const& fun_simty = cast<fun_type_t>(*simty);
        auto init_env = env.append(letfun.name, simty);
        for (size_t i=0; i<fun_simty.args.size(); ++i)
            init_env = init_env.append(letfun.type.args[i].name, fun_simty.args[i]);
        auto init_typing_result = simple_typing(*letfun.init, init_env);
        if (init_typing_result.is_error())
            return init_typing_result;
        auto body_env = env.append(letfun.name, simty);
        return simple_typing(*letfun.body, body_env);
        }
    case expr_kind_t::App: {
        auto const& f_typing_result = simple_typing(*cast<app_expr_t>(expr).f, env);
        if (f_typing_result.is_error())
            return f_typing_result;
        auto const& fun_ty = cast<fun_type_t>(*f_typing_result.ok());
        auto const& args = cast<app_expr_t>(expr).args;
        if (fun_ty.args.size() != args.size()) {
            return result_t::error(format(
                        "not match numbers of parameters and arguments (param {} vs arg {}) in \"{}\"",
                        fun_ty.args.size(), args.size(), to_debug_string(expr)));
        }
        for (size_t i=0; i<args.size(); ++i) {
            auto arg_ty_result = simple_typing(*args[i], env);
            if (arg_ty_result.is_error())
                return arg_ty_result;
            if (*arg_ty_result.ok() != *fun_ty.args[i]) {
                return result_t::error(format(
                        "not match {}th parameter and arguments ({} vs {}) in \"{}\"",
                        to_debug_string(*arg_ty_result.ok()), to_debug_string(*fun_ty.args[i]),
                        to_debug_string(expr)));
            }
        }
        return fun_ty.ret;
        }
    case expr_kind_t::Fun:
        // do not appear in syntax
        throw std::logic_error{"fun expr can not appear"};
    case expr_kind_t::Let: {
            auto init_result = simple_typing(*cast<let_expr_t>(expr).init, env);
            if (init_result.is_error())
                return init_result;
            auto new_env = env.append(cast<let_expr_t>(expr).name, init_result.ok());
            return simple_typing(*cast<let_expr_t>(expr).body, new_env);
        }
    case expr_kind_t::If: {
        auto const& cond_result = simple_typing(*cast<if_expr_t>(expr).cond_expr, env);
        auto const& tr_result = simple_typing(*cast<if_expr_t>(expr).true_expr, env);
        auto const& fl_result = simple_typing(*cast<if_expr_t>(expr).false_expr, env);
        auto sub_result = cond_result && tr_result && fl_result;
        if (sub_result.is_error())
            return sub_result;
        if (*cond_result.ok() != simty::bool_type_t{})
            return result_t::error(format("condition of if expr must be boolean in \"{}\"", to_debug_string(expr)));
        if (*tr_result.ok() != *fl_result.ok())
            return result_t::error(format("not match types of branches of if expr in \"{}\"", to_debug_string(expr)));
        return tr_result;
        }
    case expr_kind_t::Rand:
        return result_t::ok(make<simty::int_type_t>());
    case expr_kind_t::Typed: {
        auto const& inner = *cast<typed_expr_t>(expr).expr;
        auto const& expr_ty_result = simple_typing(inner, env);
        if (expr_ty_result.is_error())
            return expr_ty_result;
        auto const& ty = type_t::from(cast<typed_expr_t>(expr).type);
        if (*expr_ty_result.ok() == *ty)
            return expr_ty_result;
        else
            return result_t::error(format("{} has simple type {}, but expected {}",
                        to_debug_string(inner),
                        to_debug_string(*expr_ty_result.ok()), to_debug_string(*ty)));
        }
    case expr_kind_t::Add:
    case expr_kind_t::Sub:
    case expr_kind_t::Mul:
    case expr_kind_t::Div: {
        auto const& lhs_result = simple_typing(*cast<binop_expr_t>(expr).lhs, env);
        auto const& rhs_result = simple_typing(*cast<binop_expr_t>(expr).rhs, env);
        auto const& sub_result = lhs_result && rhs_result;
        if (sub_result.is_error())
            return sub_result;
        if (*lhs_result.ok() != simty::int_type_t{} || *rhs_result.ok() != simty::int_type_t{}) {
            return result_t::error(format(
                    "can not apply arithmetic binary operation to non-integer expression in {}",
                    to_debug_string(expr)));
        }
        return result_t::ok(make<simty::int_type_t>());
        }
    case expr_kind_t::Neg: {
        auto const& inner_result = simple_typing(*cast<neg_expr_t>(expr).inner, env);
        if (inner_result.is_error())
            return inner_result;
        if (*inner_result.ok() != simty::bool_type_t{}) {
            return result_t::error(format(
                    "can not apply negation operator to non-boolean expression in {}",
                    to_debug_string(expr)));
        }
        return result_t::ok(make<simty::bool_type_t>());
        }
    case expr_kind_t::Eq:
    case expr_kind_t::Neq:
    case expr_kind_t::Leq:
    case expr_kind_t::Geq: {
        auto const& lhs_result = simple_typing(*cast<binop_expr_t>(expr).lhs, env);
        auto const& rhs_result = simple_typing(*cast<binop_expr_t>(expr).rhs, env);
        auto sub_result = lhs_result && rhs_result;
        if (sub_result.is_error())
            return sub_result;
        if (*lhs_result.ok() != simty::int_type_t{} || *rhs_result.ok() != simty::int_type_t{}) {
            return result_t::error(format(
                    "can not apply comparison operation to non-boolean expression in {}",
                    to_debug_string(expr)));
        }
        return result_t::ok(make<simty::bool_type_t>());
        }
    case expr_kind_t::And:
    case expr_kind_t::Or: {
        auto const& lhs_result = simple_typing(*cast<binop_expr_t>(expr).lhs, env);
        auto const& rhs_result = simple_typing(*cast<binop_expr_t>(expr).rhs, env);
        auto const& sub_result = lhs_result && rhs_result;
        if (sub_result.is_error())
            return sub_result;
        if (*lhs_result.ok() != simty::bool_type_t{} || *rhs_result.ok() != simty::bool_type_t{}) {
            return result_t::error(format(
                        "can not apply logical binary operation to non-boolean expression in {}",
                        to_debug_string(expr)));
        }
        return result_t::ok(make<simty::bool_type_t>());
        }
    case expr_kind_t::Int:
        return result_t::ok(make<simty::int_type_t>());
    case expr_kind_t::Bool:
        return result_t::ok(make<simty::bool_type_t>());
    case expr_kind_t::Var:
        return result_t::ok(env.lookup(cast<var_expr_t>(expr).name));
    }
}

result_t simple_typing(ast::expr_t const& expr) {
    return simple_typing(expr, type_env_t{});
}

std::string to_debug_string(type_t const& ty) {
    switch (ty.kind()) {
    case simty::kind_t::Int:
        return "Int";
    case simty::kind_t::Bool:
        return "Bool";
    case simty::kind_t::Fun: {
        auto const& fun_ty = cast<fun_type_t>(ty);
        std::string result = "(" + to_debug_string(*fun_ty.args[0]);
        for (size_t i=1; i<fun_ty.args.size(); ++i)
            result += ", " + to_debug_string(*fun_ty.args[i]);
        result += ") -> ";
        result += to_debug_string(*fun_ty.ret);
        return result;
        }
    }
}

}


