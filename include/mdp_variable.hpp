#ifndef PML_MDP_VARIABLE_HPP
#define PML_MDP_VARIABLE_HPP

#include <string>
#include <boost/variant.hpp>
#include "utility.hpp"

namespace mdp {

struct int_var_t {
    bound_t bound;
    int init;
};

struct bool_var_t {
    bool init;
};

struct variable_t {
    std::string name;
    boost::variant<int_var_t, bool_var_t> data;

    explicit variable_t() = default;
    explicit variable_t(std::string const& name) :
        name{name}
    {}
    explicit variable_t(
            std::string const& name,
            bound_t const& bound, int init) :
        name{name}, data{int_var_t{bound, init}}
    {}
    explicit variable_t(std::string const& name, bool b) :
        name{name}, data{bool_var_t{b}}
    {}

    variable_t& operator=(variable_t const& rhs) {
        name = rhs.name;
        data = rhs.data;
        return *this;
    }
    bool is_int() const { return data.which() == 0; }
    bool is_bool() const { return data.which() == 1; }
    int_var_t const& as_int() const {
        return boost::get<int_var_t>(data);
    }
    bool_var_t const& as_bool() const {
        return boost::get<bool_var_t>(data);
    }
};

std::ostream& operator<<(std::ostream&, variable_t const&);
bool operator==(variable_t const&, variable_t const&);
inline static bool operator!=(variable_t const& lhs, variable_t const& rhs) {
    return !(lhs == rhs);
}

}

#endif
