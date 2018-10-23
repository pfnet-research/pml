#ifndef PML_TEST_HPP
#define PML_TEST_HPP

#include <exception>
#include <iostream>
#include <sstream>

namespace test {

struct test_fail_exception : public std::exception {
    explicit test_fail_exception(std::string const& msg) :
        message{msg}
    {}
    std::string message;
    char const* what() const noexcept override {
        return message.c_str();
    }
};

struct test_base {
    virtual ~test_base() = default;
    virtual std::string name() const = 0;
    int assert_count = 0;

    template<typename T1, typename T2>
    void assert_eq(T1 const& lhs, T2 const& rhs) {
        assert_count++;
        if (lhs == rhs)
            return;
        std::stringstream ss;
        ss << "\033[31mtest[" << name() <<  "] fail.. \033[39m ";
        ss << "assertion: " << lhs << " == " << rhs << std::endl;
        throw test_fail_exception{ss.str()};
    };

    void assert_(bool is_ok, std::string const& msg = "") {
        assert_count++;
        if (is_ok)
            return;
        std::stringstream ss;
        ss << "\033[31mtest[" << name() <<  "] fail.. \033[39m ";
        ss << "assertion: " << msg << std::endl;
        throw test_fail_exception{ss.str()};
    }
};

void run(int argc, const char* argv[]);

#define PML_TEST(X) \
    struct X : public test::test_base {  \
        explicit X(); \
        std::string name() const override { \
            return #X; \
        } \
        ~X() { \
            std::cerr << "\033[33m[passed " << name() << ": number of assertions: " << assert_count <<  "]\033[39m" << std::endl; \
        } \
    }; \
    inline X::X()

#define PML_CUSTOM_TEST(X, BASE) \
    struct X : public BASE {  \
        explicit X(); \
        std::string name() const override { \
            return #X; \
        } \
        ~X() { \
            std::cerr << "\033[33m[passed " << name() << ":" << assert_count <<  "]\033[39m" << std::endl; \
        } \
    }; \
    inline X::X()

}

#endif

