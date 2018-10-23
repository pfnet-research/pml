#ifndef PML_MDP_EXPR_HPP
#define PML_MDP_EXPR_HPP

namespace mdp {

enum class expr_kind_t {
    Int, Real, Bool, Var,
    Neg, BinOp, If,
    Min, Max, Floor, Ceil, Pow, Mod, Log
};

struct expr_t {
    virtual expr_kind_t kind() const = 0;
    virtual ~expr_t() = default;
};

struct int_expr_t : public expr_t {
    int n;
    explicit int_expr_t(int n) :
        n{n}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Int;
    };
};

struct real_expr_t : public expr_t {
    double d;
    explicit real_expr_t(double d) :
        d{d}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Real;
    }
};

struct bool_expr_t : public expr_t {
    bool b;
    explicit bool_expr_t(bool b) :
        b{b}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Bool;
    }
};

struct var_expr_t : public expr_t {
    std::string name;
    explicit var_expr_t(std::string const& name) :
        name{name}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Var;
    }
};

struct neg_expr_t : public expr_t {
    ptr<expr_t> inner;
    explicit neg_expr_t(ptr<expr_t> e) :
        inner{e}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Neg;
    }
};

enum class binop_kind_t {
    Mul, Div, Add, Sub,
    Lt, Leq, Geq, Gt, Eq, Neq,
    And, Or, Iff, Impl
};

std::ostream& operator<<(std::ostream&, binop_kind_t);

struct binop_expr_t : public expr_t {
    ptr<expr_t> lhs, rhs;
    binop_kind_t binop_kind;
    explicit binop_expr_t(ptr<expr_t> lhs, ptr<expr_t> rhs, binop_kind_t kind) :
        lhs{lhs}, rhs{rhs},
        binop_kind{kind}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::BinOp;
    }
};

struct if_expr_t : public expr_t {
    ptr<expr_t> cond, true_branch, false_branch;
    explicit if_expr_t(ptr<expr_t> cond, ptr<expr_t> t, ptr<expr_t> f) :
        cond{cond},
        true_branch{t}, false_branch{f}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::If;
    }
};

struct min_expr_t : public expr_t {
    std::vector<ptr<expr_t>> elems;
    explicit min_expr_t(std::vector<ptr<expr_t>> elems) :
        elems{elems}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Min;
    }
};
struct max_expr_t : public expr_t {
    std::vector<ptr<expr_t>> elems;
    explicit max_expr_t(std::vector<ptr<expr_t>> elems) :
        elems{elems}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Max;
    }
};
struct floor_expr_t : public expr_t {
    ptr<expr_t> inner;
    explicit floor_expr_t(ptr<expr_t> inner) :
        inner{inner}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Floor;
    }
};
struct ceil_expr_t : public expr_t {
    ptr<expr_t> inner;
    explicit ceil_expr_t(ptr<expr_t> inner) :
        inner{inner}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Ceil;
    }
};
struct pow_expr_t : public expr_t {
    ptr<expr_t> x, y;
    explicit pow_expr_t(ptr<expr_t> x, ptr<expr_t> y) :
        x{x}, y{y}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Pow;
    }
};
struct mod_expr_t : public expr_t {
    ptr<expr_t> i, n;
    explicit mod_expr_t(ptr<expr_t> i, ptr<expr_t> n) :
        i{i}, n{n}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Mod;
    }
};
struct log_expr_t : public expr_t {
    ptr<expr_t> x, b;
    explicit log_expr_t(ptr<expr_t> x, ptr<expr_t> b) :
        x{x}, b{b}
    {}
    expr_kind_t kind() const override {
        return expr_kind_t::Log;
    }
};

std::ostream& operator<<(std::ostream&, expr_t const&);
bool operator==(expr_t const&, expr_t const&);
inline static bool operator!=(expr_t const& lhs, expr_t const& rhs) {
    return !(lhs == rhs);
}

template<typename T>
inline static auto const& cast(expr_t const& e) {
    return dynamic_cast<T const&>(e);
}
template<typename T>
inline static auto& cast(expr_t& e) {
    return dynamic_cast<T&>(e);
}

}

#endif
