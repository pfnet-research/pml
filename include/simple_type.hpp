#ifndef PML_SIMPLE_TYPE_HPP
#define PML_SIMPLE_TYPE_HPP

#include "utility.hpp"
#include "result.hpp"

namespace ast {
struct expr_t;
struct refinement_type_t;
struct dependent_type_t;
}

namespace simty {

enum class kind_t {
    Int, Bool, Fun
};

struct type_t {
    virtual kind_t kind() const = 0;
    virtual ~type_t() = default;

    static ptr<type_t> from(ast::refinement_type_t const&);
    static ptr<type_t> from(ast::dependent_type_t const&);
};

struct int_type_t : public type_t {
    explicit int_type_t() = default;
    kind_t kind() const override {
        return kind_t::Int;
    }
};

struct bool_type_t : public type_t {
    explicit bool_type_t() = default;
    kind_t kind() const override {
        return kind_t::Bool;
    }
};

struct fun_type_t : public type_t {
    std::vector<ptr<type_t>> args;
    ptr<type_t> ret;
    explicit fun_type_t(
            std::vector<ptr<type_t>> const& args,
            ptr<type_t> const& ret) :
        args{args}, ret{ret}
    {}
    kind_t kind() const override {
        return kind_t::Fun;
    };
};

template<typename T>
inline static T const& cast(type_t const& ty) {
    return dynamic_cast<T const&>(ty);
}

std::string to_debug_string(type_t const&);

inline static bool operator==(type_t const& lhs, type_t const& rhs) {
    if (lhs.kind() != rhs.kind())
        return false;
    if (lhs.kind() != kind_t::Fun)
        return true;
    auto const& l = cast<fun_type_t>(lhs);
    auto const& r = cast<fun_type_t>(rhs);
    if (!(*l.ret == *r.ret))
        return false;
    if (l.args.size() != r.args.size())
        return false;
    for (size_t i=0; i<l.args.size(); ++i) {
        if (!(l.args[i] == r.args[i]))
            return false;
    }
    return true;
}
inline static bool operator!=(type_t const& lhs, type_t const& rhs) {
    return !(lhs == rhs);
}

using result_t = ::result_t<ptr<type_t>>;

result_t simple_typing(ast::expr_t const&);

inline static std::ostream& operator<<(std::ostream& os, type_t const& ty) {
    os << to_debug_string(ty);
    return os;
}

}

#endif
