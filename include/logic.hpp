
#ifndef PML_LOGIC_HPP
#define PML_LOGIC_HPP

#include "utility.hpp"

namespace logic {

enum class term_kind_t {
    Var, Int, Add, Sub, Mul, Div, Prob
};

struct term_t {
    virtual term_kind_t kind() const = 0;
    virtual ~term_t() = default;
};

struct formula_t;

struct var_term_t : public term_t {
    std::string name;
    explicit var_term_t(std::string const& name) :
        name{name}
    {}
    term_kind_t kind() const override {
        return term_kind_t::Var;
    }
};

struct int_term_t : public term_t {
    int n;
    explicit int_term_t(int n) : n{n} {}
    term_kind_t kind() const override {
        return term_kind_t::Int;
    }
};

struct binop_term_t : public term_t {
    ptr<term_t> lhs, rhs;
    explicit binop_term_t(ptr<term_t> const& lhs, ptr<term_t> const& rhs) :
        lhs{lhs}, rhs{rhs}
    {}
};

struct add_term_t : public binop_term_t {
    using binop_term_t::binop_term_t;
    term_kind_t kind() const override {
        return term_kind_t::Add;
    }
};

struct sub_term_t : public binop_term_t {
    using binop_term_t::binop_term_t;
    term_kind_t kind() const override {
        return term_kind_t::Sub;
    }
};

struct mul_term_t : public binop_term_t {
    using binop_term_t::binop_term_t;
    term_kind_t kind() const override {
        return term_kind_t::Mul;
    }
};

struct div_term_t : public binop_term_t {
    using binop_term_t::binop_term_t;
    term_kind_t kind() const override {
        return term_kind_t::Div;
    }
};

struct prob_term_t : public term_t {
    ptr<formula_t> inner;
    explicit prob_term_t(ptr<formula_t> const& inner) :
        inner{inner}
    {}
    term_kind_t kind() const override {
        return term_kind_t::Prob;
    }
};

enum class formula_kind_t {
    Var, Bot, Top, Neg, And, Or, Impl,
    Eq, Lt, Leq, Geq, Gt
};

struct formula_t {
    virtual formula_kind_t kind() const = 0;
    virtual ~formula_t() = default;
};

struct var_formula_t : public formula_t {
    std::string name;
    explicit var_formula_t(std::string const& name) :
        name{name}
    {}
    formula_kind_t kind() const override {
        return formula_kind_t::Var;
    }
};

struct bot_formula_t : public formula_t {
    explicit bot_formula_t() {}
    formula_kind_t kind() const override {
        return formula_kind_t::Bot;
    }
};

struct top_formula_t : public formula_t {
    explicit top_formula_t() {}
    formula_kind_t kind() const override {
        return formula_kind_t::Top;
    }
};

struct neg_formula_t : public formula_t {
    ptr<formula_t> inner;

    explicit neg_formula_t(ptr<formula_t> const& inner) :
        inner{inner}
    {}
    formula_kind_t kind() const override {
        return formula_kind_t::Neg;
    }
};

struct and_formula_t : public formula_t {
    ptr<formula_t> lhs, rhs;

    explicit and_formula_t(
            ptr<formula_t> const& lhs,
            ptr<formula_t> const& rhs) :
        lhs{lhs}, rhs{rhs}
    {}
    formula_kind_t kind() const override {
        return formula_kind_t::And;
    }
};

struct or_formula_t : public formula_t {
    ptr<formula_t> lhs, rhs;

    explicit or_formula_t(
            ptr<formula_t> const& lhs,
            ptr<formula_t> const& rhs) :
        lhs{lhs}, rhs{rhs}
    {}
    formula_kind_t kind() const override {
        return formula_kind_t::Or;
    }
};

struct impl_formula_t : public formula_t {
    ptr<formula_t> lhs, rhs;

    explicit impl_formula_t(
            ptr<formula_t> const& lhs,
            ptr<formula_t> const& rhs) :
        lhs{lhs}, rhs{rhs}
    {}
    formula_kind_t kind() const override {
        return formula_kind_t::Impl;
    }
};

struct binop_formula_t : public formula_t {
    ptr<term_t> lhs, rhs;
    explicit binop_formula_t(ptr<term_t> const& lhs, ptr<term_t> const& rhs) :
        lhs{lhs}, rhs{rhs}
    {}
};

struct eq_formula_t : public binop_formula_t {
    using binop_formula_t::binop_formula_t;
    formula_kind_t kind() const override {
        return formula_kind_t::Eq;
    }
};

struct less_formula_t : public binop_formula_t {
    using binop_formula_t::binop_formula_t;
    formula_kind_t kind() const override {
        return formula_kind_t::Lt;
    }
};

struct leq_formula_t : public binop_formula_t {
    using binop_formula_t::binop_formula_t;
    formula_kind_t kind() const override {
        return formula_kind_t::Leq;
    }
};

struct geq_formula_t : public binop_formula_t {
    using binop_formula_t::binop_formula_t;
    formula_kind_t kind() const override {
        return formula_kind_t::Geq;
    }
};

struct greater_formula_t : public binop_formula_t {
    using binop_formula_t::binop_formula_t;
    formula_kind_t kind() const override {
        return formula_kind_t::Gt;
    }
};

enum class domain_kind_t {
    Int, Bool
};

struct predicate_t {
    std::string arg_name;
    domain_kind_t arg_domain;
    ptr<formula_t> body;
};

// [t2/var]t1
ptr<term_t> subst(ptr<term_t> const& t1, std::string const& var, ptr<formula_t> const& t2);
ptr<term_t> subst(ptr<term_t> const& t, std::string const& var, ptr<formula_t> const& f);
ptr<formula_t> subst(ptr<formula_t> const& f, std::string const& var, ptr<term_t> const& t);
ptr<formula_t> subst(ptr<formula_t> const& f1, std::string const& var, ptr<formula_t> const& f2);

std::string to_debug_string(domain_kind_t);
std::string to_debug_string(term_t const&);
std::string to_debug_string(formula_t const&);
std::string to_debug_string(predicate_t const&);

std::string output(term_t const& term, int accept, bool pos);
std::string output(formula_t const& formula, int accept, bool pos);

inline static std::ostream& operator<<(std::ostream& os, term_t const& t) {
    os << to_debug_string(t);
    return os;
}
inline static std::ostream& operator<<(std::ostream& os, formula_t const& f) {
    os << to_debug_string(f);
    return os;
}

bool operator==(term_t const&, term_t const&);
bool operator==(formula_t const&, formula_t const&);
inline static bool operator!=(term_t const& lhs, term_t const& rhs) {
    return !(lhs == rhs);
}
inline static bool operator!=(formula_t const& lhs, formula_t const& rhs) {
    return !(lhs == rhs);
}

template<typename T>
inline static auto const& cast(formula_t const& f) {
    return dynamic_cast<T const&>(f);
}
template<typename T>
inline static auto& cast(formula_t& f) {
    return dynamic_cast<T&>(f);
}

template<typename T>
inline static auto const& cast(term_t const& t) {
    return dynamic_cast<T const&>(t);
}
template<typename T>
inline static auto& cast(term_t& t) {
    return dynamic_cast<T&>(t);
}

}

#endif
