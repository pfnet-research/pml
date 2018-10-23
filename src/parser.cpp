#include <cctype>
#include <iostream>
#include <unordered_map>

#include "parser.hpp"
#include "type_ast.hpp"

struct token_t {
    enum class kind_t {
        Let, LetFun, In, If, Else, Then, Rand,
        Int, True, False, Ident, Arrow, FatArrow,
        Eq, Plus, Minus, Star, Slash, Comma,
        DoubleEq, Neq, Less, Leq, Geq, Greater,
        Amp, BackSlash, Bar, Colon,
        LBrace, RBrace, LParen, RParen,
        Or, And, Prob, Not,
        Eof, Dummy
    };
    kind_t kind;
    size_t pos;
    std::string lexime;
};

std::string token_kind_to_string(token_t::kind_t kind) {
    using kind_t = token_t::kind_t;
    switch (kind) {
    case kind_t::Let:
        return "let";
    case kind_t::LetFun:
        return "letfun";
    case kind_t::In:
        return "in";
    case kind_t::If:
        return "if";
    case kind_t::Then:
        return "then";
    case kind_t::Else:
        return "else";
    case kind_t::Rand:
        return "rand";
    case kind_t::Ident:
        return "identifier";
    case kind_t::Arrow:
        return "->";
    case kind_t::FatArrow:
        return "=>";
    case kind_t::Or:
        return "\\/";
    case kind_t::And:
        return "/\\";
    case kind_t::Prob:
        return "Prob";
    case kind_t::Not:
        return "not";
    case kind_t::DoubleEq:
        return "==";
    case kind_t::Neq:
        return "!=";
    case kind_t::Less:
        return "<";
    case kind_t::Leq:
        return "<=";
    case kind_t::Geq:
        return ">=";
    case kind_t::Greater:
        return ">";
    case kind_t::Eq:
        return "=";
    case kind_t::Plus:
        return "+";
    case kind_t::Minus:
        return "-";
    case kind_t::Star:
        return "*";
    case kind_t::Slash:
        return "/";
    case kind_t::Comma:
        return ",";
    case kind_t::Amp:
        return "&";
    case kind_t::BackSlash:
        return "\\";
    case kind_t::Bar:
        return "|";
    case kind_t::Colon:
        return ":";
    case kind_t::Int:
        return "number";
    case kind_t::True:
        return "true";
    case kind_t::False:
        return "false";
    case kind_t::LParen:
        return "(";
    case kind_t::RParen:
        return ")";
    case kind_t::LBrace:
        return "{";
    case kind_t::RBrace:
        return "}";
    case kind_t::Eof:
        return "eof";
    case kind_t::Dummy:
        return "dummy";
    }
}

size_t skip_spaces(std::string const& input, size_t pos) {
    while (std::isspace(input[pos]) && pos < input.size()) {
        ++pos;
    }
    return pos;
}

size_t ident_len(std::string const& input, size_t pos) {
    if (!std::isalpha(input[pos]) || pos >= input.size())
        return 0;
    size_t len = 1;
    while ((std::isalnum(input[pos+len]) || input[pos+len] == '_') && pos < input.size()) {
        ++len;
    }
    return len;
}

token_t next_token(std::string const& input, size_t pos) {
    using kind_t = token_t::kind_t;

    pos = skip_spaces(input, pos);

    if (pos >= input.size()) {
        return token_t {
            token_t::kind_t::Eof, pos, ""
        };
    }

    if (std::isdigit(input[pos]) || (input[pos] == '-' && std::isdigit(input[pos+1]))) {
        size_t len = 1;
        while (std::isdigit(input[pos + len]) && pos + len < input.size())
            ++len;
        return token_t {
            kind_t::Int, pos, input.substr(pos, len)
        };
    }

    std::unordered_map<std::string, kind_t> string_token = {
        {"->", kind_t::Arrow},
        {"=>", kind_t::FatArrow},
        {"==", kind_t::DoubleEq},
        {"!=", kind_t::Neq},
        {"<=", kind_t::Leq},
        {">=", kind_t::Geq},
        {"\\/", kind_t::Or},
        {"/\\", kind_t::And}
    };
    for (auto const& pair : string_token) {
        if (pair.first == input.substr(pos, pair.first.size()))
            return token_t {pair.second, pos, pair.first};
    }

    std::unordered_map<char, token_t::kind_t> char_token = {
        {'=', kind_t::Eq},
        {'+', kind_t::Plus},
        {'-', kind_t::Minus},
        {'*', kind_t::Star},
        {'/', kind_t::Slash},
        {',', kind_t::Comma},
        {'&', kind_t::Amp},
        {'\\', kind_t::BackSlash},
        {'|', kind_t::Bar},
        {':', kind_t::Colon},
        {'{', kind_t::LBrace},
        {'}', kind_t::RBrace},
        {'(', kind_t::LParen},
        {')', kind_t::RParen}
    };
    auto found_ch = char_token.find(input[pos]);
    if (found_ch != char_token.end()) {
        return token_t {
            found_ch->second, pos, std::string{found_ch->first}
        };
    }

    size_t len = ident_len(input, pos);
    if (len != 0) {
        std::unordered_map<std::string, token_t::kind_t> keywords = {
            {"let", kind_t::Let},
            {"letfun", kind_t::LetFun},
            {"in", kind_t::In},
            {"if", kind_t::If},
            {"else", kind_t::Else},
            {"then", kind_t::Then},
            {"rand", kind_t::Rand},
            {"true", kind_t::True},
            {"false", kind_t::False},
            {"Prob", kind_t::Prob},
            {"not", kind_t::Not},
            {"dummy", kind_t::Dummy}
        };
        std::string lexime = input.substr(pos, len);
        auto found_keyword = keywords.find(lexime);
        if (found_keyword != keywords.end()) {
            return token_t { found_keyword->second, pos, lexime };
        } else {
            return token_t { kind_t::Ident, pos,  lexime};
        }
    }

    throw std::runtime_error{"unknown token at " + std::to_string(pos) + " : " + input.substr(pos, 5) + ".."};
}

parser::result_t<token_t> expect_token(std::string const& input, size_t pos, token_t::kind_t kind) {
    auto token = next_token(input, pos);
    if (token.kind != kind) {
        return parser::result_t<token_t>::error(parser::error_t {
            parser::error_t::kind_t::UnexpectedToken, pos,
            "expected was " + token_kind_to_string(kind) + ", but coming is " + token.lexime
        });
    } else
        return token;
}

namespace parser {

#define EXPECT(X, K) \
    auto X ## _result = expect_token(input, pos, token_t::kind_t::K); \
    if (X ## _result.is_error()) \
        return term_result_t::error(X ## _result.error()); \
    auto X = X ## _result.ok(); \
    pos = X.pos + X.lexime.size()

term_result_t term(std::string const&, size_t&);
formula_result_t formula(std::string const&, size_t&);

term_result_t var_term(std::string const& input, size_t& pos) {
    EXPECT(ident_token, Ident);
    pos = ident_token.pos + ident_token.lexime.size();
    return term_result_t::ok(make<logic::var_term_t>(ident_token.lexime));
}

term_result_t primary_term(std::string const& input, size_t& pos) {
    auto token = next_token(input, pos);
    if (token.kind == token_t::kind_t::Ident) {
        return var_term(input, pos);
    } else if (token.kind == token_t::kind_t::LParen) {
        EXPECT(lparen_token, LParen);
        auto inner_result = term(input, pos);
        if (inner_result.is_error())
            return inner_result;
        EXPECT(rparen_token, RParen);
        return inner_result;
    } else if (token.kind == token_t::kind_t::Prob) {
        EXPECT(prob_token, Prob);
        EXPECT(lparen_token, LParen);
        auto inner_result = formula(input, pos);
        if (inner_result.is_error())
            return inner_result.convert<ptr<logic::term_t>>();
        EXPECT(rparen_token, RParen);
        return term_result_t::ok(make<logic::prob_term_t>(inner_result.ok()));
    } else if (token.kind == token_t::kind_t::Int) {
        EXPECT(int_token, Int);
        pos = int_token.pos + int_token.lexime.size();
        return term_result_t::ok(make<logic::int_term_t>(std::stoi(int_token.lexime)));
    } else {
        return term_result_t::error(parser::error_t {
            parser::error_t::kind_t::UnexpectedToken, pos,
            "expected was " + token_kind_to_string(token.kind) + ", but coming is " + token.lexime
        });
    }
}

term_result_t multive_term(std::string const& input, size_t& pos) {
    auto head_result = primary_term(input, pos);
    if (head_result.is_error())
        return head_result;
    auto acc = head_result.ok();

    while (true) {
        auto op_token = next_token(input, pos);
        if (op_token.kind != token_t::kind_t::Star && op_token.kind != token_t::kind_t::Slash)
            break;
        pos = op_token.pos + op_token.lexime.size();

        auto term_result = primary_term(input, pos);
        if (term_result.is_error())
            return term_result;

        switch (op_token.kind) {
        case token_t::kind_t::Star:
            acc = make<logic::mul_term_t>(acc, term_result.ok());
            break;
        case token_t::kind_t::Slash:
            acc = make<logic::div_term_t>(acc, term_result.ok());
            break;
        default:
            throw std::logic_error{"invalid additive op"};
        }
    }
    return acc;

}

term_result_t additive_term(std::string const& input, size_t& pos) {
    auto head_result = multive_term(input, pos);
    if (head_result.is_error())
        return head_result;
    auto acc = head_result.ok();

    while (true) {
        auto op_token = next_token(input, pos);
        if (op_token.kind != token_t::kind_t::Plus && op_token.kind != token_t::kind_t::Minus)
            break;
        pos = op_token.pos + op_token.lexime.size();

        auto term_result = multive_term(input, pos);
        if (term_result.is_error())
            return term_result;

        switch (op_token.kind) {
        case token_t::kind_t::Plus:
            acc = make<logic::add_term_t>(acc, term_result.ok());
            break;
        case token_t::kind_t::Minus:
            acc = make<logic::sub_term_t>(acc, term_result.ok());
            break;
        default:
            throw std::logic_error{"invalid additive op"};
        }
    }
    return acc;
}

term_result_t term(std::string const& input, size_t& pos) {
    return additive_term(input, pos);
}

#undef EXPECT

#define EXPECT(X, K) \
    auto X ## _result = expect_token(input, pos, token_t::kind_t::K); \
    if (X ## _result.is_error()) \
        return formula_result_t::error(X ## _result.error()); \
    auto X = X ## _result.ok(); \
    pos = X.pos + X.lexime.size()

formula_result_t comparison_terms_formula(std::string const& input, size_t& pos) {
    auto lhs_result = term(input, pos);
    if (lhs_result.is_error())
        return lhs_result.convert<ptr<logic::formula_t>>();
    auto token = next_token(input, pos);
    if (token.kind != token_t::kind_t::Eq &&
            token.kind != token_t::kind_t::Less &&
            token.kind != token_t::kind_t::Leq &&
            token.kind != token_t::kind_t::Geq &&
            token.kind != token_t::kind_t::Greater) {
        return formula_result_t::error(error_t {
                error_t::kind_t::UnexpectedToken, pos,
                "expected was comparison operation, but comming is " + token.lexime
                });

    }
    pos = token.pos + token.lexime.size();
    auto rhs_result = term(input, pos);
    if (rhs_result.is_error())
        return rhs_result.convert<ptr<logic::formula_t>>();

    switch (token.kind) {
    case token_t::kind_t::Eq:
        return formula_result_t::ok(make<logic::eq_formula_t>(lhs_result.ok(), rhs_result.ok()));
    case token_t::kind_t::Less:
        return formula_result_t::ok(make<logic::less_formula_t>(lhs_result.ok(), rhs_result.ok()));
    case token_t::kind_t::Leq:
        return formula_result_t::ok(make<logic::leq_formula_t>(lhs_result.ok(), rhs_result.ok()));
    case token_t::kind_t::Geq:
        return formula_result_t::ok(make<logic::geq_formula_t>(lhs_result.ok(), rhs_result.ok()));
    case token_t::kind_t::Greater:
        return formula_result_t::ok(make<logic::greater_formula_t>(lhs_result.ok(), rhs_result.ok()));
    default:
        throw std::logic_error{"must be unreachable"};
    }
}

formula_result_t primary_formula(std::string const& input, size_t& pos) {
    auto token = next_token(input, pos);
    if (token.kind == token_t::kind_t::True) {
        pos = token.pos + token.lexime.size();
        return formula_result_t::ok(make<logic::top_formula_t>());
    } else if (token.kind == token_t::kind_t::False) {
        pos = token.pos + token.lexime.size();
        return formula_result_t::ok(make<logic::bot_formula_t>());
    } else if (token.kind == token_t::kind_t::LParen) {
        EXPECT(lparen_token, LParen);
        auto inner_result = formula(input, pos);
        if (inner_result.is_error())
            return inner_result;
        EXPECT(rparen_token, RParen);
        return inner_result;
    } else {
        auto cmp_result = comparison_terms_formula(input, pos);
        if (cmp_result.is_ok())
            return cmp_result;
        if (token.kind == token_t::kind_t::Ident) {
            pos = token.pos + token.lexime.size();
            return formula_result_t::ok(make<logic::var_formula_t>(token.lexime));
        } else {
            return formula_result_t::error(parser::error_t {
                parser::error_t::kind_t::UnexpectedToken, pos,
                "expected was " + token_kind_to_string(token.kind) + ", but coming is " + token.lexime
            });
        }
    }
}

formula_result_t neg_formula(std::string const& input, size_t& pos) {
    auto not_token = next_token(input, pos);
    if (not_token.kind != token_t::kind_t::Not)
        return primary_formula(input, pos);
    pos = not_token.pos + not_token.lexime.size();

    auto inner_result = neg_formula(input, pos);
    if (inner_result.is_error())
        return inner_result;
    return formula_result_t::ok(make<logic::neg_formula_t>(inner_result.ok()));
}

formula_result_t and_formula(std::string const& input, size_t& pos) {
    auto head_result = neg_formula(input, pos);
    if (head_result.is_error())
        return head_result;
    auto acc = head_result.ok();

    while (true) {
        auto and_token = next_token(input, pos);
        if (and_token.kind != token_t::kind_t::And)
            break;
        pos = and_token.pos + and_token.lexime.size();

        auto formula_result = neg_formula(input, pos);
        if (formula_result.is_error())
            return formula_result;
        acc = make<logic::and_formula_t>(acc, formula_result.ok());
    }
    return acc;
}

formula_result_t or_formula(std::string const& input, size_t& pos) {
    auto head_result = and_formula(input, pos);
    if (head_result.is_error())
        return head_result;
    auto acc = head_result.ok();

    while (true) {
        auto or_token = next_token(input, pos);
        if (or_token.kind != token_t::kind_t::Or)
            break;
        pos = or_token.pos + or_token.lexime.size();

        auto formula_result = and_formula(input, pos);
        if (formula_result.is_error())
            return formula_result;
        acc = make<logic::or_formula_t>(acc, formula_result.ok());
    }
    return acc;
}

formula_result_t impl_formula(std::string const& input, size_t& pos) {
    auto head_result = or_formula(input, pos);
    if (head_result.is_error())
        return head_result;
    auto acc = head_result.ok();

    while (true) {
        auto impl_token = next_token(input, pos);
        if (impl_token.kind != token_t::kind_t::FatArrow)
            break;
        pos = impl_token.pos + impl_token.lexime.size();

        auto formula_result = or_formula(input, pos);
        if (formula_result.is_error())
            return formula_result;
        acc = make<logic::impl_formula_t>(acc, formula_result.ok());
    }
    return acc;
}

formula_result_t formula(std::string const& input, size_t& pos) {
    return impl_formula(input, pos);
}

#undef EXPECT

result_t<logic::domain_kind_t> simple_type(std::string const& input, size_t& pos) {
    auto sty_result = expect_token(input, pos, token_t::kind_t::Ident);
    if (sty_result.is_error())
        return sty_result.convert<logic::domain_kind_t>();
    auto sty = sty_result.ok();
    pos = sty.pos + sty.lexime.size();

    if (sty.lexime == "int")
        return result_t<logic::domain_kind_t>::ok(logic::domain_kind_t::Int);
    else if (sty.lexime == "bool")
        return result_t<logic::domain_kind_t>::ok(logic::domain_kind_t::Bool);
    else
        return result_t<logic::domain_kind_t>::error(error_t {
                error_t::kind_t::UnexpectedToken, pos,
                "expected was 'int' or 'bool', but coming is " + sty.lexime
                });
}

#define EXPECT(X, K) \
    auto X ## _result = expect_token(input, pos, token_t::kind_t::K); \
    if (X ## _result.is_error()) \
        return predicate_result_t::error(X ## _result.error()); \
    auto X = X ## _result.ok(); \
    pos = X.pos + X.lexime.size()

predicate_result_t predicate(std::string const& input, size_t& pos) {
    EXPECT(backslash_token, BackSlash);

    EXPECT(arg_token, Ident);
    auto arg_name = arg_token.lexime;

    EXPECT(colon_token, Colon);

    auto stype_result = simple_type(input, pos);
    if (stype_result.is_error())
        return stype_result.convert<logic::predicate_t>();
    auto stype = stype_result.ok();

    EXPECT(fat_arrow_token, FatArrow);

    auto formula_result = formula(input, pos);
    if (formula_result.is_error())
        return formula_result.convert<logic::predicate_t>();
    auto formula = formula_result.ok();

    return predicate_result_t::ok(logic::predicate_t{arg_name, stype, formula});
}

#undef EXPECT

using refty_result_t = result_t<ast::refinement_type_t>;

#define EXPECT(X, K) \
    auto X ## _result = expect_token(input, pos, token_t::kind_t::K); \
    if (X ## _result.is_error()) \
        return refty_result_t::error(X ## _result.error()); \
    auto X = X ## _result.ok(); \
    pos = X.pos + X.lexime.size()

// {x:int|\phi}
refty_result_t refty_detail(std::string const& input, size_t& pos) {
    EXPECT(lbrace_token, LBrace);
    EXPECT(ident_token, Ident);
    EXPECT(colon_token, Colon);
    auto sty_result = simple_type(input, pos);
    if (sty_result.is_error())
        return sty_result.convert<ast::refinement_type_t>();
    EXPECT(bar_token, Bar);
    auto constraint_result = formula(input, pos);
    if (constraint_result.is_error())
        return constraint_result.convert<ast::refinement_type_t>();
    EXPECT(rbrace_token, RBrace);
    return refty_result_t::ok(ast::refinement_type_t {
            ident_token.lexime, sty_result.ok(), constraint_result.ok()
            });
}

// x:int for {x:int|true}
// x:bool for {x:bool|true}
// int for {blah:int|true}
// bool for {blah:bool|true}
refty_result_t refty_abbreviation(std::string const& input, size_t& pos) {
    std::string arg = "@blah";
    size_t prev_pos = pos;
    auto sty_result = simple_type(input, pos); // in the cold night : a:int
    if (sty_result.is_error()) {
        pos = prev_pos;
        EXPECT(ident_token, Ident);
        EXPECT(colon_token, Colon);
        arg = ident_token.lexime;
        sty_result = simple_type(input, pos);
    }
    if (sty_result.is_ok()) {
        return refty_result_t::ok(ast::refinement_type_t {
            arg, sty_result.ok(), make<logic::top_formula_t>()
            });
    } else {
        return sty_result.convert<ast::refinement_type_t>();
    }
}

refty_result_t refinement_type(std::string const& input, size_t& pos) {
    size_t prev_pos= pos;
    auto refty_result = refty_detail(input, pos);
    if (refty_result.is_ok())
        return refty_result;
    pos = prev_pos;
    return refty_abbreviation(input, pos);
}

#undef EXPECT

using depty_result_t = result_t<ast::dependent_type_t>;

#define EXPECT(X, K) \
    auto X ## _result = expect_token(input, pos, token_t::kind_t::K); \
    if (X ## _result.is_error()) \
        return depty_result_t::error(X ## _result.error()); \
    auto X = X ## _result.ok(); \
    pos = X.pos + X.lexime.size()

// ({n:int|\phi}, {b:bool|\phi}) -> {x:int|\phi}
// {n:int|\phi} -> {x:int|\phi}
depty_result_t dependent_type(std::string const& input, size_t& pos) {
    auto token = next_token(input, pos);
    std::vector<ast::refinement_type_t> args;
    if (token.kind == token_t::kind_t::LParen) {
        EXPECT(lparen_token, LParen);
        auto refty_result = refinement_type(input, pos);
        if (refty_result.is_error())
            return refty_result.convert<ast::dependent_type_t>();
        args.push_back(refty_result.ok());
        while (true) {
            if (next_token(input, pos).kind != token_t::kind_t::Comma)
                break;
            EXPECT(comma_token, Comma);
            auto refty_result = refinement_type(input, pos);
            if (refty_result.is_error())
                return refty_result.convert<ast::dependent_type_t>();
            args.push_back(refty_result.ok());
        }
        EXPECT(rparen_token, RParen);
    } else {
        auto refty_result = refinement_type(input, pos);
        if (refty_result.is_error())
            return refty_result.convert<ast::dependent_type_t>();
        args.push_back(refty_result.ok());
    }
    EXPECT(arrow_token, Arrow);
    auto retty_result = refinement_type(input, pos);
    if (retty_result.is_error())
        return retty_result.convert<ast::dependent_type_t>();
    return depty_result_t::ok(ast::dependent_type_t {
            args, retty_result.ok()
            });
}

#undef EXPECT

expr_result_t expr(std::string const& input, size_t& pos);

#define EXPECT(X, K) \
    auto X ## _result = expect_token(input, pos, token_t::kind_t::K); \
    if (X ## _result.is_error()) \
        return expr_result_t::error(X ## _result.error()); \
    auto X = X ## _result.ok(); \
    pos = X.pos + X.lexime.size()

// let [ident] = [expr] in [expr]
expr_result_t let_expr(std::string const& input, size_t& pos) {

    EXPECT(let_token, Let);
    EXPECT(var_token, Ident);
    EXPECT(eq_token, Eq);

    auto init_result = expr(input, pos);
    if (init_result.is_error())
        return init_result;
    auto init = init_result.ok();

    EXPECT(in_token, In);

    auto body_result = expr(input, pos);
    if (body_result.is_error())
        return body_result;
    auto body = body_result.ok();

    return expr_result_t::ok(make<ast::let_expr_t>(
                var_token.lexime, init, body
            ));
}

// letfun [ident] [refty]+ : [refty] = [expr] in [expr]
expr_result_t letfun_expr(std::string const& input, size_t& pos) {
    EXPECT(letfun_token, LetFun);
    EXPECT(ident_token, Ident);
    auto name = ident_token.lexime;
    auto depty_result = dependent_type(input, pos);
    if (depty_result.is_error())
        return depty_result.convert<ptr<ast::expr_t>>();
    auto type = depty_result.ok();

    EXPECT(eq_token, Eq);

    auto init_result = expr(input, pos);
    if (init_result.is_error())
        return init_result;
    auto init = init_result.ok();

    EXPECT(in_token, In);

    auto body_result = expr(input, pos);
    if (body_result.is_error())
        return body_result;
    auto body = body_result.ok();

    return expr_result_t::ok(make<ast::letfun_expr_t>(name, type, init, body));
}

// if [expr] then [expr] else [expr]
expr_result_t if_expr(std::string const& input, size_t& pos) {

    EXPECT(if_token, If);

    auto cond_result = expr(input, pos);
    if (cond_result.is_error())
        return cond_result;
    auto cond_expr = cond_result.ok();

    EXPECT(then_token, Then);

    auto true_result = expr(input, pos);
    if (true_result.is_error())
        return true_result;
    auto true_expr = true_result.ok();

    EXPECT(else_token, Else);

    auto false_result = expr(input, pos);
    if (false_result.is_error())
        return false_result;
    auto false_expr = false_result.ok();

    return expr_result_t::ok(make<ast::if_expr_t>(
                cond_expr, true_expr, false_expr));
}

// number or boolean or variable or parened expr
expr_result_t primary_expr(std::string const& input, size_t& pos) {
    auto token = next_token(input, pos);
    if (token.kind == token_t::kind_t::Int) {
        pos = token.pos + token.lexime.size();
        return expr_result_t::ok(make<ast::int_expr_t>(std::stoi(token.lexime)));
    } else if (token.kind == token_t::kind_t::True) {
        pos = token.pos + token.lexime.size();
        return expr_result_t::ok(make<ast::bool_expr_t>(true));
    } else if (token.kind == token_t::kind_t::False) {
        pos = token.pos + token.lexime.size();
        return expr_result_t::ok(make<ast::bool_expr_t>(false));
    } else if (token.kind == token_t::kind_t::Rand) {
        EXPECT(rand_token, Rand);
        EXPECT(lparen_token, LParen);
        EXPECT(start_token, Int);
        int start = std::stoi(start_token.lexime);
        EXPECT(comma_token, Comma);
        EXPECT(end_token, Int);
        int end = std::stoi(end_token.lexime);
        EXPECT(rparen_token, RParen);
        return expr_result_t::ok(make<ast::rand_expr_t>(start, end));
    } else if (token.kind == token_t::kind_t::Ident) {
        pos = token.pos + token.lexime.size();
        return expr_result_t::ok(make<ast::var_expr_t>(token.lexime));
    } else if (token.kind == token_t::kind_t::LParen) {
        EXPECT(lparen_token, LParen);
        auto body_result = expr(input, pos);
        if (body_result.is_error())
            return  body_result;
        EXPECT(rparen_token, RParen);
        return body_result;
    } else {
        return expr_result_t::error(error_t {
                error_t::kind_t::UnexpectedToken, pos,
                "expected was number or boolean or paren, but comming is " + token.lexime + "(" + token_kind_to_string(token.kind) + ")"
                });
    }
}

expr_result_t neg_expr_t(std::string const& input, size_t& pos) {
    auto minus_token = next_token(input, pos);
    if (minus_token.kind != token_t::kind_t::Not)
        return primary_expr(input, pos);
    pos = minus_token.pos + minus_token.lexime.size();

    auto inner_result = neg_expr_t(input, pos);
    if (inner_result.is_ok())
        return expr_result_t::ok(make<ast::neg_expr_t>(inner_result.ok()));
    else
        return inner_result;
}

// [expr] : [qualified-type]
expr_result_t typed_expr(std::string const& input, size_t& pos) {
    auto expr_result = neg_expr_t(input, pos);
    if (expr_result.is_error())
        return expr_result;

    auto colon_token = next_token(input, pos);
    if (colon_token.kind != token_t::kind_t::Colon)
        return expr_result;
    pos = colon_token.pos + colon_token.lexime.size();

    auto type_result = refinement_type(input, pos);
    if (type_result.is_error())
        return type_result.convert<ptr<ast::expr_t>>();

    auto expr = expr_result.ok();
    auto type = type_result.ok();

    return expr_result_t::ok(make<ast::typed_expr_t>(expr, type));
}

// [expr] [expr]+
expr_result_t applicative_expr(std::string const& input, size_t& pos) {
    auto f_result = typed_expr(input, pos);
    if (f_result.is_error())
        return f_result;
    auto f = f_result.ok();

    std::vector<ptr<ast::expr_t>> args;
    while (true) {
        auto e_result = typed_expr(input, pos);
        if (e_result.is_error())
            break;
        args.push_back(e_result.ok());
    }

    if (args.empty())
        return f;
    else
        return expr_result_t::ok(make<ast::app_expr_t>(f, args));
}

// [expr] ((*|/) [expr])*
expr_result_t multive_expr(std::string const& input, size_t& pos) {
    auto head_result = applicative_expr(input, pos);
    if (head_result.is_error())
        return head_result;
    auto acc = head_result.ok();

    while (true) {
        auto op_token = next_token(input, pos);
        if (op_token.kind != token_t::kind_t::Star && op_token.kind != token_t::kind_t::Slash)
            break;
        pos = op_token.pos + op_token.lexime.size();

        auto e_result = applicative_expr(input, pos);
        if (e_result.is_error())
            return e_result;
        auto e = e_result.ok();

        switch (op_token.kind) {
        case token_t::kind_t::Star:
            acc = make<ast::mul_expr_t>(acc, e);
            break;
        case token_t::kind_t::Slash:
            acc = make<ast::div_expr_t>(acc, e);
            break;
        default:
            throw std::logic_error{"invalid additive op"};
        }
    }
    return acc;
}


// [expr] ((+|-) [expr])*
expr_result_t additive_expr(std::string const& input, size_t& pos) {
    auto head_result = multive_expr(input, pos);
    if (head_result.is_error())
        return head_result;
    auto acc = head_result.ok();

    while (true) {
        auto op_token = next_token(input, pos);
        if (op_token.kind != token_t::kind_t::Plus && op_token.kind != token_t::kind_t::Minus)
            break;
        pos = op_token.pos + op_token.lexime.size();

        auto e_result = multive_expr(input, pos);
        if (e_result.is_error())
            return e_result;
        auto e = e_result.ok();

        switch (op_token.kind) {
        case token_t::kind_t::Plus:
            acc = make<ast::add_expr_t>(acc, e);
            break;
        case token_t::kind_t::Minus:
            acc = make<ast::sub_expr_t>(acc, e);
            break;
        default:
            throw std::logic_error{"invalid additive op"};
        }
    }
    return acc;
}


// [expr] ((==|!=|<=|>=) [expr])*
expr_result_t equive_expr(std::string const& input, size_t& pos) {
    auto head_result = additive_expr(input, pos);
    if (head_result.is_error())
        return head_result;
    auto acc = head_result.ok();

    while (true) {
        auto op_token = next_token(input, pos);
        if (op_token.kind != token_t::kind_t::DoubleEq &&
            op_token.kind != token_t::kind_t::Neq &&
            op_token.kind != token_t::kind_t::Leq &&
            op_token.kind != token_t::kind_t::Geq)
            break;
        pos = op_token.pos + op_token.lexime.size();

        auto e_result = additive_expr(input, pos);
        if (e_result.is_error())
            return e_result;
        auto e = e_result.ok();

        switch (op_token.kind) {
        case token_t::kind_t::DoubleEq:
            acc = make<ast::eq_expr_t>(acc, e);
            break;
        case token_t::kind_t::Neq:
            acc = make<ast::neq_expr_t>(acc, e);
            break;
        case token_t::kind_t::Leq:
            acc = make<ast::leq_expr_t>(acc, e);
            break;
        case token_t::kind_t::Geq:
            acc = make<ast::geq_expr_t>(acc, e);
            break;
        default:
            throw std::logic_error{"invalid formula op"};
        }
    }
    return acc;
}

// [expr] (& [expr])*
expr_result_t and_expr(std::string const& input, size_t& pos) {
    auto head_result = equive_expr(input, pos);
    if (head_result.is_error())
        return head_result;
    auto acc = head_result.ok();

    while (true) {
        auto op_token = next_token(input, pos);
        if (op_token.kind != token_t::kind_t::And)
            break;
        pos = op_token.pos + op_token.lexime.size();

        auto e_result = equive_expr(input, pos);
        if (e_result.is_error())
            return e_result;
        acc = make<ast::and_expr_t>(acc, e_result.ok());
    }
    return acc;

}

// [expr] (| [expr])*
expr_result_t or_expr(std::string const& input, size_t& pos) {
    auto head_result = and_expr(input, pos);
    if (head_result.is_error())
        return head_result;
    auto acc = head_result.ok();

    while (true) {
        auto op_token = next_token(input, pos);
        if (op_token.kind != token_t::kind_t::Or)
            break;
        pos = op_token.pos + op_token.lexime.size();

        auto e_result = and_expr(input, pos);
        if (e_result.is_error())
            return e_result;
        acc = make<ast::and_expr_t>(acc, e_result.ok());
    }
    return acc;
}

#undef EXPECT

expr_result_t expr(std::string const& input, size_t& pos) {
    auto token = next_token(input, pos);
    switch (token.kind) {
    case token_t::kind_t::Let:
        return let_expr(input, pos);
    case token_t::kind_t::LetFun:
        return letfun_expr(input, pos);
    case token_t::kind_t::If:
        return if_expr(input, pos);
    default:
        return or_expr(input, pos);
    }
}

expr_result_t parse(std::string const& input) {
    size_t pos = 0;
    return expr(input, pos);
}

result_t<ast::refinement_type_t> parse_reftype(std::string const& input) {
    size_t pos = 0;
    return refinement_type(input, pos);
}
result_t<ast::dependent_type_t> parse_deptype(std::string const& input) {
    size_t pos = 0;
    return dependent_type(input, pos);
}
formula_result_t parse_formula(std::string const& input) {
    size_t pos = 0;
    return formula(input, pos);
}
term_result_t parse_term(std::string const& input) {
    size_t pos = 0;
    return term(input, pos);
}



}

