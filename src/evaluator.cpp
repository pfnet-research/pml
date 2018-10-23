#include <cassert>
#include <iostream>
#include <vector>

#include "result.hpp"
#include "environment.hpp"
#include "evaluator.hpp"

ptr<ast::expr_t> calc_int_binop(
        ast::expr_t const& lhs,
        ast::expr_t const& rhs,
        ast::expr_kind_t kind) {
    using namespace ast;
    assert(is_value(lhs));
    assert(is_value(rhs));
    switch (kind) {
    case expr_kind_t::Eq:
        return make<bool_expr_t>(lhs == rhs);
    case expr_kind_t::Neq:
        return make<bool_expr_t>(lhs != rhs);
    case expr_kind_t::Leq:
        return make<bool_expr_t>(cast<int_expr_t>(lhs).n <= cast<int_expr_t>(rhs).n);
    case expr_kind_t::Geq:
        return make<bool_expr_t>(cast<int_expr_t>(lhs).n >= cast<int_expr_t>(rhs).n);
    case expr_kind_t::Add:
        return make<int_expr_t>(cast<int_expr_t>(lhs).n + cast<int_expr_t>(rhs).n);
    case expr_kind_t::Sub:
        return make<int_expr_t>(cast<int_expr_t>(lhs).n - cast<int_expr_t>(rhs).n);
    case expr_kind_t::Mul:
        return make<int_expr_t>(cast<int_expr_t>(lhs).n * cast<int_expr_t>(rhs).n);
    case expr_kind_t::Div:
        return make<int_expr_t>(cast<int_expr_t>(lhs).n / cast<int_expr_t>(rhs).n);
    case expr_kind_t::And:
        return make<bool_expr_t>(cast<bool_expr_t>(lhs).b && cast<bool_expr_t>(rhs).b);
    case expr_kind_t::Or:
        return make<bool_expr_t>(cast<bool_expr_t>(lhs).b || cast<bool_expr_t>(rhs).b);
    default:
        throw std::logic_error{"invalid binop : " + to_string(kind)};
    }
}


namespace evaluator {

using environment_t = ::environment_t<ast::expr_t>;

ptr<ast::expr_t> eval(ptr<ast::expr_t> e, environment_t const& env) {
    using namespace ast;
    switch (e->kind()) {
    case expr_kind_t::Let: {
        auto& let_expr = cast<let_expr_t>(*e);
        auto init = eval(let_expr.init, env);
        auto new_env = env.append(let_expr.name, init);
        return eval(let_expr.body, new_env);
        }
    case expr_kind_t::LetFun: {
        auto& letfun_expr = cast<letfun_expr_t>(*e);
        auto fun = make<ast::fun_expr_t>(letfun_expr.type, letfun_expr.init);
        auto new_env = env.append(letfun_expr.name, fun);
        return eval(letfun_expr.body, new_env);
        }
    case expr_kind_t::If: {
        auto& if_expr = cast<if_expr_t>(*e);
        auto cond_val = eval(if_expr.cond_expr, env);
        if (cond_val->kind() != ast::expr_kind_t::Bool) {
            std::cerr << "eval error: if-condition must be boolean " + ast::to_debug_string(*cond_val) << std::endl;
            assert(false);
            return nullptr;
        } else if (cast<bool_expr_t>(*cond_val).b == true) {
            return eval(if_expr.true_expr, env);
        } {
            return eval(if_expr.false_expr, env);
        }
        }
    case expr_kind_t::Eq:  case expr_kind_t::Neq:
    case expr_kind_t::Leq:  case expr_kind_t::Geq:
    case expr_kind_t::Add: case expr_kind_t::Sub:
    case expr_kind_t::Mul: case expr_kind_t::Div:
    case expr_kind_t::And: case expr_kind_t::Or: {
            auto& binop_expr = cast<binop_expr_t>(*e);
            return calc_int_binop(
                    *eval(binop_expr.lhs, env),
                    *eval(binop_expr.rhs, env),
                    e->kind());
        }
    case expr_kind_t::Neg: {
            auto val = eval(cast<neg_expr_t>(*e).inner, env);
            return make<bool_expr_t>(!cast<bool_expr_t>(*val).b);
        }
    case expr_kind_t::Int: case expr_kind_t::Bool: case expr_kind_t::Fun:
        return e;
    case expr_kind_t::Var:
        return env.lookup(cast<var_expr_t>(*e).name);
    case expr_kind_t::App: {
            auto const& app_expr = cast<app_expr_t>(*e);
            auto f_ = eval(app_expr.f, env);
            if (f_->kind() != ast::expr_kind_t::Fun)
                throw std::logic_error{"arienai"};
            auto f = cast<fun_expr_t>(*f_);
            if (f.type.args.size() != app_expr.args.size())
                throw std::logic_error{"arienai"};
            environment_t new_env = env;
            for (size_t i=0; i<app_expr.args.size(); ++i) {
                new_env = new_env.append(
                        f.type.args[i].name,
                        eval(app_expr.args[i], env));
            }
            return eval(f.body, new_env);
        }
    case expr_kind_t::Rand: {
            auto& rand_expr = cast<rand_expr_t>(*e);
            int start = rand_expr.start;
            int end = rand_expr.end;
            return make<int_expr_t>(std::rand() % (end - start + 1) + start);
        }
    case expr_kind_t::Typed:
        return eval(cast<typed_expr_t>(*e).expr, env);
    }
}

ptr<ast::expr_t> eval(ptr<ast::expr_t> e) {
    return eval(e, environment_t{});
}

}


