#include "mdp_variable.hpp"

namespace mdp {

std::ostream& operator<<(std::ostream& os, variable_t const& var) {
    if (var.is_int()) { // bounded integer
        std::string const& name = var.name;
        int min = var.as_int().bound.min;
        int max = var.as_int().bound.max;
        int init = var.as_int().init;
        if (min == max)
            os << name << " : int init " << min << ";";
        else
            os << name << " : [" << min << ".." << max << "] init " << init << ";";
    } else { // bool
        os << var.name << ": bool init " << std::boolalpha << var.as_bool().init << ";";
    }
    return os;
}

bool operator==(variable_t const& lhs, variable_t const& rhs) {
    if (lhs.is_int() != rhs.is_int())
        return false;

    if (lhs.is_int()) {
        return lhs.name == rhs.name &&
            lhs.as_int().init == rhs.as_int().init;
    } else {
        return lhs.name == rhs.name &&
            lhs.as_bool().init == rhs.as_bool().init;
    }
}

}

