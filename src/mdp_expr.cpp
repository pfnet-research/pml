#include "utility.hpp"
#include "mdp_expr.hpp"

namespace mdp {

std::ostream& operator<<(std::ostream& os, binop_kind_t kind) {
    switch (kind) {
    case binop_kind_t::Mul:
        os << "*"; break;
    case binop_kind_t::Div:
        os << "/"; break;
    case binop_kind_t::Add:
        os << "+"; break;
    case binop_kind_t::Sub:
        os << "-"; break;
    case binop_kind_t::Lt:
        os << "<"; break;
    case binop_kind_t::Leq:
        os << "<="; break;
    case binop_kind_t::Geq:
        os << ">="; break;
    case binop_kind_t::Gt:
        os << ">"; break;
    case binop_kind_t::Eq:
        os << "="; break;
    case binop_kind_t::Neq:
        os << "!="; break;
    case binop_kind_t::And:
        os << "&"; break;
    case binop_kind_t::Or:
        os << "|"; break;
    case binop_kind_t::Iff:
        os << "<=>"; break;
    case binop_kind_t::Impl:
        os << "=>"; break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, expr_t const& e) {
    switch (e.kind()) {
    case expr_kind_t::Int:
        os << cast<int_expr_t>(e).n; break;
    case expr_kind_t::Real:
        os << cast<real_expr_t>(e).d; break;
    case expr_kind_t::Bool:
        os << cast<bool_expr_t>(e).b; break;
    case expr_kind_t::Var:
        os << cast<var_expr_t>(e).name; break;
    case expr_kind_t::Neg:
        os << "!(" << *cast<neg_expr_t>(e).inner << ")";
        break;
    case expr_kind_t::BinOp:
        if (cast<binop_expr_t>(e).binop_kind != binop_kind_t::Eq) {
            os <<
                *cast<binop_expr_t>(e).lhs <<
                cast<binop_expr_t>(e).binop_kind <<
                *cast<binop_expr_t>(e).rhs;
        } else {
            os << "(" <<
                *cast<binop_expr_t>(e).lhs <<
                cast<binop_expr_t>(e).binop_kind <<
                *cast<binop_expr_t>(e).rhs << ")";
        }
        break;
    case expr_kind_t::If:
        os << "(" <<
            *cast<if_expr_t>(e).cond << "?" <<
            *cast<if_expr_t>(e).true_branch << ":" <<
            *cast<if_expr_t>(e).false_branch << ")";
        break;
    case expr_kind_t::Min: {
        auto const& args = cast<min_expr_t>(e).elems;
        os << "min(" << args[0];
        for (size_t i=1; i<args.size(); ++i)
            os << ", " << args[i];
        os << ")";
        break;
        }
    case expr_kind_t::Max: {
        auto const& args = cast<max_expr_t>(e).elems;
        os << "max(" << args[0];
        for (size_t i=1; i<args.size(); ++i)
            os << ", " << args[i];
        os << ")";
        break;
        }
    case expr_kind_t::Floor:
        os << "floor(" << *cast<floor_expr_t>(e).inner << ")";
        break;
    case expr_kind_t::Ceil:
        os << "ceil(" << *cast<ceil_expr_t>(e).inner << ")";
        break;
    case expr_kind_t::Pow:
        os << "pow(" <<
            *cast<pow_expr_t>(e).x << "" <<
            *cast<pow_expr_t>(e).y << ")";
        break;
    case expr_kind_t::Mod:
        os << "mod(" <<
            *cast<mod_expr_t>(e).i << "" <<
            *cast<mod_expr_t>(e).n << ")";
        break;
    case expr_kind_t::Log:
        os << "log(" <<
            *cast<log_expr_t>(e).x << "" <<
            *cast<log_expr_t>(e).b << ")";
        break;
    }
    return os;
}

bool operator==(expr_t const& lhs, expr_t const& rhs) {
    if (lhs.kind() != rhs.kind())
        return false;
    if (lhs.kind() == expr_kind_t::Int)
        return cast<int_expr_t>(lhs).n == cast<int_expr_t>(rhs).n;
    if (lhs.kind() == expr_kind_t::Real)
        return cast<real_expr_t>(lhs).d == cast<real_expr_t>(rhs).d;
    if (lhs.kind() == expr_kind_t::Bool)
        return cast<bool_expr_t>(lhs).b == cast<bool_expr_t>(rhs).b;
    if (lhs.kind() == expr_kind_t::Var)
        return cast<var_expr_t>(lhs).name == cast<var_expr_t>(rhs).name;
    if (lhs.kind() == expr_kind_t::Neg)
        return *cast<neg_expr_t>(lhs).inner == *cast<neg_expr_t>(rhs).inner;
    if (lhs.kind() == expr_kind_t::BinOp) {
        auto const& lhs_ = cast<binop_expr_t>(lhs);
        auto const& rhs_ = cast<binop_expr_t>(rhs);
        return
            *lhs_.lhs == *rhs_.lhs && *lhs_.rhs == *rhs_.rhs &&
            lhs_.binop_kind == rhs_.binop_kind;
    }
    if (lhs.kind() == expr_kind_t::If) {
        auto const& lhs_ = cast<if_expr_t>(lhs);
        auto const& rhs_ = cast<if_expr_t>(rhs);
        return
            *lhs_.cond == *rhs_.cond &&
            *lhs_.true_branch == *rhs_.true_branch &&
            *lhs_.false_branch == *rhs_.false_branch;
    }
    if (lhs.kind() == expr_kind_t::Min) {
        auto const& lhs_args = cast<min_expr_t>(lhs).elems;
        auto const& rhs_args = cast<min_expr_t>(rhs).elems;
        if (lhs_args.size() != rhs_args.size())
            return false;
        for (size_t i=0; i<lhs_args.size(); ++i) {
            if (*lhs_args[i] != *rhs_args[i])
                return false;
        }
        return true;
    }
    if (lhs.kind() == expr_kind_t::Max) {
        auto const& lhs_args = cast<max_expr_t>(lhs).elems;
        auto const& rhs_args = cast<max_expr_t>(rhs).elems;
        if (lhs_args.size() != rhs_args.size())
            return false;
        for (size_t i=0; i<lhs_args.size(); ++i) {
            if (*lhs_args[i] != *rhs_args[i])
                return false;
        }
        return true;
    }
    if (lhs.kind() == expr_kind_t::Floor)
        return *cast<floor_expr_t>(lhs).inner == *cast<floor_expr_t>(rhs).inner;
    if (lhs.kind() == expr_kind_t::Ceil)
        return *cast<ceil_expr_t>(lhs).inner == *cast<ceil_expr_t>(rhs).inner;
    if (lhs.kind() == expr_kind_t::Pow) {
        return
            *cast<pow_expr_t>(lhs).x == *cast<pow_expr_t>(rhs).x &&
            *cast<pow_expr_t>(lhs).y == *cast<pow_expr_t>(rhs).y;
    }
    if (lhs.kind() == expr_kind_t::Mod) {
        return
            *cast<mod_expr_t>(lhs).i == *cast<mod_expr_t>(rhs).i &&
            *cast<mod_expr_t>(lhs).n == *cast<mod_expr_t>(rhs).n;
    }
    if (lhs.kind() == expr_kind_t::Log) {
        return
            *cast<log_expr_t>(lhs).x == *cast<log_expr_t>(rhs).x &&
            *cast<log_expr_t>(lhs).b == *cast<log_expr_t>(rhs).b;
    }
    throw std::logic_error{"unreachable"};
}

}

