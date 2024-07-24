// Raylib's function names collide with some parts of `windows.h`.
// This way some parts of `windows.h` (whick we don't need yet) don't get included.
//
// ref: Raylib doesn't build with windows.h #1217
// https://github.com/raysan5/raylib/issues/1217

#if defined(_WIN32)
#define NOGDI
#define NOUSER
#endif
