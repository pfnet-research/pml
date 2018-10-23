#include "PCTL.hpp"

namespace pctl {

void pctl_t::output(std::ostream& os) const {
    os << logic::output(*constraint, final_location, true) << std::endl;
}

}

