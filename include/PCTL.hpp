#ifndef PML_PCTL_HPP
#define PML_PCTL_HPP

#include <ostream>

#include "logic.hpp"

namespace pctl {

struct pctl_t {
    int final_location;
    ptr<logic::formula_t> constraint;

    void output(std::ostream&) const;
};

}

#endif
