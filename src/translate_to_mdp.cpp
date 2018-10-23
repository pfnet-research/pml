#include "utility.hpp"
#include "environment.hpp"
#include "translate.hpp"
#include "MDP.hpp"
#include "expr_ast.hpp"

using mdp_t = mdp::mdp_t;

using var_env_t = environment_t<value_info_t>;

int translation_data::location_count = 0;
int translation_data::var_count = 0;

mdp_with_info_t trans_impl(ast::expr_t const&, var_env_t const&);

mdp_with_info_t create_rand_case(int start, int end) {
    int from = translation_data::fresh_location();
    int to = translation_data::fresh_location();
    auto rand_var = translation_data::fresh_var();

    mdp::command_t command {
        make<mdp::binop_expr_t>(
                make<mdp::var_expr_t>("location"),
                make<mdp::int_expr_t>(from),
                mdp::binop_kind_t::Eq),
        {}
    };

    auto prob = make<mdp::binop_expr_t>(
            make<mdp::int_expr_t>(1),
            make<mdp::int_expr_t>(end - start + 1),
            mdp::binop_kind_t::Div);
    auto next = make<mdp::binop_expr_t>(
            make<mdp::var_expr_t>("location'"),
            make<mdp::int_expr_t>(to),
            mdp::binop_kind_t::Eq);
    for (int i=start; i<=end; ++i) {
        auto branch = make<mdp::binop_expr_t>(
                next,
                make<mdp::binop_expr_t>(
                    make<mdp::var_expr_t>(rand_var + "'"),
                    make<mdp::int_expr_t>(i),
                    mdp::binop_kind_t::Eq),
                mdp::binop_kind_t::And);
        command.branches.push_back(mdp::branch_t {prob, branch});
    }

    mdp::mdp_t mdp {
        "default",
        {
            mdp::variable_t {
                "location",
                bound_t{from, to}, from
            },
            mdp::variable_t {
                rand_var,
                bound_t{start, end}, start
            }
        },
        {}, // constants
        {command}
    };

    return mdp_with_info_t {
        mdp, from, to,
        value_info_t {
            rand_var, {bound_t{start, end}}
        }
    };
}

mdp::command_t make_concat(int from, int to) {
    return mdp::command_t {
        make<mdp::binop_expr_t>(
                make<mdp::var_expr_t>("location"),
                make<mdp::int_expr_t>(from),
                mdp::binop_kind_t::Eq),
        {
            mdp::branch_t{
                make<mdp::int_expr_t>(1),
                make<mdp::binop_expr_t>(
                        make<mdp::var_expr_t>("location'"),
                        make<mdp::int_expr_t>(to),
                        mdp::binop_kind_t::Eq)
            }
        }
    };
}

mdp::command_t make_concat(int from, int to, ptr<mdp::expr_t> const& update) {
    return mdp::command_t {
        make<mdp::binop_expr_t>(
                make<mdp::var_expr_t>("location"),
                make<mdp::int_expr_t>(from),
                mdp::binop_kind_t::Eq),
        {
            mdp::branch_t{
                make<mdp::int_expr_t>(1),
                make<mdp::binop_expr_t>(
                        make<mdp::binop_expr_t>(
                            make<mdp::var_expr_t>("location'"),
                            make<mdp::int_expr_t>(to),
                            mdp::binop_kind_t::Eq),
                        update,
                        mdp::binop_kind_t::And)
            }
        }
    };
}

mdp::command_t make_concat_with_cond(int from, int to, ptr<mdp::expr_t> const& update) {
    return mdp::command_t {
        make<mdp::binop_expr_t>(
                make<mdp::binop_expr_t>(
                    make<mdp::var_expr_t>("location"),
                    make<mdp::int_expr_t>(from),
                    mdp::binop_kind_t::Eq),
                update,
                mdp::binop_kind_t::And),
        {
            mdp::branch_t {
                make<mdp::int_expr_t>(1),
                make<mdp::binop_expr_t>(
                        make<mdp::var_expr_t>("location'"),
                        make<mdp::int_expr_t>(to),
                        mdp::binop_kind_t::Eq)
            }
        }
    };
}

mdp_with_info_t create_let_case(
        std::string const& name,
        ast::expr_t const& init,
        ast::expr_t const& body,
        var_env_t const& var_env) {

    // TODO: resolve name conflict case
    // e.g) let a = 1 in let a = 3 in ...

    auto init_ = trans_impl(init, var_env);
    auto new_env = var_env.append(name, make<value_info_t>(init_.value));
    auto body_ = trans_impl(body, new_env);

    // concat accept location of init to init location of body,
    // and subst value of init to variable `name`.
    //
    // [] location=accept-of-init -> 1:(location'=init-of-body)&(name'=value_name-of-init)
    auto concat = make_concat(
            init_.accept, body_.init,
            make<mdp::binop_expr_t>(
                make<mdp::var_expr_t>(name+"'"),
                make<mdp::var_expr_t>(init_.value.name),
                mdp::binop_kind_t::Eq));

    auto result_mdp = mdp::mdp_t::merge(std::move(init_.mdp), std::move(body_.mdp));
    result_mdp.commands.push_back(concat);

    if (init_.value.bound) { // integer
        result_mdp.variables.push_back(mdp::variable_t {
                name, *init_.value.bound, 0
                });
    } else { // boolean
        result_mdp.variables.push_back(mdp::variable_t {
                name, true
                });
    }

    return mdp_with_info_t {
        result_mdp, init_.init, body_.accept,
        body_.value
    };
}

mdp_with_info_t create_if_case(
        ast::expr_t const& cond,
        ast::expr_t const& tr, ast::expr_t const& fl,
        var_env_t const& var_env) {

    auto cond_ = trans_impl(cond, var_env);
    auto tr_ = trans_impl(tr, var_env);
    auto fl_ = trans_impl(fl, var_env);

    auto result_mdp = mdp::mdp_t::merge(
            mdp::mdp_t::merge(std::move(cond_.mdp), std::move(tr_.mdp)),
            std::move(fl_.mdp));

    // [] location=accept-of-cond & cond -> 1:location'=init-of-tr
    auto concat_to_true = make_concat_with_cond(
            cond_.accept, tr_.init,
            make<mdp::var_expr_t>(cond_.value.name));
    // [] location=accept-of-cond & !cond -> 1:location'=init-of-fl
    auto concat_to_false = make_concat(
            cond_.accept, fl_.init,
            make<mdp::neg_expr_t>(make<mdp::var_expr_t>(cond_.value.name)));

    auto accept_loc = translation_data::fresh_location();
    auto result_var_name = translation_data::fresh_var();
    // [] location=accept-of-tr -> 1:location'=accept & value_name'=value_name-of-tr
    auto phi_true = make_concat(
            tr_.accept, accept_loc,
            make<mdp::binop_expr_t>(
                make<mdp::var_expr_t>(result_var_name + "'"),
                make<mdp::var_expr_t>(tr_.value.name),
                mdp::binop_kind_t::Eq));
    // [] location=accept-of-fl -> 1:location'=accept & value_name'=value_name-of-fl
    auto phi_false = make_concat(
            fl_.accept, accept_loc,
            make<mdp::binop_expr_t>(
                make<mdp::var_expr_t>(result_var_name+"'"),
                make<mdp::var_expr_t>(fl_.value.name),
                mdp::binop_kind_t::Eq));

    result_mdp.commands.push_back(concat_to_true);
    result_mdp.commands.push_back(concat_to_false);
    result_mdp.commands.push_back(phi_true);
    result_mdp.commands.push_back(phi_false);

    if (!tr_.value.bound != !fl_.value.bound)
        throw std::logic_error{"invalid if expression"};

    auto result_var = tr_.value.bound ?
        mdp::variable_t{result_var_name, *tr_.value.bound | *fl_.value.bound, 0} :
        mdp::variable_t{result_var_name, true};

    result_mdp.variables.push_back(result_var);

    return mdp_with_info_t {
        result_mdp, cond_.init, accept_loc,
        value_info_t {
            result_var_name,
            result_var.is_int() ?
                util::make_optional(result_var.as_int().bound) :
                util::nullopt
        }
    };
}

value_info_t calc_binop_bound(
        value_info_t const& lhs,
        value_info_t const& rhs,
        ast::expr_kind_t op) {
    switch (op) {
    case ast::expr_kind_t::Add:
        return value_info_t {
            format("({}+{})", lhs.name, rhs.name),
            *lhs.bound + *rhs.bound
        };
    case ast::expr_kind_t::Sub:
        return value_info_t {
            format("({}-{})", lhs.name, rhs.name),
            *lhs.bound - *rhs.bound
        };
    case ast::expr_kind_t::Mul:
        return value_info_t {
            format("({}*{})", lhs.name, rhs.name),
            *lhs.bound * *rhs.bound
        };
    case ast::expr_kind_t::Div:
        return value_info_t {
            format("({}/{})", lhs.name, rhs.name),
            *lhs.bound / *rhs.bound
        };
    case ast::expr_kind_t::Eq:
        return value_info_t {
            format("{}={}", lhs.name, rhs.name),
            util::nullopt
        };
    case ast::expr_kind_t::Neq:
        return value_info_t {
            format("({}!={})", lhs.name, rhs.name),
            util::nullopt
        };
    case ast::expr_kind_t::Leq:
        return value_info_t {
            format("({}<={})", lhs.name, rhs.name),
            util::nullopt
        };
    case ast::expr_kind_t::Geq:
        return value_info_t {
            format("({}>={})", lhs.name, rhs.name),
            util::nullopt
        };
    case ast::expr_kind_t::And:
        return value_info_t {
            format("({}&{})", lhs.name, rhs.name),
            util::nullopt
        };
    case ast::expr_kind_t::Or:
        return value_info_t {
            format("({}|{})", lhs.name, rhs.name),
            util::nullopt
        };
    default:
        throw std::logic_error{"invalid binop"};
    }
}

mdp_with_info_t create_binop_case(
        ast::expr_t const& lhs_expr,
        ast::expr_t const& rhs_expr,
        ast::expr_kind_t op,
        var_env_t const& var_env) {
    auto lhs_ = trans_impl(lhs_expr, var_env);
    auto const& lhs = lhs_.value;
    auto rhs_ = trans_impl(rhs_expr, var_env);
    auto const& rhs = rhs_.value;

    auto result_mdp = mdp::mdp_t::merge(
            std::move(lhs_.mdp),
            std::move(rhs_.mdp));

    auto result_bound = calc_binop_bound(lhs, rhs, op);

    return mdp_with_info_t {
        result_mdp, lhs_.init, rhs_.accept,
        result_bound
    };
}

mdp_with_info_t create_neg_case(ast::expr_t const& inner, var_env_t const& var_env) {
    auto inner_ = trans_impl(inner, var_env);
    auto accept_loc = translation_data::fresh_location();
    auto result_var_name = translation_data::fresh_var();

    auto result_var = mdp::variable_t{result_var_name, true};

    // [] location=accept-of-inner -> 1:location'=accept_loc&result_var_name'=!inner
    auto concat = make_concat(
            inner_.accept, accept_loc,
            make<mdp::binop_expr_t>(
                make<mdp::var_expr_t>(result_var_name+"'"),
                make<mdp::neg_expr_t>(
                    make<mdp::var_expr_t>(inner_.value.name)),
                mdp::binop_kind_t::Eq));

    inner_.mdp.commands.push_back(concat);
    inner_.mdp.variables.push_back(result_var);

    return mdp_with_info_t {
        inner_.mdp, inner_.init, accept_loc,
        value_info_t {
            result_var_name, util::nullopt
        }
    };
}

mdp_with_info_t create_int_case(int n) {
    auto const_name = "c" + std::to_string(n);
    int current = translation_data::current_location();
    return mdp_with_info_t {
        mdp::mdp_t {
            "default",
            {},
            { mdp::constant_t{const_name, n} },
            {}
        },
        current, current,
        value_info_t {
            const_name, {bound_t{n, n}}
        }
    };
}

mdp_with_info_t create_bool_case(bool b) {
    auto const_name = "c" + std::to_string(b);
    int current = translation_data::current_location();
    return mdp_with_info_t {
        mdp::mdp_t {
            "default",
            {},
            { mdp::constant_t{const_name, b} },
            {}
        },
        current, current,
        value_info_t {
            const_name, util::nullopt
        }
    };
}

mdp_with_info_t create_var_case(std::string const& name, var_env_t const& var_env) {
    auto result_var = *var_env.lookup(name);
    result_var.name = name;
    int current = translation_data::current_location();
    return mdp_with_info_t {
        mdp::mdp_t {
            "default", {}, {}, {}
        },
        current, current,
        result_var
    };
}

mdp_with_info_t trans_impl(ast::expr_t const& e, var_env_t const& var_env) {
    using namespace ast;
    switch (e.kind()) {
    case expr_kind_t::Let:
        return create_let_case(
                cast<ast::let_expr_t>(e).name,
                *cast<ast::let_expr_t>(e).init,
                *cast<ast::let_expr_t>(e).body,
                var_env);
    case expr_kind_t::If:
        return create_if_case(
                *cast<if_expr_t>(e).cond_expr,
                *cast<if_expr_t>(e).true_expr,
                *cast<if_expr_t>(e).false_expr,
                var_env);
    case expr_kind_t::Rand:
        return create_rand_case(
                cast<rand_expr_t>(e).start,
                cast<rand_expr_t>(e).end);
    case expr_kind_t::Add: case expr_kind_t::Sub:
    case expr_kind_t::Mul: case expr_kind_t::Div:
    case expr_kind_t::Eq:  case expr_kind_t::Neq:
    case expr_kind_t::Leq: case expr_kind_t::Geq:
    case expr_kind_t::And: case expr_kind_t::Or:
        return create_binop_case(
                *cast<ast::binop_expr_t>(e).lhs,
                *cast<ast::binop_expr_t>(e).rhs,
                e.kind(), var_env);
    case expr_kind_t::Neg:
        return create_neg_case(*cast<ast::neg_expr_t>(e).inner, var_env);
    case expr_kind_t::Int:
        return create_int_case(cast<ast::int_expr_t>(e).n);
    case expr_kind_t::Bool:
        return create_bool_case(cast<ast::bool_expr_t>(e).b);
    case expr_kind_t::Var:
        return create_var_case(cast<ast::var_expr_t>(e).name, var_env);
    case expr_kind_t::Typed:
        return trans_impl(*cast<typed_expr_t>(e).expr, var_env);
    default:
        throw std::logic_error{"unimplemented translation"};
    }
}

mdp_with_info_t translate_to_mdp(ast::expr_t const& e) {
    translation_data::init();
    auto mdp_with_info = trans_impl(e, var_env_t{});
    for (auto&& var : mdp_with_info.mdp.variables) {
        if (var.name == "location") {
            var = mdp::variable_t{
                "location",
                bound_t{0, mdp_with_info.accept}, 0};
        }
    }
    return mdp_with_info;
}


