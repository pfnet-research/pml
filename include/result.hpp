
#ifndef PML_RESULT_HPP
#define PML_RESULT_HPP

#include <stdexcept>
#include <exception>
#include <string>
#include <ostream>

#define MAKE_ADAPTOR(X) \
    namespace detail { struct X ## _impl {}; } \
    static const detail:: X ## _impl X {}; \
    template<typename F> struct X ## _adaptor { F const& f; }; \
    template<typename F> inline static auto operator>>(detail:: X ## _impl const&, F const& f) { \
        return X ## _adaptor <F>{f}; \
    }

MAKE_ADAPTOR(ok);
MAKE_ADAPTOR(error);
#undef MAKE_ADAPTOR
template<typename T, typename E=std::string>
struct result_t {
    using ok_t = T;
    using error_t = E;

    // constructions

    static result_t ok(ok_t const& o) {
        result_t result;
        result.m_is_ok = true;
        result.assign_ok(o);
        return result;
    }
    static result_t ok(ok_t&& o) {
        result_t result;
        result.m_is_ok = true;
        result.assign_ok(std::move(o));
        return result;
    }
    static result_t error(error_t const& e) {
        result_t result;
        result.m_is_ok = false;
        result.assign_err(e);
        return result;
    }
    static result_t error(error_t&& e) {
        result_t result;
        result.m_is_ok = false;
        result.assign_err(std::move(e));
        return result;
    }
    template<typename F>
    static result_t<T, std::exception_ptr> trying(F const& f) {
        using ret_type = result_t<T, std::exception_ptr>;
        try {
            return ret_type::ok(f());
        } catch (...) {
            return ret_type::error(std::current_exception());
        }
    }

    result_t(ok_t const& ok) :
        m_is_ok{true}
    {
        assign_ok(ok);
    }
    result_t(ok_t&& ok) :
        m_is_ok{true}
    {
        assign_ok(std::move(ok));
    }

    result_t(result_t const& src) :
        m_is_ok{src.m_is_ok}
    {
        if (m_is_ok)
            assign_ok(src.m_ok);
        else
            assign_err(src.m_error);
    }
    result_t(result_t&& src) :
        m_is_ok{src.m_is_ok}
    {
        if (m_is_ok)
            assign_ok(std::move(src.m_ok));
        else
            assign_err(std::move(src.m_error));
    }

    // copy
    result_t& operator=(result_t const& rhs) {
        if (m_is_ok == rhs.m_is_ok) {
            if (m_is_ok)
                m_ok = rhs.m_ok;
            else
                m_error = rhs.m_error;
        } else {
            clear();
            m_is_ok = rhs.m_is_ok;
            if (m_is_ok)
                assign_ok(rhs.m_ok);
            else
                assign_err(rhs.m_error);
        }
        return *this;
    }

    // move
    result_t& operator=(result_t&& rhs) {
        if (m_is_ok == rhs.m_is_ok) {
            if (m_is_ok)
                m_ok = std::move(rhs.m_ok);
            else
                m_error = std::move(rhs.m_error);
        } else {
            clear();
            m_is_ok = rhs.m_is_ok;
            if (m_is_ok)
                assign_ok(std::move(rhs.m_ok));
            else
                assign_err(std::move(rhs.m_error));
        }
        return *this;
    }

    ok_t const& ok() const {
        if (!m_is_ok)
            throw std::logic_error{"invalid result_t::ok for error result"};
        return m_ok;
    }
    error_t const& error() const {
        if (m_is_ok)
            throw std::logic_error{"invalid result_t::error for ok result"};
        return m_error;
    }
    ok_t move_ok() {
        auto ret = std::move(m_ok);
        return ret;
    }

    bool is_ok() const { return m_is_ok; }
    bool is_error() const { return !m_is_ok; }

    template<typename OkTy>
    result_t<OkTy, error_t> convert() const {
        return result_t<OkTy, error_t>::error(error());
    }

    template<typename F, typename G>
    auto case_of(ok_adaptor<F> const& ok_adap, error_adaptor<G> const& err_adap) const {
        using ok_ret_type = decltype(ok_adap.f(std::declval<ok_t>()));
        using err_ret_type = decltype(err_adap.f(std::declval<error_t>()));
        static_assert(std::is_same<ok_ret_type, err_ret_type>::value, "result_t::case_of: return types are different!");
        if (m_is_ok)
            return ok_adap.f(m_ok);
        else
            return err_adap.f(m_error);
    }

    ~result_t() { clear(); }
private:
    result_t() {}

    union {
        T m_ok;
        E m_error;
    };

    void clear() {
        if (m_is_ok)
            m_ok.~T();
        else
            m_error.~E();
    }

    void assign_ok(T&& ok) {
        new (&m_ok) T{std::move(ok)};
    }
    void assign_err(E&& err) {
        new (&m_error) E{std::move(err)};
    }

    void assign_ok(T const& ok) {
        new (&m_ok) T{ok};
    }
    void assign_err(E const& err) {
        new (&m_error) E{err};
    }

    bool m_is_ok;
};

template<typename T, typename U, typename E>
inline static result_t<U, E> operator&&(result_t<T, E> const& lhs, result_t<U, E> const& rhs) {
    if (lhs.is_ok())
        return rhs;
    else
        return result_t<U, E>::error(lhs.error());
}

template<typename T, typename U, typename E>
inline static result_t<U, E> operator||(result_t<T, E> const& lhs, result_t<U, E> const& rhs) {
    if (lhs.is_ok())
        return result_t<U, E>::ok(lhs.ok());
    else
        return rhs;
}

template<typename T, typename E>
inline static bool operator==(result_t<T, E> const& lhs, result_t<T, E> const& rhs) {
    if (lhs.is_ok() != rhs.is_ok())
        return false;
    if (lhs.is_ok())
        return lhs.ok() == rhs.ok();
    else
        return rhs.error() == rhs.error();
}

template<typename T, typename E>
inline static std::ostream&
operator <<(std::ostream& os, result_t<T, E> const& res) {
    if (res.is_ok())
        os << "result_t::ok(" << res.ok() << ")";
    else
        os << "result_t::error(" << res.error() << ")";
    return os;
}

template<typename T>
inline static std::ostream&
operator <<(std::ostream& os, result_t<T, std::exception_ptr> const& res) {
    if (res.is_ok()) {
        os << "result_t::ok(" << res.ok() << ")";
        return os;
    }
    try {
        std::rethrow_exception(res.error());
    } catch (std::exception const& e) {
        os << "result_t::error(" << e.what() << ")";
    } catch (...) {
        os << "result_t::error(non standard exception)";
    }
    return os;
}

#define MAKE_ADAPTOR(X) \
    namespace detail { struct X ## _impl {}; } \
    static const detail:: X ## _impl X {}; \
    template<typename F> struct X ## _adaptor { F const& f; }; \
    template<typename F> inline static auto operator>>(detail:: X ## _impl const&, F const& f) { \
        return X ## _adaptor <F>{f}; \
    }

MAKE_ADAPTOR(map);
MAKE_ADAPTOR(map_error);
MAKE_ADAPTOR(and_then);
MAKE_ADAPTOR(or_else);
MAKE_ADAPTOR(trying);
#undef MAKE_ADAPTOR

template<typename T, typename E, typename F>
auto operator|(result_t<T, E> result, map_adaptor<F> const& adaptor) {
    using ret_type = result_t<decltype(adaptor.f(std::declval<T>())), E>;
    if (result.is_ok())
        return ret_type::ok(adaptor.f(result.ok()));
    else
        return ret_type::error(result.error());
}

template<typename T, typename E, typename F>
auto operator|(result_t<T, E> result, map_error_adaptor<F> const& adaptor) {
    using ret_type = result_t<T, decltype(adaptor.f(std::declval<E>()))>;
    if (result.is_error())
        return ret_type::error(adaptor.f(result.error()));
    else
        return ret_type::ok(result.ok());
}
template<typename T, typename E, typename F>
auto operator|(result_t<T, E> result, and_then_adaptor<F> const& adaptor) {
    using ret_type = decltype(adaptor.f(std::declval<T>()));
    if (result.is_ok())
        return adaptor.f(result.ok());
    else
        return ret_type::error(result.error());
}
template<typename T, typename E, typename F>
auto operator|(result_t<T, E> result, or_else_adaptor<F> const& adaptor) {
    using ret_type = decltype(adaptor.f(std::declval<E>()));
    if (result.is_error())
        return adaptor.f(result.error());
    else
        return ret_type::ok(result.ok());
}
template<typename T, typename E, typename F>
auto operator|(result_t<T, E> result, trying_adaptor<F> const& adaptor) {
    using ret_type = result_t<decltype(adaptor.f(std::declval<T>())), std::exception_ptr>;
    if (result.is_error())
        return ret_type::error(std::make_exception_ptr(result.error()));

    try {
        return ret_type::ok(adaptor.f(result.ok()));
    } catch (...) {
        return ret_type::error(std::current_exception());
    }
}

#endif

