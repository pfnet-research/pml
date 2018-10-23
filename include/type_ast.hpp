#ifndef PML_TYPE_AST_HPP
#define PML_TYPE_AST_HPP

#include <ostream>
#include "logic.hpp"

namespace ast {

enum class simple_type_t {
    Int, Bool
};

struct refinement_type_t {
    std::string name;
    logic::domain_kind_t domain;
    ptr<logic::formula_t> constraint;
};
bool operator==(refinement_type_t const&, refinement_type_t const&);
inline static bool operator!=(refinement_type_t const& lhs, refinement_type_t const& rhs) {
    return !(lhs == rhs);
}
std::string to_debug_string(refinement_type_t const&);
inline static std::ostream& operator<<(std::ostream& os, refinement_type_t const& ty) {
    os << to_debug_string(ty);
    return os;
}

struct dependent_type_t {
    std::vector<refinement_type_t> args;
    refinement_type_t ret_type;
};
bool operator==(dependent_type_t const&, dependent_type_t const&);
inline static bool operator!=(dependent_type_t const& lhs, dependent_type_t const& rhs) {
    return !(lhs == rhs);
}
std::string to_debug_string(dependent_type_t const&);
inline static std::ostream& operator<<(std::ostream& os, dependent_type_t const& ty) {
    os << to_debug_string(ty);
    return os;
}

}

#endif
