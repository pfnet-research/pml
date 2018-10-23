#ifndef PML_EXPR_AST_HPP
#define PML_EXPR_AST_HPP

#include <memory>
#include <string>
#include <ostream>
#include "utility.hpp"
#include "type_ast.hpp"

namespace ast {

enum class expr_kind_t {
    Let, LetFun, If, App,
    Rand, Typed,
    Add, Sub, Mul, Div,
    Neg, Eq, Neq, Leq, Geq,
    And, Or,
    Int, Bool, Fun, Var
};

struct expr_t {
    virtual expr_kind_t kind() const = 0;
    virtual ~expr_t() = default;
};

struct let_expr_t : public expr_t {
    std::string name;
    ptr<expr_t> init;
    ptr<expr_t> body;

    explicit let_expr_t(
            std::string const& name,
            ptr<expr_t> init,
            ptr<expr_t> body) :
        name{name}, init{init}, body{body}
    {}

    expr_kind_t kind() const override {
        return expr_kind_t::Let;
    }
};

struct letfun_expr_t : public expr_t {
    std::string name;
    ast::dependent_type_t type;
    ptr<expr_t> init;
    ptr<expr_t> body;

    explicit letfun_expr_t(
            std::string const& name,
            ast::dependent_type_t const& type,
            ptr<expr_t> const& init,
            ptr<expr_t> const& body) :
        name{name}, type{type}, init{init}, body{body}
    {}

    expr_kind_t kind() const override {
        return expr_kind_t::LetFun;
    }
};

struct if_expr_t : public expr_t {
    ptr<expr_t> cond_expr;
    ptr<expr_t> true_expr, false_expr;

    explicit if_expr_t(
            ptr<expr_t> cond,
            ptr<expr_t> true_e,
            ptr<expr_t> false_e) :
        cond_expr{cond}, true_expr{true_e}, false_expr{false_e}
    {}

    expr_kind_t kind() const override {
        return expr_kind_t::If;
    }
};

struct fun_expr_t : public expr_t {
    dependent_type_t type;
    ptr<expr_t> body;

    explicit fun_expr_t(dependent_type_t const& type, ptr<expr_t> const& body) :
        type{type}, body{body}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Fun;
    }
};

struct app_expr_t : public expr_t {
    ptr<expr_t> f;
    std::vector<ptr<expr_t>> args;

    explicit app_expr_t(
            ptr<expr_t> const& f,
            std::vector<ptr<expr_t>> const& args) :
        f{f}, args{args}
    {}

    expr_kind_t kind() const override {
        return expr_kind_t::App;
    }
};

struct rand_expr_t : public expr_t {
    int start, end;

    explicit rand_expr_t(int start, int end) :
        start{start}, end{end}
    {}

    expr_kind_t kind() const override {
        return expr_kind_t::Rand;
    }
};

struct binop_expr_t : public expr_t {
    ptr<expr_t> lhs, rhs;

    explicit binop_expr_t(
            ptr<expr_t> lhs,
            ptr<expr_t> rhs) :
        lhs{lhs}, rhs{rhs}
    {}
};

struct eq_expr_t : public binop_expr_t {
    using binop_expr_t::binop_expr_t;
    expr_kind_t kind() const override {
        return expr_kind_t::Eq;
    }
};

struct neq_expr_t : public binop_expr_t {
    using binop_expr_t::binop_expr_t;
    expr_kind_t kind() const override {
        return expr_kind_t::Neq;
    }
};

struct leq_expr_t : public binop_expr_t {
    using binop_expr_t::binop_expr_t;
    expr_kind_t kind() const override {
        return expr_kind_t::Leq;
    }
};

struct geq_expr_t : public binop_expr_t {
    using binop_expr_t::binop_expr_t;
    expr_kind_t kind() const override {
        return expr_kind_t::Geq;
    }
};

struct and_expr_t : public binop_expr_t {
    using binop_expr_t::binop_expr_t;
    expr_kind_t kind() const override {
        return expr_kind_t::And;
    }
};

struct or_expr_t : public binop_expr_t {
    using binop_expr_t::binop_expr_t;
    expr_kind_t kind() const override {
        return expr_kind_t::Or;
    }
};

struct add_expr_t : public binop_expr_t {
    using binop_expr_t::binop_expr_t;
    expr_kind_t kind() const override {
        return expr_kind_t::Add;
    }
};

struct sub_expr_t : public binop_expr_t {
    using binop_expr_t::binop_expr_t;
    expr_kind_t kind() const override {
        return expr_kind_t::Sub;
    }
};

struct mul_expr_t : public binop_expr_t {
    using binop_expr_t::binop_expr_t;
    expr_kind_t kind() const override {
        return expr_kind_t::Mul;
    }
};

struct div_expr_t : public binop_expr_t {
    using binop_expr_t::binop_expr_t;
    expr_kind_t kind() const override {
        return expr_kind_t::Div;
    }
};

struct neg_expr_t : public expr_t {
    ptr<expr_t> inner;
    explicit neg_expr_t(ptr<expr_t> inner) :
        inner{inner}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Neg;
    }
};

struct typed_expr_t : public expr_t {
    ptr<expr_t> expr;
    refinement_type_t type;

    explicit typed_expr_t(
            ptr<expr_t> expr,
            refinement_type_t const& type) :
        expr{expr},
        type{type}
    {}

    expr_kind_t kind() const override {
        return expr_kind_t::Typed;
    }
};

struct int_expr_t : public expr_t {
    int n;
    explicit int_expr_t(int n) :
        n{n}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Int;
    }
};

inline static bool operator==(int_expr_t const& lhs, int_expr_t const& rhs) {
    return lhs.n == rhs.n;
}

struct bool_expr_t : public expr_t {
    bool b;
    explicit bool_expr_t(bool b) :
        b{b}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Bool;
    }
};

inline static bool operator==(bool_expr_t const& lhs, bool_expr_t const& rhs) {
    return lhs.b == rhs.b;
}

struct var_expr_t : public expr_t {
    std::string name;
    explicit var_expr_t(std::string const& name) :
        name{name}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Var;
    }
};

template<expr_kind_t>
struct expr_kind_t_to_type;

#define EXPR_KIND_TO_TYPE(X, Y) \
    template<> struct expr_kind_t_to_type<expr_kind_t::X> { \
        using kind = Y; \
    }

EXPR_KIND_TO_TYPE(Let, let_expr_t);
EXPR_KIND_TO_TYPE(LetFun, letfun_expr_t);
EXPR_KIND_TO_TYPE(If, if_expr_t);
EXPR_KIND_TO_TYPE(Rand, rand_expr_t);
EXPR_KIND_TO_TYPE(App, app_expr_t);
EXPR_KIND_TO_TYPE(Eq, eq_expr_t);
EXPR_KIND_TO_TYPE(Neq, neq_expr_t);
EXPR_KIND_TO_TYPE(Leq, leq_expr_t);
EXPR_KIND_TO_TYPE(Geq, geq_expr_t);
EXPR_KIND_TO_TYPE(Add, add_expr_t);
EXPR_KIND_TO_TYPE(Sub, sub_expr_t);
EXPR_KIND_TO_TYPE(Mul, mul_expr_t);
EXPR_KIND_TO_TYPE(Div, div_expr_t);
EXPR_KIND_TO_TYPE(Int, int_expr_t);
EXPR_KIND_TO_TYPE(Bool, bool_expr_t);
EXPR_KIND_TO_TYPE(Var, var_expr_t);

#undef EXPR_KIND_TO_TYPE

std::string to_string(expr_kind_t);

template<typename T>
inline static auto const& cast(expr_t const& e) {
    return dynamic_cast<T const&>(e);
}
template<typename T>
inline static auto& cast(expr_t& e) {
    return dynamic_cast<T&>(e);
}

bool operator==(expr_t const&, expr_t const&);
inline static bool operator!=(expr_t const& lhs, expr_t const& rhs) {
    return !(lhs == rhs);
}

inline static bool is_value(expr_t const& e) {
    switch (e.kind()) {
    case expr_kind_t::Int:
    case expr_kind_t::Bool:
    case expr_kind_t::Var:
        return true;
    default:
        return false;
    }
}

std::string to_debug_string(expr_t const&);

inline static std::ostream& operator<<(std::ostream& os, expr_t const& e) {
    os << to_debug_string(e);
    return os;
}

}

#endif



