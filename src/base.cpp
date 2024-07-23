#define globalVar static
#define localPersist static

#define FOR_RANGE(type, variable_name, max_value_exclusive) \
    for (type variable_name = 0; (variable_name) < (max_value_exclusive); variable_name++)

#define INVALID_PATH assert(false)
#define NOT_IMPLEMENTED assert(false)
#define NOT_SUPPORTED assert(false)

#define scast static_cast
#define rcast reinterpret_cast

using u8 = char;

constexpr float floatInf = std::numeric_limits<float>::infinity();
