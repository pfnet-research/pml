#include "mdp_constant.hpp"

namespace mdp {

std::ostream& operator<<(std::ostream& os, constant_t const& cnst) {
    // in PRISM v4.4, somehow can not use 'const' keyword
    if (cnst.is_int()) {
        int val = cnst.as_int();
        os << cnst.name << " : [" << val << " .. " << val+1 << "] init " << val << ";";
    } else {
        int val = cnst.as_bool();
        os << cnst.name << " bool init " << std::boolalpha << val << ";";
    }
    return os;
}

}

