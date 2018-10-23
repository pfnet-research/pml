#include "test.hpp"
#include "expr_ast.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "MDP.hpp"
#include "simple_type.hpp"
#include "translate.hpp"
#include "typechecker.hpp"

struct lang_feature_test : public test::test_base {
    void parse_test(std::string const& input, std::string const& output) {
        parser::parse(input).case_of(
            ok >> [&](ptr<ast::expr_t> const& ast) {
                assert_(
                    ast != nullptr,
                    "\"" + input + "\" was parsed to nullptr");
                assert_eq(ast::to_debug_string(*ast), output);
            },
            error >> [this](parser::error_t err) {
                std::string msg = "parse error at" +
                    std::to_string(err.pos) + " : " +
                    err.message;
                assert_(false, msg);
            });
    }

    void eval_test(ptr<ast::expr_t> input, ptr<ast::expr_t> const& output) {
        assert_(input != nullptr && output != nullptr, "input or output was nullptr");
        assert_eq(*evaluator::eval(input), *output);
    }
};

PML_CUSTOM_TEST(arith_test, lang_feature_test) {
    using namespace ast;
    parse_test("1 +(2+3*4) - 3", "Sub(Add(1, Add(2, Mul(3, 4))), 3)");
    eval_test(
            make<sub_expr_t>(
                make<add_expr_t>(
                    make<int_expr_t>(1),
                    make<add_expr_t>(
                        make<int_expr_t>(2),
                        make<mul_expr_t>(
                            make<int_expr_t>(3),
                            make<int_expr_t>(4)))),
                make<int_expr_t>(3)),
            make<int_expr_t>(12));
    parse_test("1-(-3)", "Sub(1, -3)");
    eval_test(
            make<sub_expr_t>(
                make<int_expr_t>(1),
                make<int_expr_t>(-3)),
            make<int_expr_t>(4));
}

PML_CUSTOM_TEST(formula_test, lang_feature_test) {
    using namespace ast;
    parse_test("1+2 == 4", "Eq(Add(1, 2), 4)");
    parse_test("1+2 != 4", "Neq(Add(1, 2), 4)");
    parse_test("1+2 <= 4", "Leq(Add(1, 2), 4)");
    parse_test("1+2 >= 4", "Geq(Add(1, 2), 4)");
    eval_test(
            make<eq_expr_t>(
                make<add_expr_t>(
                    make<int_expr_t>(1),
                    make<int_expr_t>(2)),
                make<int_expr_t>(4)),
            make<bool_expr_t>(false));
    eval_test(
            make<neq_expr_t>(
                make<add_expr_t>(
                    make<int_expr_t>(1),
                    make<int_expr_t>(2)),
                make<int_expr_t>(4)),
            make<bool_expr_t>(true));
    eval_test(
            make<leq_expr_t>(
                make<add_expr_t>(
                    make<int_expr_t>(1),
                    make<int_expr_t>(2)),
                make<int_expr_t>(4)),
            make<bool_expr_t>(true));
    eval_test(
            make<geq_expr_t>(
                make<add_expr_t>(
                    make<int_expr_t>(1),
                    make<int_expr_t>(2)),
                make<int_expr_t>(4)),
            make<bool_expr_t>(false));
}

PML_CUSTOM_TEST(let_test, lang_feature_test) {
    using namespace ast;
    parse_test("let a = 1+2 in a + 3", "Let(a, Add(1, 2), Add(a, 3))");
    eval_test(
            make<let_expr_t>("a",
                make<add_expr_t>(
                    make<int_expr_t>(1),
                    make<int_expr_t>(2)),
                make<add_expr_t>(
                    make<var_expr_t>("a"),
                    make<int_expr_t>(3))),
            make<int_expr_t>(6));
}

PML_CUSTOM_TEST(letfun_test, lang_feature_test) {
    using namespace ast;
    parse_test("letfun plus4 (a:int, b:int) -> int = a+4 in plus4 (1+3) 0", "LetFun(plus4, [a, b], Add(a, 4), App(plus4, [Add(1, 3), 0]))");
    eval_test(
            make<letfun_expr_t>("plus4",
                dependent_type_t {
                    {
                        refinement_type_t{"a",
                            logic::domain_kind_t::Int,
                            make<logic::top_formula_t>()},
                        refinement_type_t{"b",
                            logic::domain_kind_t::Int,
                            make<logic::top_formula_t>()}
                    },
                    refinement_type_t {"@blah",
                        logic::domain_kind_t::Int,
                        make<logic::top_formula_t>()
                    }
                },
                make<add_expr_t>(
                    make<var_expr_t>("a"),
                    make<int_expr_t>(4)),
                make<app_expr_t>(
                    make<var_expr_t>("plus4"),
                    std::vector<ptr<expr_t>>{
                        make<add_expr_t>(
                                make<int_expr_t>(1),
                                make<int_expr_t>(3)),
                        make<int_expr_t>(0)
                        })),
            make<int_expr_t>(8));
}

PML_CUSTOM_TEST(if_test, lang_feature_test) {
    using namespace ast;
    parse_test("if true then 42 else 1", "If(true, 42, 1)");
    eval_test(
            make<if_expr_t>(
                make<bool_expr_t>(true),
                make<int_expr_t>(42),
                make<int_expr_t>(1)),
            make<int_expr_t>(42));

    parse_test("let x = false in if x then 31 else 21", "Let(x, false, If(x, 31, 21))");
    eval_test(
            make<let_expr_t>("x",
                make<bool_expr_t>(false),
                make<if_expr_t>(
                    make<var_expr_t>("x"),
                    make<int_expr_t>(31),
                    make<int_expr_t>(21))),
            make<int_expr_t>(21));
}

PML_CUSTOM_TEST(rand_test, lang_feature_test) {
    using namespace ast;
    using namespace evaluator;
    parse_test("rand(1, 4)", "Rand(1, 4)");

    for (int i=0; i<16; ++i) {
        auto n1 = cast<int_expr_t>(*eval(make<rand_expr_t>(1, 4))).n;
        assert_(1 <= n1 && n1 <= 4, "1 <= rand(1, 4) <= 4");
    }

    parse_test("rand(1, 4) + 42", "Add(Rand(1, 4), 42)");
    auto n2 = cast<int_expr_t>(*eval(
                make<add_expr_t>(
                    make<rand_expr_t>(1, 4),
                    make<int_expr_t>(42)))).n;
    assert_(43 <= n2 && n2 <= 46, "43 <= rand(1, 4) + 42 <= 46");
}

PML_CUSTOM_TEST(typed_test, lang_feature_test) {
    using namespace ast;
    using namespace evaluator;
    parse_test("1:{x:int | true}", "Typed(1, Ref(x, Int, Top))");
}

PML_TEST(translation_test) {
    using namespace ast;
    using namespace mdp;
    assert_eq(
            translate_to_mdp(ast::int_expr_t{42}).mdp,
            mdp_t {
                "default",
                {},
                {
                    mdp::constant_t{"c42", 42}
                },
                {}
            });
}

PML_TEST(translation_rand_test) {
    using namespace ast;
    using namespace mdp;
    assert_eq(
            translate_to_mdp(ast::rand_expr_t{1, 2}).mdp,
            mdp_t {
                "default",
                {
                    mdp::variable_t{"location", bound_t{0, 1}, 0},
                    mdp::variable_t{"v0", bound_t{1, 2}, 1}
                },
                {}, // constants
                {
                    command_t {
                        make<mdp::binop_expr_t>(
                            make<mdp::var_expr_t>("location"),
                            make<mdp::int_expr_t>(0),
                            mdp::binop_kind_t::Eq),
                        {
                            branch_t {
                                make<mdp::binop_expr_t>(
                                    make<mdp::int_expr_t>(1),
                                    make<mdp::int_expr_t>(2),
                                    mdp::binop_kind_t::Div),
                                make<mdp::binop_expr_t>(
                                    make<mdp::binop_expr_t>(
                                        make<mdp::var_expr_t>("location'"),
                                        make<mdp::int_expr_t>(1),
                                        mdp::binop_kind_t::Eq),
                                    make<mdp::binop_expr_t>(
                                        make<mdp::var_expr_t>("v0'"),
                                        make<mdp::int_expr_t>(1),
                                        mdp::binop_kind_t::Eq),
                                    mdp::binop_kind_t::And)
                            },
                            branch_t {
                                make<mdp::binop_expr_t>(
                                    make<mdp::int_expr_t>(1),
                                    make<mdp::int_expr_t>(2),
                                    mdp::binop_kind_t::Div),
                                make<mdp::binop_expr_t>(
                                    make<mdp::binop_expr_t>(
                                        make<mdp::var_expr_t>("location'"),
                                        make<mdp::int_expr_t>(1),
                                        mdp::binop_kind_t::Eq),
                                    make<mdp::binop_expr_t>(
                                        make<mdp::var_expr_t>("v0'"),
                                        make<mdp::int_expr_t>(2),
                                        mdp::binop_kind_t::Eq),
                                    mdp::binop_kind_t::And)
                            }
                        }
                    }
                }
            });
}

PML_TEST(parsing_formula_test) {
    std::string input = "true \\/ true /\\ false";
    parser::parse_formula(input).case_of(
        ok >> [&](ptr<logic::formula_t> const& formula) {
            assert_(formula != nullptr, "\"" + input + "\" was parsed to nullptr");
            assert_eq(
                    logic::to_debug_string(*formula),
                    "Or(Top, And(Top, Bot))");
        },
        error >> [this](parser::error_t err) {
            std::string msg = "parse error at " +
                std::to_string(err.pos) + " : " +
                err.message;
            assert_(false, msg);
        });
}

PML_TEST(parsing_term_test) {
    std::string input = "a+b*c";
    parser::parse_term(input).case_of(
        ok >> [&](ptr<logic::term_t> const& term) {
            assert_(term != nullptr, "\"" + input + "\" was parsed to nullptr");
            assert_eq(
                    logic::to_debug_string(*term),
                    "Add(a, Mul(b, c))");
        },
        error >> [this](parser::error_t err) {
            std::string msg = "parse error at " +
                std::to_string(err.pos) + " : " +
                err.message;
            assert_(false, msg);
        });
}

PML_TEST(parsing_reftype_test) {
    std::unordered_map<std::string, std::string> inputs = {
        {"{x:int|true}", "Ref(x, Int, Top)"},
        {"{x:bool|x}", "Ref(x, Bool, x)"},
        {"int", "Ref(@blah, Int, Top)"},
        {"bool", "Ref(@blah, Bool, Top)"},
        {"x:int", "Ref(x, Int, Top)"},
        {"x:bool", "Ref(x, Bool, Top)"}
    };
    for (auto const& input : inputs) {
        parser::parse_reftype(input.first).case_of(
            ok >> [&](ast::refinement_type_t const& ty) {
                assert_eq(
                    ast::to_debug_string(ty),
                    input.second);
            },
            error >> [&](parser::error_t err) {
                assert_(false, format("parse error at {} : {}",
                        std::to_string(err.pos), err.message));
            });
    }
}
PML_TEST(parsing_deptype_test) {
    std::unordered_map<std::string, std::string> inputs = {
        {"{n:int|true} -> {x:bool|x}", "Dep([Ref(n, Int, Top)], Ref(x, Bool, x))"},
        {"(int, int) -> int", "Dep([Ref(@blah, Int, Top), Ref(@blah, Int, Top)], Ref(@blah, Int, Top))"}
    };
    for (auto const& input : inputs) {
        parser::parse_deptype(input.first).case_of(
            ok >> [&](ast::dependent_type_t const& ty) {
                assert_eq(
                    ast::to_debug_string(ty),
                    input.second);
            },
            error >> [&](parser::error_t err) {
                assert_(false, format("parse error at {} : {}",
                        std::to_string(err.pos), err.message));
            });
    }

}

PML_TEST(subst_term_test) {
    assert_eq(
            *logic::subst(
                make<logic::prob_term_t>(make<logic::var_formula_t>("x")),
                "x", make<logic::top_formula_t>()),
            logic::prob_term_t{make<logic::top_formula_t>()});
}

PML_TEST(subst_formula_test) {
    // [true/hoge]hoge == true
    assert_eq(
            *logic::subst(
                make<logic::var_formula_t>("hoge"),
                "hoge", make<logic::top_formula_t>()),
            logic::top_formula_t{});
    // [true/neko](Prob(neko) >= 2/3) == Prob(true) >= 2/3
    assert_eq(
            *logic::subst(
                make<logic::geq_formula_t>(
                    make<logic::prob_term_t>(
                        make<logic::var_formula_t>("neko")),
                    make<logic::div_term_t>(
                        make<logic::int_term_t>(2),
                        make<logic::int_term_t>(3))),
                "neko", make<logic::top_formula_t>()),
            logic::geq_formula_t(
                make<logic::prob_term_t>(
                    make<logic::top_formula_t>()),
                make<logic::div_term_t>(
                    make<logic::int_term_t>(2),
                    make<logic::int_term_t>(3))));
}

PML_TEST(simple_type_arith) {
    assert_eq(
        *simty::simple_typing(ast::int_expr_t{42}).ok(),
        simty::int_type_t{});

    // 2+3*4
    assert_eq(
        *simty::simple_typing(ast::add_expr_t {
            make<ast::int_expr_t>(2),
            make<ast::mul_expr_t>(
                make<ast::int_expr_t>(3),
                make<ast::int_expr_t>(4))}).ok(),
        simty::int_type_t{});

    // 5-4/2
    assert_eq(
        *simty::simple_typing(ast::sub_expr_t {
            make<ast::int_expr_t>(5),
            make<ast::div_expr_t>(
                make<ast::int_expr_t>(4),
                make<ast::int_expr_t>(2))}).ok(),
        simty::int_type_t{});

    // if 1=3 then 1 else 2
    assert_eq(
        *simty::simple_typing(ast::if_expr_t {
            make<ast::bool_expr_t>(true),
            make<ast::int_expr_t>(1),
            make<ast::int_expr_t>(2)}).ok(),
        simty::int_type_t{});
}

PML_TEST(simple_type_logical) {
    assert_eq(
        *simty::simple_typing(ast::neg_expr_t{
            make<ast::bool_expr_t>(true)}).ok(),
        simty::bool_type_t{});

    assert_eq(
        *simty::simple_typing(ast::eq_expr_t{
            make<ast::int_expr_t>(42),
            make<ast::int_expr_t>(32)}).ok(),
        simty::bool_type_t{});

    assert_eq(
        *simty::simple_typing(ast::leq_expr_t {
            make<ast::int_expr_t>(32),
            make<ast::int_expr_t>(12)}).ok(),
        simty::bool_type_t{});

    assert_eq(
        *simty::simple_typing(ast::and_expr_t {
            make<ast::bool_expr_t>(true),
            make<ast::bool_expr_t>(false)}).ok(),
        simty::bool_type_t{});

    assert_eq(
        *simty::simple_typing(ast::or_expr_t {
            make<ast::bool_expr_t>(true),
            make<ast::bool_expr_t>(false)}).ok(),
        simty::bool_type_t{});
}

PML_TEST(simple_type_let) {
    // let a = 12 in 42
    assert_eq(
        *simty::simple_typing(ast::let_expr_t {
            "a",
            make<ast::int_expr_t>(12),
            make<ast::int_expr_t>(42)
        }).ok(),
        simty::int_type_t{});

    // let a = 12 in a
    assert_eq(
        *simty::simple_typing(ast::let_expr_t {
            "a",
            make<ast::int_expr_t>(12),
            make<ast::var_expr_t>("a")
        }).ok(),
        simty::int_type_t{});
}

PML_TEST(simple_type_rand) {
    assert_eq(
        *simty::simple_typing(ast::rand_expr_t{1, 10}).ok(),
        simty::int_type_t{});
}

PML_TEST(typecheck_test) {
    using namespace typechecker;
    assert_(
        typecheck(ast::let_expr_t{ "a",
            make<ast::rand_expr_t>(0, 1),
            make<ast::typed_expr_t>(
                    make<ast::var_expr_t>("a"),
                    ast::refinement_type_t{
                            "x", logic::domain_kind_t::Int,
                            make<logic::eq_formula_t>(
                                    make<logic::prob_term_t>(
                                        make<logic::eq_formula_t>(
                                            make<logic::var_term_t>("x"),
                                            make<logic::int_term_t>(0))),
                                    make<logic::div_term_t>(
                                        make<logic::int_term_t>(1),
                                        make<logic::int_term_t>(2)))})
        }),
        "can not type `let a = rand(0, 1) in a : {x:int | Prob(x=0) = 1/2}`");
}

void test::run(int, const char**) {
    std::cerr << "\033[32m    <<<< language feature test >>>> \033[39m" << std::endl;
    arith_test{};
    formula_test{};
    let_test{};
    letfun_test{};
    if_test{};
    rand_test{};
    typed_test{};

    std::cerr << "\033[32m    <<<< MDP transion test >>>> \033[39m" << std::endl;
    translation_test{};
    translation_rand_test{};

    std::cerr << "\033[32m    <<<< parsing test >>>> \033[39m" << std::endl;
    parsing_formula_test{};
    parsing_term_test{};
    parsing_reftype_test{};
    parsing_deptype_test{};

    std::cerr << "\033[32m    <<<< subst test >>>> \033[39m" << std::endl;
    subst_term_test{};
    subst_formula_test{};

    std::cerr << "\033[32m    <<<< simple typing test >>>> \033[39m" << std::endl;
    simple_type_arith{};
    simple_type_logical{};
    simple_type_let{};
    simple_type_rand{};

    std::cerr << "\033[32m    <<<< typecheck test >>>> \033[39m" << std::endl;
    typecheck_test{};
}

