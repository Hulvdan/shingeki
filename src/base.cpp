#define globalVar static
#define localPersist static

#define FOR_RANGE(type, variable_name, max_value_exclusive) \
    for (type variable_name = 0; (variable_name) < (max_value_exclusive); variable_name++)

#define INVALID_PATH Assert(false)
#define NOT_IMPLEMENTED Assert(false)
#define NOT_SUPPORTED Assert(false)

#define scast static_cast
#define rcast reinterpret_cast

#define Min(a, b) ((a) > (b) ? (b) : (a))
#define Max(a, b) ((a) > (b) ? (a) : (b))

#ifdef TESTS
#define Assert(expr) CHECK(expr)
#define Assert_False(expr) CHECK_FALSE(expr)
#else  // TESTS
#include <cassert>
#define Assert(expr) assert(expr)
#define Assert_False(expr) assert(!((bool)(expr)))
#endif  // TESTS

using u8 = char;

constexpr float floatInf = std::numeric_limits<float>::infinity();

//----------------------------------------------------------------------------------
// Defer.
//----------------------------------------------------------------------------------
template <typename F>
struct _Defer {
    _Defer(F f)
        : f(f) {}
    ~_Defer() {
        f();
    }
    F f;
};

template <typename F>
_Defer<F> _makeDefer(F f) {
    return _Defer<F>(f);
};

#define __defer(counter) defer_##counter
#define _defer(counter) __defer(counter)

struct _defer_dummy {};
template <typename F>
_Defer<F> operator+(_defer_dummy, F&& f) {
    return _makeDefer<F>(std::forward<F>(f));
}

// Usage:
//     {
//         defer { printf("Deferred\n"); };
//         printf("Normal\n");
//     }
//
// Output:
//     Normal
//     Deferred
//
#define defer auto _defer(__COUNTER__) = _defer_dummy() + [&]()
