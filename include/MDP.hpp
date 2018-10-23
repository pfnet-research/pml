#ifndef PML_MDP_HPP
#define PML_MDP_HPP

#include <vector>
#include <string>
#include <ostream>
#include <unordered_map>

#include "utility.hpp"
#include "mdp_variable.hpp"
#include "mdp_constant.hpp"
#include "mdp_expr.hpp"

namespace mdp {

struct branch_t {
    ptr<expr_t> prob;
    ptr<expr_t> update;
};

// [] guard -> branches
struct command_t {
    ptr<expr_t> guard;
    std::vector<branch_t> branches;
};

struct mdp_t {
    std::string module_name;
    std::vector<variable_t> variables;
    std::vector<constant_t> constants;
    std::vector<command_t> commands;

    static mdp_t merge(mdp_t&& lhs, mdp_t&& rhs);
};

std::ostream& operator<<(std::ostream&, command_t const&);
std::ostream& operator<<(std::ostream&, mdp_t const&);

bool operator==(branch_t const&, branch_t const&);
bool operator==(command_t const&, command_t const&);
inline static bool operator!=(command_t const& lhs, command_t const& rhs) {
    return !(lhs == rhs);
}
bool operator==(mdp_t const&, mdp_t const&);

}

#endif


