#include "type_ast.hpp"

namespace ast {

bool operator==(refinement_type_t const& lhs, refinement_type_t const& rhs) {
    return
        lhs.name == rhs.name &&
        lhs.domain == rhs.domain &&
        *lhs.constraint == *rhs.constraint;
}

std::string to_debug_string(refinement_type_t const& ty) {
    return format("Ref({}, {}, {})",
            ty.name, to_debug_string(ty.domain),
            to_debug_string(*ty.constraint));
}

bool operator==(dependent_type_t const& lhs, dependent_type_t const& rhs) {
    return lhs.args == rhs.args && lhs.ret_type == rhs.ret_type;
}

std::string to_debug_string(dependent_type_t const& ty) {
    std::string args = to_debug_string(ty.args[0]);
    for (size_t i=1; i<ty.args.size(); ++i)
        args += format(", {}", to_debug_string(ty.args[i]));
    return format("Dep([{}], {})",
            args,
            to_debug_string(ty.ret_type));
}

}

