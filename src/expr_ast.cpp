#include "expr_ast.hpp"

namespace ast {

std::string to_string(expr_kind_t kind) {
    switch (kind) {
    case expr_kind_t::Let:
        return "Let";
    case expr_kind_t::LetFun:
        return "LetFun";
    case expr_kind_t::If:
        return "If";
    case expr_kind_t::App:
        return "App";
    case expr_kind_t::Rand:
        return "Rand";
    case expr_kind_t::Eq:
        return "Eq";
    case expr_kind_t::Neq:
        return "Neq";
    case expr_kind_t::Leq:
        return "Leq";
    case expr_kind_t::Geq:
        return "Geq";
    case expr_kind_t::Add:
        return "Add";
    case expr_kind_t::Sub:
        return "Sub";
    case expr_kind_t::Mul:
        return "Mul";
    case expr_kind_t::Div:
        return "Div";
    case expr_kind_t::And:
        return "And";
    case expr_kind_t::Or:
        return "Or";
    case expr_kind_t::Neg:
        return "Neg";
    case expr_kind_t::Typed:
        return "Typed";
    case expr_kind_t::Fun:
        return "Fun";
    case expr_kind_t::Int:
        return "Int";
    case expr_kind_t::Bool:
        return "Bool";
    case expr_kind_t::Var:
        return "Var";
    }
}

bool operator==(expr_t const& lhs, expr_t const& rhs) {
    if (lhs.kind() != rhs.kind())
        return false;

    if (lhs.kind() == expr_kind_t::Let) {
        auto const& l = cast<let_expr_t>(lhs);
        auto const& r = cast<let_expr_t>(rhs);
        return l.name == r.name && *l.init == *r.init && *l.body == *r.body;
    }
    if (lhs.kind() == expr_kind_t::LetFun) {
        auto const& l = cast<letfun_expr_t>(lhs);
        auto const& r = cast<letfun_expr_t>(rhs);
        return
            l.name == r.name && l.type == r.type &&
            *l.init == *r.init && *l.body == *r.body;
    }
    if (lhs.kind() == expr_kind_t::If) {
        auto const& l = cast<if_expr_t>(lhs);
        auto const& r = cast<if_expr_t>(rhs);
        return
            *l.cond_expr == *r.cond_expr &&
            *l.true_expr == *r.true_expr && *l.false_expr == *r.false_expr;
    }
    if (lhs.kind() == expr_kind_t::App) {
        auto const& l = cast<app_expr_t>(lhs);
        auto const& r = cast<app_expr_t>(rhs);
        if (*l.f != *r.f || l.args.size() != r.args.size())
            return false;
        for (size_t i=0; i<l.args.size(); ++i) {
            if (*l.args[i] != *r.args[i])
                return false;
        }
        return true;
    }
    if (lhs.kind() == expr_kind_t::Rand) {
        auto const& l = cast<rand_expr_t>(lhs);
        auto const& r = cast<rand_expr_t>(rhs);
        return l.start == r.start && l.end == r.end;
    }
    if (lhs.kind() == expr_kind_t::Eq ||
            lhs.kind() == expr_kind_t::Neq ||
            lhs.kind() == expr_kind_t::Leq ||
            lhs.kind() == expr_kind_t::Geq ||
            lhs.kind() == expr_kind_t::Add ||
            lhs.kind() == expr_kind_t::Sub ||
            lhs.kind() == expr_kind_t::Mul ||
            lhs.kind() == expr_kind_t::Div ||
            lhs.kind() == expr_kind_t::And ||
            lhs.kind() == expr_kind_t::Or) {
        auto const& l = cast<binop_expr_t>(lhs);
        auto const& r = cast<binop_expr_t>(rhs);
        return *l.lhs == *r.lhs && *l.rhs == *r.rhs;
    }
    if (lhs.kind() == expr_kind_t::Neg)
        return *cast<neg_expr_t>(lhs).inner == *cast<neg_expr_t>(rhs).inner;
    if (lhs.kind() == expr_kind_t::Typed) {
        auto const& l = cast<typed_expr_t>(lhs);
        auto const& r = cast<typed_expr_t>(rhs);
        return *l.expr == *r.expr && l.type == r.type;
    }
    if (lhs.kind() == expr_kind_t::Int) {
        return cast<int_expr_t>(lhs) == cast<int_expr_t>(rhs);
    }
    if (lhs.kind() == expr_kind_t::Bool)
        return cast<bool_expr_t>(lhs) == cast<bool_expr_t>(rhs);
    throw std::logic_error{"unreachable!!"};
}

std::string to_debug_string(expr_t const& e) {
    switch (e.kind()) {
    case expr_kind_t::Let: {
            auto const& let = cast<let_expr_t>(e);
            return "Let(" +
                let.name + ", " +
                to_debug_string(*let.init) + ", " +
                to_debug_string(*let.body) + ")";
        }
    case expr_kind_t::LetFun: {
            auto const& letfun = cast<letfun_expr_t>(e);
            std::string args = letfun.type.args[0].name;
            for (size_t i=1; i<letfun.type.args.size(); ++i)
                args += ", " + letfun.type.args[i].name;
            auto init = to_debug_string(*letfun.init);
            auto body = to_debug_string(*letfun.body);
            return format("LetFun({}, [{}], {}, {})",
                    letfun.name, args, init, body);
        }
    case expr_kind_t::If: {
            auto const& if_expr = cast<if_expr_t>(e);
            return "If(" +
                to_debug_string(*if_expr.cond_expr) + ", " +
                to_debug_string(*if_expr.true_expr) + ", " +
                to_debug_string(*if_expr.false_expr) + ")";
        }
    case expr_kind_t::App: {
            auto const& app_expr = cast<app_expr_t>(e);
            auto f = to_debug_string(*app_expr.f);
            std::string args = to_debug_string(*app_expr.args[0]);
            for (size_t i=1; i<app_expr.args.size(); ++i) {
                args += ", " + to_debug_string(*app_expr.args[i]);
            }
            return format("App({}, [{}])", f, args);
        }
    case expr_kind_t::Rand: {
            auto const& rand_expr = cast<rand_expr_t>(e);
            return "Rand(" +
                std::to_string(rand_expr.start) + ", " +
                std::to_string(rand_expr.end) + ")";
        }
    case expr_kind_t::Eq:  case expr_kind_t::Neq:
    case expr_kind_t::Leq: case expr_kind_t::Geq:
    case expr_kind_t::Add: case expr_kind_t::Sub:
    case expr_kind_t::Mul: case expr_kind_t::Div:
    case expr_kind_t::And: case expr_kind_t::Or:  {
            auto const& binop_expr = cast<binop_expr_t>(e);
            return to_string(binop_expr.kind()) + "(" +
                to_debug_string(*binop_expr.lhs) + ", " +
                to_debug_string(*binop_expr.rhs) + ")";
        }
    case expr_kind_t::Neg:
        return "Neg(" + to_debug_string(*cast<neg_expr_t>(e).inner) + ")";
    case expr_kind_t::Typed: {
            auto const& typed_expr = cast<typed_expr_t>(e);
            return "Typed(" +
                to_debug_string(*typed_expr.expr) + ", " +
                to_debug_string(typed_expr.type) + ")";
        }
    case expr_kind_t::Int:
        return std::to_string(cast<int_expr_t>(e).n);
    case expr_kind_t::Bool:
        if (cast<bool_expr_t>(e).b)
            return "true";
        else
            return "false";
    case expr_kind_t::Var:
        return cast<var_expr_t>(e).name;
    default:
        throw std::logic_error{"invalid expr to make debug string : " + std::to_string(static_cast<int>(e.kind()))};
    }
}
}

