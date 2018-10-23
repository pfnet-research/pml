#ifndef PML_ENVIRONMENT_HPP
#define PML_ENVIRONMENT_HPP

#include <string>
#include <memory>
#include <boost/container/flat_map.hpp>
#include "utility.hpp"

template<typename T>
struct environment_t {
    using element_t = T;
    boost::container::flat_map<std::string, ptr<element_t>> elems;
    environment_t append(std::string const& name, ptr<element_t> const& val) const {
        auto result = *this;
        auto found = result.elems.find(name);
        if (found != result.elems.end())
            result.elems.erase(name);
        result.elems.emplace(name, val);
        return result;
    }
    ptr<element_t> lookup(std::string const& name) const {
        for (auto const& e : elems) {
            if (e.first == name)
                return e.second;
        }
        return nullptr;
    }
};

#endif
