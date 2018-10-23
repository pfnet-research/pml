#ifndef PML_PARSER_HPP
#define PML_PARSER_HPP

#include <string>
#include <memory>

#include "expr_ast.hpp"
#include "result.hpp"

namespace parser {

struct error_t {
    enum class kind_t {
        UnknownToken,
        UnexpectedToken,

        Debug
    };
    kind_t kind;
    size_t pos;
    std::string message;
};

template<typename T>
using result_t = ::result_t<T, error_t>;

using expr_result_t = result_t<ptr<ast::expr_t>>;
using predicate_result_t = result_t<logic::predicate_t>;
using formula_result_t = result_t<ptr<logic::formula_t>>;
using term_result_t = result_t<ptr<logic::term_t>>;

expr_result_t parse(std::string const& input);
result_t<ast::refinement_type_t> parse_reftype(std::string const& input);
result_t<ast::dependent_type_t> parse_deptype(std::string const& input);
formula_result_t parse_formula(std::string const& input);
term_result_t parse_term(std::string const& input);

}


#endif



