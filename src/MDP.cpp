#include <algorithm>

#include "expr_ast.hpp"
#include "MDP.hpp"

namespace mdp {

mdp_t mdp_t::merge(mdp_t&& lhs, mdp_t&& rhs) {
    mdp_t result{std::move(lhs)};

    result.variables.reserve(result.variables.size() + rhs.variables.size());
    for (auto&& var : rhs.variables) {
        if (var.is_int() && var.name == "location")
            continue;
        auto found = std::find_if(
                result.variables.begin(), result.variables.end(),
                [&](mdp::variable_t const& var_) {
                    return var_.name == var.name;
                });
        if (found != result.variables.end())
            continue;
        result.variables.emplace_back(std::move(var));
    }

    result.constants.reserve(result.constants.size() + rhs.constants.size());
    for (auto&& cnst : rhs.constants) {
        auto found = std::find_if(
                result.constants.begin(), result.constants.end(),
                [&](mdp::constant_t const& cnst_) {
                    return cnst_.name == cnst.name;
                });
        if (found != result.constants.end())
            continue;
        result.constants.emplace_back(std::move(cnst));
    }

    result.commands.reserve(result.commands.size() + rhs.commands.size());
    std::move(
            rhs.commands.begin(), rhs.commands.end(),
            std::back_inserter(result.commands));
    return result;
}

std::ostream& operator<<(std::ostream& os, command_t const& command) {
    if (command.branches.empty())
        return os;
    os << "[] " << *command.guard << " -> ";
    os << *command.branches[0].prob << " : " << *command.branches[0].update;
    for (size_t i=1; i<command.branches.size(); ++i)
        os << "+" << *command.branches[i].prob << " : " << *command.branches[i].update;
    os << ";";
    return os;
}

std::ostream& operator<<(std::ostream& os, mdp_t const& mdp) {
    os << "mdp" << std::endl << std::endl;
    os << "module " << mdp.module_name << std::endl << std::endl;

    for (auto const& var : mdp.variables)
        os << var << std::endl;
    for (auto const& cnst : mdp.constants)
        os << cnst << std::endl;

    os << std::endl;

    for (auto const& command : mdp.commands)
        os << command << std::endl;

    os << std::endl;
    os << "endmodule";
    return os;
}

bool operator==(branch_t const& lhs, branch_t const& rhs) {
    return
        *lhs.prob == *rhs.prob &&
        *lhs.update == *rhs.update;
}

bool operator==(command_t const& lhs, command_t const& rhs) {
    if (*lhs.guard != *rhs.guard)
        return false;
    return lhs.branches == rhs.branches;
}

bool operator==(mdp_t const& lhs, mdp_t const& rhs) {
    if (lhs.variables.size() != rhs.variables.size())
        return false;

    for (size_t i=0; i<lhs.variables.size(); ++i) {
        if (lhs.variables[i] != rhs.variables[i])
            return false;
    }

    if (lhs.commands.size() != rhs.commands.size())
        return false;

    for (size_t i=0; i<lhs.commands.size(); ++i) {
        if (lhs.commands[i] != rhs.commands[i])
            return false;
    }
    return true;
}

}


