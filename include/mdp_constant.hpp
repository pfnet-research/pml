#include <boost/variant.hpp>

namespace mdp {

struct constant_t {
    std::string name;
    boost::variant<int, bool> data;
    explicit constant_t() = default;
    explicit constant_t(std::string const& name, int n) :
        name{name}, data{n}
    {}
    explicit constant_t(std::string const& name, bool b) :
        name{name}, data{b}
    {}
    bool is_int() const {
        return data.which() == 0;
    }
    bool is_bool() const {
        return data.which() == 1;
    }
    int as_int() const {
        return boost::get<int>(data);
    }
    bool as_bool() const {
        return boost::get<bool>(data);
    }
};

std::ostream& operator<<(std::ostream&, constant_t const&);
bool operator==(constant_t const&, constant_t const&);
inline static bool operator!=(constant_t const& lhs, constant_t const& rhs) {
    return !(lhs == rhs);
}

}

