#ifndef PML_TRANS_TO_MDP_HPP
#define PML_TRANS_TO_MDP_HPP

#include "utility.hpp"
#include "MDP.hpp"

namespace pctl {
struct pctl_t;
}

namespace ast {
struct expr_t;
struct refinement_type_t;
struct dependent_type_t;
}

struct translation_data {
    static int fresh_location() {
        return location_count++;
    }
    static int current_location() {
        return location_count;
    }
    static std::string fresh_var() {
        return "v" + std::to_string(var_count++);
    }
    static void init() {
        location_count = 0;
        var_count = 0;
    }
private:
    static int location_count;
    static int var_count;
};

struct value_info_t {
    std::string name;
    util::optional<bound_t> bound;
};

struct mdp_with_info_t {
    mdp::mdp_t mdp;
    int init, accept;
    value_info_t value;
};

mdp_with_info_t translate_to_mdp(ast::expr_t const&);
pctl::pctl_t translate_to_pctl(ast::refinement_type_t const&, mdp_with_info_t const&);

#endif



