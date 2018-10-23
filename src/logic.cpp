#include <string>
#include "logic.hpp"

namespace logic {

ptr<term_t> subst(
        ptr<term_t> const& t1,
        std::string const& var, ptr<term_t> const& t2) {
    switch (t1->kind()) {
    case term_kind_t::Add:
        return make<add_term_t>(
                subst(logic::cast<add_term_t>(*t1).lhs, var, t2),
                subst(logic::cast<add_term_t>(*t1).rhs, var, t2));
    case term_kind_t::Sub:
        return make<sub_term_t>(
                subst(logic::cast<sub_term_t>(*t1).lhs, var, t2),
                subst(logic::cast<sub_term_t>(*t1).rhs, var, t2));
    case term_kind_t::Mul:
        return make<mul_term_t>(
                subst(logic::cast<mul_term_t>(*t1).lhs, var, t2),
                subst(logic::cast<mul_term_t>(*t1).rhs, var, t2));
    case term_kind_t::Div:
        return make<div_term_t>(
                subst(logic::cast<div_term_t>(*t1).lhs, var, t2),
                subst(logic::cast<div_term_t>(*t1).rhs, var, t2));
    case term_kind_t::Prob:
        return make<prob_term_t>(subst(cast<prob_term_t>(*t1).inner, var ,t2));
    case term_kind_t::Var:
        if (var == cast<var_term_t>(*t1).name)
            return t2;
        else
            return t1;
    case term_kind_t::Int:
        return t1;
    }
}

ptr<term_t> subst(
        ptr<term_t> const& t,
        std::string const& var, ptr<formula_t> const& f) {
    switch (t->kind()) {
    case term_kind_t::Add:
        return make<add_term_t>(
                subst(logic::cast<add_term_t>(*t).lhs, var, f),
                subst(logic::cast<add_term_t>(*t).rhs, var, f));
    case term_kind_t::Sub:
        return make<sub_term_t>(
                subst(logic::cast<sub_term_t>(*t).lhs, var, f),
                subst(logic::cast<sub_term_t>(*t).rhs, var, f));
    case term_kind_t::Mul:
        return make<mul_term_t>(
                subst(logic::cast<mul_term_t>(*t).lhs, var, f),
                subst(logic::cast<mul_term_t>(*t).rhs, var, f));
    case term_kind_t::Div:
        return make<div_term_t>(
                subst(logic::cast<div_term_t>(*t).lhs, var, f),
                subst(logic::cast<div_term_t>(*t).rhs, var, f));
    case term_kind_t::Prob:
        return make<prob_term_t>(subst(cast<prob_term_t>(*t).inner, var ,f));
    case term_kind_t::Var:
    case term_kind_t::Int:
        return t;
    }
}

ptr<formula_t> subst(
        ptr<formula_t> const& f,
        std::string const& var, ptr<term_t> const& t) {
    switch (f->kind()) {
    case formula_kind_t::Neg:
        return make<neg_formula_t>(subst(cast<neg_formula_t>(*f).inner, var, t));
    case formula_kind_t::And:
        return make<and_formula_t>(
                subst(cast<and_formula_t>(*f).lhs, var, t),
                subst(cast<and_formula_t>(*f).rhs, var, t));
    case formula_kind_t::Or:
        return make<or_formula_t>(
                subst(cast<or_formula_t>(*f).lhs, var, t),
                subst(cast<or_formula_t>(*f).rhs, var, t));
    case formula_kind_t::Impl:
        return make<impl_formula_t>(
                subst(cast<impl_formula_t>(*f).lhs, var, t),
                subst(cast<impl_formula_t>(*f).rhs, var, t));
    case formula_kind_t::Eq:
        return make<eq_formula_t>(
                subst(cast<eq_formula_t>(*f).lhs, var, t),
                subst(cast<eq_formula_t>(*f).rhs, var, t));
    case formula_kind_t::Lt:
        return make<less_formula_t>(
                subst(cast<less_formula_t>(*f).lhs, var, t),
                subst(cast<less_formula_t>(*f).rhs, var, t));
    case formula_kind_t::Leq:
        return make<leq_formula_t>(
                subst(cast<leq_formula_t>(*f).lhs, var, t),
                subst(cast<leq_formula_t>(*f).rhs, var, t));
    case formula_kind_t::Geq:
        return make<geq_formula_t>(
                subst(cast<geq_formula_t>(*f).lhs, var, t),
                subst(cast<geq_formula_t>(*f).rhs, var, t));
    case formula_kind_t::Gt:
        return make<greater_formula_t>(
                subst(cast<greater_formula_t>(*f).lhs, var, t),
                subst(cast<greater_formula_t>(*f).rhs, var, t));
    case formula_kind_t::Var:
    case formula_kind_t::Top:
    case formula_kind_t::Bot:
        return f;
    }
}

ptr<formula_t> subst(
        ptr<formula_t> const& f1,
        std::string const& var, ptr<formula_t> const& f2) {
    switch (f1->kind()) {
    case formula_kind_t::Var:
        if (cast<var_formula_t>(*f1).name == var)
            return f2;
        else
            return f1;
    case formula_kind_t::Neg:
        return make<neg_formula_t>(subst(cast<neg_formula_t>(*f1).inner, var, f2));
    case formula_kind_t::And:
        return make<and_formula_t>(
                subst(cast<and_formula_t>(*f1).lhs, var, f2),
                subst(cast<and_formula_t>(*f1).rhs, var, f2));
    case formula_kind_t::Or:
        return make<or_formula_t>(
                subst(cast<or_formula_t>(*f1).lhs, var, f2),
                subst(cast<or_formula_t>(*f1).rhs, var, f2));
    case formula_kind_t::Impl:
        return make<impl_formula_t>(
                subst(cast<impl_formula_t>(*f1).lhs, var, f2),
                subst(cast<impl_formula_t>(*f1).rhs, var, f2));
    case formula_kind_t::Eq:
        return make<eq_formula_t>(
                subst(cast<eq_formula_t>(*f1).lhs, var, f2),
                subst(cast<eq_formula_t>(*f1).rhs, var, f2));
    case formula_kind_t::Lt:
        return make<less_formula_t>(
                subst(cast<less_formula_t>(*f1).lhs, var, f2),
                subst(cast<less_formula_t>(*f1).rhs, var, f2));
    case formula_kind_t::Leq:
        return make<leq_formula_t>(
                subst(cast<leq_formula_t>(*f1).lhs, var, f2),
                subst(cast<leq_formula_t>(*f1).rhs, var, f2));
    case formula_kind_t::Geq:
        return make<geq_formula_t>(
                subst(cast<geq_formula_t>(*f1).lhs, var, f2),
                subst(cast<geq_formula_t>(*f1).rhs, var, f2));
    case formula_kind_t::Gt:
        return make<greater_formula_t>(
                subst(cast<greater_formula_t>(*f1).lhs, var, f2),
                subst(cast<greater_formula_t>(*f1).rhs, var, f2));
    case formula_kind_t::Top:
    case formula_kind_t::Bot:
        return f1;
    }
}

std::string to_debug_string(domain_kind_t kind) {
    switch (kind) {
    case domain_kind_t::Int:
        return "Int";
    case domain_kind_t::Bool:
        return "Bool";
    }
}

std::string to_debug_string(term_t const& term) {
    switch (term.kind()) {
    case term_kind_t::Var:
        return cast<var_term_t>(term).name;
    case term_kind_t::Int:
        return std::to_string(cast<int_term_t>(term).n);
    case term_kind_t::Add:
        return "Add(" +
            to_debug_string(*cast<add_term_t>(term).lhs) + ", " +
            to_debug_string(*cast<add_term_t>(term).rhs) + ")";
    case term_kind_t::Sub:
        return "Sub(" +
            to_debug_string(*cast<sub_term_t>(term).lhs) + ", " +
            to_debug_string(*cast<sub_term_t>(term).rhs) + ")";
    case term_kind_t::Mul:
        return "Mul(" +
            to_debug_string(*cast<mul_term_t>(term).lhs) + ", " +
            to_debug_string(*cast<mul_term_t>(term).rhs) + ")";
    case term_kind_t::Div:
        return "Div(" +
            to_debug_string(*cast<div_term_t>(term).lhs) + ", " +
            to_debug_string(*cast<div_term_t>(term).rhs) + ")";
    case term_kind_t::Prob:
        return "Prob(" + to_debug_string(*cast<prob_term_t>(term).inner) + ")";
    }
}

std::string to_debug_string(formula_t const& formula) {
    switch (formula.kind()) {
    case formula_kind_t::Var:
        return cast<var_formula_t>(formula).name;
    case formula_kind_t::Bot:
        return "Bot";
    case formula_kind_t::Top:
        return "Top";
    case formula_kind_t::Neg:
        return "Not(" + to_debug_string(*cast<neg_formula_t>(formula).inner) + ")";
    case formula_kind_t::And:
        return "And(" +
           to_debug_string(*cast<and_formula_t>(formula).lhs) + ", " +
           to_debug_string(*cast<and_formula_t>(formula).rhs) + ")";
    case formula_kind_t::Or:
        return "Or(" +
           to_debug_string(*cast<or_formula_t>(formula).lhs) + ", " +
           to_debug_string(*cast<or_formula_t>(formula).rhs) + ")";
    case formula_kind_t::Impl:
        return "Impl(" +
           to_debug_string(*cast<impl_formula_t>(formula).lhs) + ", " +
           to_debug_string(*cast<impl_formula_t>(formula).rhs) + ")";
    case formula_kind_t::Eq:
        return "Eq(" +
           to_debug_string(*cast<eq_formula_t>(formula).lhs) + ", " +
           to_debug_string(*cast<eq_formula_t>(formula).rhs) + ")";
    case formula_kind_t::Lt:
        return "Lt(" +
           to_debug_string(*cast<less_formula_t>(formula).lhs) + ", " +
           to_debug_string(*cast<less_formula_t>(formula).rhs) + ")";
    case formula_kind_t::Leq:
        return "Leq(" +
           to_debug_string(*cast<leq_formula_t>(formula).lhs) + ", " +
           to_debug_string(*cast<leq_formula_t>(formula).rhs) + ")";
    case formula_kind_t::Geq:
        return "Geq(" +
           to_debug_string(*cast<geq_formula_t>(formula).lhs) + ", " +
           to_debug_string(*cast<geq_formula_t>(formula).rhs) + ")";
    case formula_kind_t::Gt:
        return "Gt(" +
           to_debug_string(*cast<greater_formula_t>(formula).lhs) + ", " +
           to_debug_string(*cast<greater_formula_t>(formula).rhs) + ")";
    }
}

std::string to_debug_string(predicate_t const& pred) {
    return "Pred(" +
        pred.arg_name + ", " +
        to_debug_string(pred.arg_domain) + ", " +
        to_debug_string(*pred.body) + ")";
}

std::string output(term_t const& term, int accept, bool pos) {
    switch (term.kind()) {
    case term_kind_t::Var:
        return cast<var_term_t>(term).name;
    case term_kind_t::Int:
        return std::to_string(cast<int_term_t>(term).n);
    case term_kind_t::Add:
        return "(" +
            output(*cast<add_term_t>(term).lhs, accept, pos) + "+" +
            output(*cast<add_term_t>(term).rhs, accept, pos) + ")";
    case term_kind_t::Sub:
        return "(" +
            output(*cast<sub_term_t>(term).lhs, accept, pos) + "-" +
            output(*cast<sub_term_t>(term).rhs, accept, pos) + ")";
    case term_kind_t::Mul:
        return "(" +
            output(*cast<mul_term_t>(term).lhs, accept, pos) + "*" +
            output(*cast<mul_term_t>(term).rhs, accept, pos) + ")";
    case term_kind_t::Div:
        return "(" +
            output(*cast<div_term_t>(term).lhs, accept, pos) + "/" +
            output(*cast<div_term_t>(term).rhs, accept, pos) + ")";
    case term_kind_t::Prob: {
        std::string pmin =
            format("Pmin=? [F location={} & {}]",
                accept,
                output(*cast<prob_term_t>(term).inner, accept, pos));
        std::string pmax =
            format("Pmax=? [F location={} & {}]",
                accept,
                output(*cast<prob_term_t>(term).inner, accept, pos));
        if (pos)
            return pmin;
        else
            return pmax;
        }
    }
}

std::string output(formula_t const& f, int accept, bool pos) {
    switch (f.kind()) {
    case formula_kind_t::Var:
        return cast<var_formula_t>(f).name;
    case formula_kind_t::Bot:
        return "(1=2)";
    case formula_kind_t::Top:
        return "(1=1)";
    case formula_kind_t::Neg:
        return "!("  + output(*cast<neg_formula_t>(f).inner, accept, pos) + ")";
    case formula_kind_t::And:
        return "(" +
            output(*cast<and_formula_t>(f).lhs, accept, pos) + "&" +
            output(*cast<and_formula_t>(f).rhs, accept, pos) + ")";
        break;
    case formula_kind_t::Or:
        return "(" +
            output(*cast<or_formula_t>(f).lhs, accept, pos) + "|" +
            output(*cast<or_formula_t>(f).rhs, accept, pos) + ")";
    case formula_kind_t::Impl:
        return "(" +
            output(*cast<impl_formula_t>(f).lhs, accept, !pos) + "=>" +
            output(*cast<impl_formula_t>(f).rhs, accept, pos) + ")";
    case formula_kind_t::Eq:
        return "(" +
            output(*cast<eq_formula_t>(f).lhs, accept, pos) + "=" +
            output(*cast<eq_formula_t>(f).rhs, accept, pos) + ")";
    case formula_kind_t::Lt:
        return "(" +
            output(*cast<less_formula_t>(f).lhs, accept, !pos) + "<" +
            output(*cast<less_formula_t>(f).rhs, accept, pos) + ")";
    case formula_kind_t::Leq:
        return "(" +
            output(*cast<leq_formula_t>(f).lhs, accept, !pos) + "<=" +
            output(*cast<leq_formula_t>(f).rhs, accept, pos) + ")";
    case formula_kind_t::Geq:
        return "(" +
            output(*cast<geq_formula_t>(f).lhs, accept, pos) + ">=" +
            output(*cast<geq_formula_t>(f).rhs, accept, !pos) + ")";
    case formula_kind_t::Gt:
        return "(" +
            output(*cast<greater_formula_t>(f).lhs, accept, pos) + ">" +
            output(*cast<greater_formula_t>(f).rhs, accept, !pos) + ")";
    }
}

bool operator==(term_t const& lhs, term_t const& rhs) {
    if (lhs.kind() != rhs.kind())
        return false;
    switch (lhs.kind()) {
    case term_kind_t::Var:
        return cast<var_term_t>(lhs).name == cast<var_term_t>(rhs).name;
    case term_kind_t::Int:
        return cast<int_term_t>(lhs).n == cast<int_term_t>(rhs).n;
    case term_kind_t::Add:
        return
            *cast<add_term_t>(lhs).lhs == *cast<add_term_t>(rhs).lhs &&
            *cast<add_term_t>(lhs).rhs == *cast<add_term_t>(rhs).rhs;
    case term_kind_t::Sub:
        return
            *cast<sub_term_t>(lhs).lhs == *cast<sub_term_t>(rhs).lhs &&
            *cast<sub_term_t>(lhs).rhs == *cast<sub_term_t>(rhs).rhs;
    case term_kind_t::Mul:
        return
            *cast<mul_term_t>(lhs).lhs == *cast<mul_term_t>(rhs).lhs &&
            *cast<mul_term_t>(lhs).rhs == *cast<mul_term_t>(rhs).rhs;
    case term_kind_t::Div:
        return
            *cast<div_term_t>(lhs).lhs == *cast<div_term_t>(rhs).lhs &&
            *cast<div_term_t>(lhs).rhs == *cast<div_term_t>(rhs).rhs;
    case term_kind_t::Prob:
        return
            *cast<prob_term_t>(lhs).inner == *cast<prob_term_t>(rhs).inner;
    }
}

bool operator==(formula_t const& lhs, formula_t const& rhs) {
    if (lhs.kind() != rhs.kind())
        return false;
    switch (lhs.kind()) {
    case formula_kind_t::Var:
        return cast<var_formula_t>(lhs).name == cast<var_formula_t>(rhs).name;
    case formula_kind_t::Bot:
    case formula_kind_t::Top:
        return true;
    case formula_kind_t::Neg:
        return *cast<neg_formula_t>(lhs).inner == *cast<neg_formula_t>(rhs).inner;
    case formula_kind_t::And:
        return *cast<and_formula_t>(lhs).lhs == *cast<and_formula_t>(rhs).lhs &&
            *cast<and_formula_t>(lhs).rhs == *cast<and_formula_t>(rhs).rhs;
    case formula_kind_t::Or:
        return *cast<or_formula_t>(lhs).lhs == *cast<or_formula_t>(rhs).lhs &&
            *cast<or_formula_t>(lhs).rhs == *cast<or_formula_t>(rhs).rhs;
    case formula_kind_t::Impl:
        return *cast<impl_formula_t>(lhs).lhs == *cast<impl_formula_t>(rhs).lhs &&
            *cast<impl_formula_t>(lhs).rhs == *cast<impl_formula_t>(rhs).rhs;
    case formula_kind_t::Eq:
        return *cast<eq_formula_t>(lhs).lhs == *cast<eq_formula_t>(rhs).lhs &&
            *cast<eq_formula_t>(lhs).rhs == *cast<eq_formula_t>(rhs).rhs;
    case formula_kind_t::Lt:
        return *cast<less_formula_t>(lhs).lhs == *cast<less_formula_t>(rhs).lhs &&
            *cast<less_formula_t>(lhs).rhs == *cast<less_formula_t>(rhs).rhs;
    case formula_kind_t::Leq:
        return *cast<leq_formula_t>(lhs).lhs == *cast<leq_formula_t>(rhs).lhs &&
            *cast<leq_formula_t>(lhs).rhs == *cast<leq_formula_t>(rhs).rhs;
    case formula_kind_t::Geq:
        return *cast<geq_formula_t>(lhs).lhs == *cast<geq_formula_t>(rhs).lhs &&
            *cast<geq_formula_t>(lhs).rhs == *cast<geq_formula_t>(rhs).rhs;
    case formula_kind_t::Gt:
        return *cast<greater_formula_t>(lhs).lhs == *cast<greater_formula_t>(rhs).lhs &&
            *cast<greater_formula_t>(lhs).rhs == *cast<greater_formula_t>(rhs).rhs;
    }
    return true;
}

}

