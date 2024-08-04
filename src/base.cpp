#define globalVar static
#define localPersist static

// NOLINTBEGIN(bugprone-macro-parentheses)
#define FOR_RANGE(type, variable_name, max_value_exclusive) \
    for (type variable_name = 0; (variable_name) < (max_value_exclusive); variable_name++)
// NOLINTEND(bugprone-macro-parentheses)

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

constexpr float  floatInf  = std::numeric_limits<float>::infinity();
constexpr double doubleInf = std::numeric_limits<double>::infinity();

//----------------------------------------------------------------------------------
// Defer.
//----------------------------------------------------------------------------------
template <typename F>
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct Defer_ {
    Defer_(F f)
        : f(f) {}
    ~Defer_() {
        f();
    }
    F f;
};

template <typename F>
Defer_<F> makeDefer_(F f) {
    return Defer_<F>(f);
};

#define defer_with_counter_(counter) defer_##counter
#define defer_(counter) defer_with_counter_(counter)

struct defer_dummy_ {};

template <typename F>
// NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward)
Defer_<F> operator+(defer_dummy_, F&& f) {
    return makeDefer_<F>(std::forward<F>(f));
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
// NOLINTNEXTLINE(bugprone-macro-parentheses)
#define defer auto defer_(__COUNTER__) = defer_dummy_() + [&]()
