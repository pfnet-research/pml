#ifndef PML_UTILITY_HPP
#define PML_UTILITY_HPP

#include <memory>
#include <sstream>
#include <vector>
#include <unordered_map>

template<typename T>
using ptr = std::shared_ptr<T>;

template<typename T, typename ... Ts>
inline static auto make(Ts&& ... args) {
    return std::make_shared<T>(std::forward<Ts>(args) ...);
}

inline static std::string format(std::string const& text) {
    return text;
}

template<typename T1, typename T2>
inline static std::ostream& operator<<(std::ostream& os, std::pair<T1, T2> const& p) {
    os << "Pair(" <<  p.first << ", " << p.second << ")";
    return os;
}
template<typename T>
inline static std::ostream& operator<<(std::ostream& os, ptr<T> const& p) {
    os << *p;
    return os;
}
template<typename T>
inline static std::ostream& operator<<(std::ostream& os, std::vector<T> const& v) {
    os << "Vec(";
    for (auto const& e : v)
        os << e << ")";
    os << ")";
    return os;
}
template<typename T1, typename T2>
inline static std::ostream& operator<<(std::ostream& os, std::unordered_map<T1, T2> const& map) {
    os << "Vec(";
    for (auto const& p : map)
        os << "(" << p.first << ", " << p.second << ")";
    os << ")";
    return os;
}


template<typename Head, typename ... Tail>
inline static std::string format(std::string const& text, Head const& head, Tail const& ... tail) {
    auto pos = text.find("{}");
    if (pos == std::string::npos)
        throw std::logic_error{"too few arguments"};
    std::string rest = text.substr(pos+2, text.length());
    std::stringstream ss;
    ss << head;
    return text.substr(0, pos) + ss.str() + format(rest, tail ...);
}

template<typename T1, typename T2>
inline static bool is_equal(
        std::unordered_map<ptr<T1>, ptr<T2>> const& lhs,
        std::unordered_map<ptr<T1>, ptr<T2>> const& rhs) {
    if (lhs.size() != rhs.size())
        return false;
    auto it_l = lhs.begin();
    auto it_r = rhs.begin();
    while (it_l != lhs.end()) {
        if (*it_l->first != *it_r->first)
            return false;
        if (*it_l->second != *it_r->second)
            return false;
        ++it_l;
        ++it_r;
    }
    return true;
}

#include <experimental/optional>

namespace util {

template<typename T>
using optional = std::experimental::optional<T>;
template<typename T>
inline static optional<T> make_optional(T const& val) {
    return optional<T>(val);
}
template<typename T>
inline static optional<T> make_optional(T&& val) {
    return optional<T>(std::move(val));
}
using nullopt_t = std::experimental::nullopt_t;
static auto const& nullopt = std::experimental::nullopt;

}

struct bound_t {
    int min, max;
};

inline static bound_t operator|(bound_t const& lhs, bound_t const& rhs) {
    return bound_t {
        std::min(lhs.min, rhs.min),
        std::max(lhs.max, rhs.max)
    };
}
inline static util::optional<bound_t> operator&(bound_t const& lhs, bound_t const& rhs) {
    auto result = bound_t {
        std::max(lhs.min, rhs.min),
        std::min(lhs.max, rhs.max)
    };
    if (result.min <= result.max)
        return result;
    else
        return util::nullopt;
}
inline static bound_t operator+(bound_t const& lhs, bound_t const& rhs) {
    return bound_t {
        lhs.min + rhs.min,
        lhs.max + rhs.max
    };
}
inline static bound_t operator-(bound_t const& lhs, bound_t const& rhs) {
    return bound_t {
        lhs.min - rhs.max,
        lhs.max - rhs.min
    };
}
inline static bound_t operator*(bound_t const& lhs, bound_t const& rhs) {
    return bound_t {
        lhs.min * rhs.min,
        lhs.max * rhs.max
    };
}
inline static bound_t operator/(bound_t const& lhs, bound_t const& rhs) {
    return bound_t {
        lhs.min / rhs.max,
        lhs.max / rhs.max
    };
}
inline static bound_t operator-(bound_t const& bound) {
    return bound_t {
        -bound.max, -bound.min
    };
}

template<typename T, typename F>
inline static util::optional<T>
operator|(util::optional<T> const& opt, F const& f) {
    if (!opt)
        return util::nullopt;
    else
        return f(*opt);
}


#endif

