
struct Arena {
    size_t      used;
    size_t      size;
    u8*         base;
    const char* name;
};

#define AllocateFor(arena, type) rcast<type*>(Allocate_(arena, sizeof(type)))
#define AllocateArray(arena, type, count) \
    rcast<type*>(Allocate_(arena, sizeof(type) * (count)))

#define AllocateZerosFor(arena, type) rcast<type*>(AllocateZeros_(arena, sizeof(type)))
#define AllocateZerosArray(arena, type, count) \
    rcast<type*>(AllocateZeros_(arena, sizeof(type) * (count)))

// NOLINTBEGIN(bugprone-macro-parentheses)
#define AllocateArrayAndInitialize(arena, type, count)                     \
    [&]() {                                                                \
        auto ptr = rcast<type*>(Allocate_(arena, sizeof(type) * (count))); \
        FOR_RANGE (int, i, (count)) {                                      \
            std::construct_at(ptr + i);                                    \
        }                                                                  \
        return ptr;                                                        \
    }()
// NOLINTEND(bugprone-macro-parentheses)

#define DeallocateArray(arena, type, count) Deallocate_(arena, sizeof(type) * (count))

//
// TODO: Introduce the notion of `alignment` here!
// NOTE: Refer to Casey's memory allocation functions
// https://youtu.be/MvDUe2evkHg?list=PLEMXAbCVnmY6Azbmzj3BiC3QRYHE9QoG7&t=2121
//
u8* Allocate_(Arena& arena, size_t size) {
    Assert(size > 0);
    Assert(arena.size >= size);
    Assert(arena.used <= arena.size - size);

    u8* result = arena.base + arena.used;
    arena.used += size;
    return result;
}

u8* AllocateZeros_(Arena& arena, size_t size) {
    auto result = Allocate_(arena, size);
    memset(result, 0, size);
    return result;
}

void Deallocate_(Arena& arena, size_t size) {
    Assert(size > 0);
    Assert(arena.used >= size);
    arena.used -= size;
}

// TEMP_USAGE используется для временного использования арены.
// При вызове TEMP_USAGE запоминается текущее количество занятого
// пространства арены, которое обратно устанавливается при выходе из scope.
//
// Пример использования:
//
//     size_t X = trash_arena.used;
//     {
//         TEMP_USAGE(trash_arena);
//         int* aboba = ALLOCATE_FOR(trash_arena, u32);
//         Assert(trash_arena.used == X + 4);
//     }
//     Assert(trash_arena.used == X);
//
#define TEMP_USAGE(arena)                     \
    auto _arena_used_ = (arena).used;         \
    defer {                                   \
        Assert((arena).used >= _arena_used_); \
        (arena).used = _arena_used_;          \
    };
