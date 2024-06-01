#ifndef SAV_CORE_H
#define SAV_CORE_H

// TODO: Not sure if this is the best way to do this (to stop reporting on {0} initializers used in raylib)
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wmissing-braces"

#include <cstdint>
#include <cstring>
#include <cmath>
#include <sdl2/SDL_scancode.h>

// Types and macros
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;

typedef i32      b32;

void __debugbreak(); // usually in <intrin.h>
#define Assert(Expression) if (!(Expression)) { __debugbreak(); }
#define InvalidCodePath Assert(!"Invalid Code Path")
#define Noop { volatile int X = 0; }
#define Breakpoint __debugbreak()

#define ArrayCount(Array) (sizeof((Array)) / (sizeof((Array)[0])))

#define Kilobytes(Value) (         (Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)

#define Max(X, Y) (((X) > (Y)) ? (X) : (Y))
#define Min(X, Y) (((X) < (Y)) ? (X) : (Y))

#define global_var static
#define local_persist static
#define internal_func static

// Math
#define PI 3.14159265358979311599796346854

inline f32 ToRad(f32 d) { return (d * PI / 180.0f); }
inline f32 ToDeg(f32 r) { return (r / PI * 180.0f); }

union v2 { struct { f32 x, y; }; f32 e[2]; };
union v3 { struct { f32 x, y, z; }; struct { f32 r, g, b; }; f32 e[3]; };
union v4 { struct { f32 x, y, z, w; }; struct { f32 r, g, b, a; }; f32 e[4]; };
union v2i { struct { i32 x, y; }; i32 e[2]; };
struct m3 { f32 e[3][3]; };
struct m4 { f32 e[4][4]; };

inline v2 V2(f32 x, f32 y) { v2 result; result.x = x; result.y = y; return result; }
inline v3 V3(f32 x, f32 y, f32 z) { v3 result; result.x = x; result.y = y; result.z = z; return result; }
inline v4 V4(f32 x, f32 y, f32 z, f32 w ) { v4 result; result.x = x; result.y = y; result.z = z; result.w = w; return result; }
inline v2i V2I(i32 x, i32 y) { v2i result; result.x = x; result.y = y; return result; }
inline m3 M3(f32 d) { m3 result = {}; result.e[0][0] = d; result.e[1][1] = d; result.e[2][2] = d; return result; }
inline m4 M4(f32 d) { m4 result = {}; result.e[0][0] = d; result.e[1][1] = d; result.e[2][2] = d; result.e[3][3] = d; return result; }

inline v2 operator+(v2 a, v2 b) { return { a.x + b.x, a.y + b.y }; }
inline v2 operator-(v2 a, v2 b) { return { a.x - b.x, a.y - b.y }; }
inline v2 operator-(v2 a) { return { -a.x, -a.y }; }
inline v2 operator*(v2 v, f32 s) { return { v.x * s, v.y * s }; }
inline v2 operator*(f32 s, v2 v) { return { v.x * s, v.y * s }; }
inline v2 operator*(v2 a, v2 b) { return { a.x * b.x, a.y * b.y }; }
inline v2 operator/(v2 v, f32 s) { return { v.x / s, v.y / s }; }
inline v2 &operator+=(v2 &a, v2 b) { a = a + b; return a; }
inline v2 &operator-=(v2 &a, v2 b) { a = a - b; return a; }
inline v2 &operator*=(v2 &v, f32 s) { v = v * s; return v; }
inline v2 &operator/=(v2 &v, f32 s) { v = v / s; return v; }
inline f32 VecDot(v2 a, v2 b) { return a.x * b.x + a.y * b.y; }
inline f32 VecLengthSq(v2 v) { return v.x * v.x + v.y * v.y; }
inline f32 VecLength(v2 v) { return sqrtf(v.x * v.x + v.y * v.y); }
inline v2 VecNormalize(v2 v) { f32 l = sqrtf(v.x * v.x + v.y * v.y); Assert(l > 0.0f); return { v.x / l, v.y / l }; }

inline v3 operator+(v3 a, v3 b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
inline v3 operator-(v3 a, v3 b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
inline v3 operator-(v3 a) { return { -a.x, -a.y, -a.z }; }
inline v3 operator*(v3 a, v3 b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }
inline v3 operator*(v3 v, f32 s) { return { v.x * s, v.y * s, v.z * s }; }
inline v3 operator*(f32 s, v3 v) { return { v.x * s, v.y * s, v.z * s }; }
inline v3 operator/(v3 v, f32 s) { return { v.x / s, v.y / s, v.z / s }; }
inline v3 operator/(f32 s, v3 v) { return { s / v.x, s / v.y, s / v.z }; }
inline v3 &operator+=(v3 &a, v3 b) { a = a + b; return a; }
inline v3 &operator-=(v3 &a, v3 b) { a = a - b; return a; }
inline v3 &operator*=(v3 &v, f32 s) { v = v * s; return v; }
inline v3 &operator/=(v3 &v, f32 s) { v = v / s; return v; }
inline f32 VecDot(v3 a, v3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline f32 VecLengthSq(v3 v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
inline f32 VecLength(v3 v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }
inline v3 VecNormalize(v3 v) { f32 l = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); Assert(l > 0.0f); return { v.x / l, v.y / l, v.z / l }; } 
inline v3 VecCross(v3 a, v3 b) { return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }

inline v4 operator+(v4 a, v4 b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
inline v4 operator-(v4 a) { return { -a.x, -a.y, -a.z, -a.w }; }
inline v4 operator-(v4 a, v4 b) { return a + (-b); }
inline v4 operator*(v4 v, f32 s) { return { v.x * s, v.y * s, v.z * s, v.w * s }; }
inline v4 operator*(f32 s, v4 v) { return v * s; }
inline v4 operator/(v4 v, f32 s);
inline v4 &operator+=(v4 &a, v4 b);
inline v4 &operator-=(v4 &a, v4 b);
inline v4 &operator*=(v4 &v, f32 s);
inline v4 &operator/=(v4 &v, f32 s);

inline v2i operator+(v2i a, v2i b) { return { a.x + b.x, a.y + b.y }; }
inline v2i operator-(v2i a, v2i b) { return { a.x - b.x, a.y - b.y }; }
inline v2i operator-(v2i v) { return { -v.x, -v.y }; }
inline v2i operator*(v2i a, v2i b) { return { a.x * b.x, a.y * b.y }; }
inline v2i operator*(v2i v, i32 i) { return { v.x * i, v.y * i }; }
inline v2i operator*(v2i v, f32 i) { return { (i32) (v.x * i), (i32) (v.y * i) }; }
inline v2i operator*(i32 i, v2i v) { return { v.x * i, v.y * i }; }
inline v2i operator*(f32 i, v2i v) { return { (i32) (v.x * i), (i32) (v.y * i) }; }
inline v2i operator/(v2i v, i32 i) { return { v.x / i, v.y / i }; }
inline v2i operator/(v2i v, f32 i) { return { (i32) (v.x / i), (i32) (v.y / i) }; }
inline v2i operator/(i32 i, v2i v) { return { i / v.x, i / v.y }; }
inline v2i operator/(f32 i, v2i v) { return { (i32) (i / v.x), (i32) (i / v.y) }; }
inline v2i &operator+=(v2i &a, v2i b) { a = a + b; return a; }
inline v2i &operator-=(v2i &a, v2i b) { a = a - b; return a; }
inline v2i &operator*=(v2i &v, i32 s) { v = v * s; return v; }
inline v2i &operator/=(v2i &v, i32 s) { v = v / s; return v; }
inline b32 operator==(v2i a, v2i b) { return (a.x == b.x && a.y == b.y); }
inline i32 VecLengthSq(v2i v) { return (v.x * v.x + v.y * v.y); }

inline m3
operator*(m3 a, m3 b)
{
    m3 result = {};
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result.e[j][i] = (a.e[0][i]*b.e[j][0] +
                              a.e[1][i]*b.e[j][1] +
                              a.e[2][i]*b.e[j][2]);
        }
    }
    return result;
}
inline v3 operator*(m3 m, v3 v)
{
    v3 result = {};
    result.x = m.e[0][0] * v.x + m.e[1][0] * v.y + m.e[2][0] * v.z;
    result.y = m.e[0][1] * v.x + m.e[1][1] * v.y + m.e[2][1] * v.z;
    result.z = m.e[0][2] * v.x + m.e[1][2] * v.y + m.e[2][2] * v.z;
    return result;
}

inline m4
operator*(m4 a, m4 b)
{
    m4 result = {};
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result.e[j][i] = (a.e[0][j]*b.e[j][0] +
                              a.e[1][j]*b.e[j][1] +
                              a.e[2][j]*b.e[j][2] +
                              a.e[3][j]*b.e[j][3]);
        }
    }
    return result;
}
inline v4
operator*(m4 m, v4 v)
{
    v4 result = {};
    result.x = m.e[0][0] * v.x + m.e[1][0] * v.y + m.e[2][0] * v.z + m.e[3][0] * v.w;
    result.y = m.e[0][1] * v.x + m.e[1][1] * v.y + m.e[2][1] * v.z + m.e[3][1] * v.w;
    result.z = m.e[0][2] * v.x + m.e[1][2] * v.y + m.e[2][2] * v.z + m.e[3][2] * v.w;
    result.w = m.e[0][2] * v.x + m.e[1][2] * v.y + m.e[2][2] * v.z + m.e[3][3] * v.w;
    return result;
}

inline m4 GetPerspecitveProjection(f32 fovYDeg, f32 aspectRatio, f32 near, f32 far)
{
    // NOTE: http://www.songho.ca/opengl/gl_projectionmatrix.html
    m4 result = {};

    f32 halfHeight = near * tanf(ToRad(fovYDeg) * 0.5f);
    f32 halfWidth = halfHeight * aspectRatio;

    result.e[0][0] = near / halfWidth;
    result.e[1][1] = near / halfHeight;
    result.e[2][2] = -(far + near) / (far - near);
    result.e[2][3] = -1.0f;
    result.e[3][2] = -2.0f * far * near / (far - near);

    return result;
}

inline m4 GetOrthographicProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
    // NOTE: http://www.songho.ca/opengl/gl_projectionmatrix.html
    m4 result = {};

    result.e[0][0] = 2.0f / (right - left);
    result.e[1][1] = 2.0f / (top - bottom);
    result.e[2][2] = -2.0f / (far - near);

    result.e[3][0] = (right + left) / (left - right);
    result.e[3][1] = (top + bottom) / (bottom - top);
    result.e[3][2] = (far + near) / (near - far);
    result.e[3][3] = 1.0f;

    return result;
}

struct Rect { f32 x, y, w, h; }; Rect MakeRect(f32 x, f32 y, f32 w, f32 h);

// Memory arena
struct MemoryArena
{
    u8 *base;

    size_t size;
    size_t used;
    size_t prevUsed;
    size_t frozenUsed;
    size_t frozenPrevUsed;

    int freezeCount;
};

inline MemoryArena MakeMemoryArena(void *base, size_t size)
{
    MemoryArena arena = {};
    
    arena.size = size;
    arena.base = (u8 *) base;

    return arena;
}

inline void *MemoryArenaPushSize_(MemoryArena *arena, size_t size)
{
    Assert((arena->used + size) <= arena->size);
    void *result = arena->base + arena->used;
    arena->prevUsed = arena->used;
    arena->used += size;
    return result;
}


inline void *MemoryArenaPushSizeAndZero_(MemoryArena *arena, size_t size)
{
    void *base = MemoryArenaPushSize_(arena, size);
    memset(base, 0, size);
    return base;
}

inline void MemoryArenaResizePreviousPushArray_(MemoryArena *arena, size_t size)
{
    arena->used = arena->prevUsed + size;
}

#define MemoryArenaPushStruct(arena, type) (type *) MemoryArenaPushSize_(arena, sizeof(type))
#define MemoryArenaPushArray(arena, count, type) (type *) MemoryArenaPushSize_(arena, count * sizeof(type))
#define MemoryArenaPushBytes(arena, byteCount) (u8 *) MemoryArenaPushSize_(arena, byteCount)
#define MemoryArenaPushArrayAndZero(arena, count, type) (type *) MemoryArenaPushSizeAndZero_(arena, count * sizeof(type))
#define MemoryArenaPushStructAndZero(arena, type) (type *) MemoryArenaPushSizeAndZero_(arena, sizeof(type))
#define MemoryArenaResizePreviousPushArray(arena, count, type) MemoryArenaResizePreviousPushArray_(arena, count * sizeof(type))

inline MemoryArena MakeNestedMemoryArena(MemoryArena *arena, size_t size)
{
    MemoryArena NewArena = MakeMemoryArena(MemoryArenaPushBytes(arena, size), size);
    return NewArena;
}

inline void MemoryArenaFreeze(MemoryArena *arena)
{
    Assert(arena->freezeCount != 0);
    arena->freezeCount++;
    arena->frozenUsed = arena->used;
    arena->frozenPrevUsed = arena->prevUsed;
}

inline void MemoryArenaUnfreeze(MemoryArena *arena)
{
    Assert(arena->freezeCount > 0);
    arena->freezeCount--;
    arena->used = arena->frozenUsed;
    arena->prevUsed = arena->frozenPrevUsed;
    arena->frozenUsed = 0;
    arena->frozenPrevUsed = 0;
}

inline void MemoryArenaReset(MemoryArena *arena)
{
    arena->used = 0;
    arena->prevUsed = 0;
    arena->frozenUsed = 0;
    arena->frozenPrevUsed = 0;
}

// SAV functions
void InitWindow(const char *title, int width, int height);
void Quit();
void PollEvents();

void SetTargetFPS(f32 fps);
void BeginFrameTiming();
void EndFrameTiming();
u64 GetCurrentFrame();
f64 GetDeltaFixed();
f64 GetDeltaPrev();
f64 GetDeltaAvg();
f64 GetFPSPrev();
f64 GetFPSAvg();

b32 WindowShouldClose();
void SetWindowTitle(const char *title);
v2 GetWindowSize();
b32 WindowSizeChanged();
void SetWindowBorderless(b32 borderless);
void ToggleWindowBorderless();

b32 KeyDown(int key);
b32 KeyPressed(int key);
b32 KeyReleased(int key);
b32 KeyRepeat(int key);
b32 KeyPressedOrRepeat(int key);
b32 GetMouseRelativeMode();
void SetMouseRelativeMode(b32 enabled);
v2 MousePos();
v2 MouseRelPos();
b32 MouseDown(int button);
b32 MousePressed(int button);
b32 MouseReleased(int button);
b32 MouseClicks(int button, int clicks);
i32 MouseWheel();

void PrepareGpuData(u32 *vbo, u32 *vao, u32 *ebo, int maxVertCount, int maxIndexCount, u32 *matricesUbo, u32 matricesUboBindingPoint);
void PushProjection(m4 projection);
void PushModelView(m4 modelView);
void PopProjection();
void PopModelView();
void BeginDraw();
void DrawTexture();
void EndDraw();
void SavSwapBuffers();

const char *TextFormat(const char *format, ...);
void TraceLog(const char *format, ...);
void TraceError(const char *format, ...);

#ifdef SAV_IMPLEMENTATION

#include <windows.h>
#include <shellscalingapi.h>
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include <glad/glad.h>

#include <cstdio>

#define TIME_STAT_AVG_COUNT 32
#define PRINT_FRAME_TIME_UTIL 0
#define STRING_BUFFER 1024
#define MVP_MATRIX_STACK_COUNT 32

struct SdlState
{
    SDL_Window *window;
    v2 windowSize;
    b32 windowSizeChanged;
    b32 shouldClose;

    u64 perfCounterFreq;
    u64 lastCounter;
    f64 prevDelta;
    f64 deltaSamples[TIME_STAT_AVG_COUNT];
    int currentTimeStatSample;
    f64 avgDelta;

    UINT desiredSchedulerMs;
    b32 sleepIsGranular;
    b32 limitFps;
    f64 targetFps;
    f64 targetDelta;

    u64 currentFrame;
};

struct GlState
{
    sav_shader defaultShader;
    u32 defaultVBO;
    u32 defaultVAO;
    u32 defaultEBO;
    int maxVertexCount;
    int maxIndexCount;
    u32 matricesUBO;
    u32 matricesUBOBindingPoint;

    m4 projectionStack[MVP_MATRIX_STACK_COUNT];
    m4 modelViewStack[MVP_MATRIX_STACK_COUNT];
    int projectionStackCurrent;
    int modelViewStackCurrent;

    b32 drawModeActive;
    b32 canDraw;
};

struct InputState
{
    u8 currentKeyStates[SDL_NUM_SCANCODES];
    u8 previousKeyStates[SDL_NUM_SCANCODES];
    u8 repeatKeyStates[SDL_NUM_SCANCODES];
    v2 mousePos;
    v2 mouseRelPos;
    b32 isRelMouse;
    u8 currentMouseButtonStates[SDL_BUTTON_X2 + 1];
    u8 previousMouseButtonStates[SDL_BUTTON_X2 + 1];
    u8 clickMouseButtonStates[SDL_BUTTON_X2 + 1];
    i32 mouseWheel;
};

global_var SdlState _sdlState;
global_var GlState _glState;
global_var InputState _inputState;

// Internal functions
inline f64 getAvgDelta(f64 *samples, int sampleCount)
{
    f64 accum = 0.0f;
    for (int i = 0; i < sampleCount; i++)
    {
        accum += samples[i];
    }
    return accum / TIME_STAT_AVG_COUNT;
}

void TraceLogEngine(const char *severity, const char *format, ...);

// Window fundamentals
void InitWindow(const char *title, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        _sdlState.window = SDL_CreateWindow(title,
                                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                            width, height,
                                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        if (_sdlState.window)
        {
            int sdlImageFlags = IMG_INIT_JPG | IMG_INIT_PNG;
            int imgInitResult = IMG_Init(sdlImageFlags);
            if (!(imgInitResult & sdlImageFlags))
            {
                // TODO: Logging
                InvalidCodePath;
            }

            SDL_GLContext glContext = SDL_GL_CreateContext(_sdlState.window);

            if (glContext)
            {
                gladLoadGLLoader(SDL_GL_GetProcAddress);
                TraceLogEngine("INFO", "OpenGL loaded");
                TraceLogEngine("INFO", "Vendor: %s", glGetString(GL_VENDOR));
                TraceLogEngine("INFO", "Renderer: %s", glGetString(GL_RENDERER));
                TraceLogEngine("INFO", "Version: %s", glGetString(GL_VERSION));

                _sdlState.perfCounterFreq = SDL_GetPerformanceFrequency();

                TIMECAPS devCaps;
                MMRESULT devCapsGetResult = timeGetDevCaps(&devCaps, sizeof(devCaps));
                if (devCapsGetResult == MMSYSERR_NOERROR)
                {
                    TraceLogEngine("INFO", "Available DevCaps range: [%u, %u] ms", devCaps.wPeriodMin, devCaps.wPeriodMax);
                }

                _sdlState.desiredSchedulerMs = 1;
                _sdlState.sleepIsGranular = (timeBeginPeriod(_sdlState.desiredSchedulerMs) == TIMERR_NOERROR);
                TraceLogEngine("INFO", "Sleep is granular: %d", _sdlState.sleepIsGranular);

                // TODO: Don't do the following things in this function
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                u32 white = 0xFFFFFFFF;
                SavTexture defaultTexture = SavLoadTextureFromData(&white, 1, 1);
                _glState.defaultTextureId = defaultTexture.id;

                _glState.maxVertexCount = 65536;
                _glState.maxIndexCount = 393216;
                _glState.matricesUBOBindingPoint = 0;
                PrepareGpuData(&_glState.defaultVBO,
                               &_glState.defaultVAO,
                               &_glState.defaultEBO,
                               _glState.maxVertexCount,
                               _glState.maxIndexCount,
                               &_glState.matricesUBO,
                               _glState.matricesUBOBindingPoint);

                _glState.defaultShader = BuildBasicShader();
                _glState.currentShader = _glState.defaultShader;
                UseProgram(_glState.currentShader.glid);
                SetShaderMatricesBindingPoint(_glState.defaultShader, "Matrices");

                _glState.projectionStackCurr = -1;
                _glState.modelViewStackCurr = -1;
                PushProjection(M4(1));
                PushModelView(M4(1));

                SDL_GL_SetSwapInterval(0);
            }
            else
            {
                // TODO: Logging
                InvalidCodePath;
            }
        }
        else
        {
            // TODO: Logging
            InvalidCodePath;
        }
    }
    else
    {
        // TODO: Logging
        InvalidCodePath;
    }
}

void Quit()
{
    if (_sdlState.sleepIsGranular)
    {
        timeEndPeriod(_sdlState.desiredSchedulerMs);
    }

    if (_sdlState.window)
    {
        SDL_DestroyWindow(_sdlState.window);
    }

    SDL_Quit();
}

void PollEvents()
{
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        _inputState.repeatKeyStates[i] = 0;

        _inputState.previousKeyStates[i] = _inputState.currentKeyStates[i];
    }

    for (int i = 0; i < SDL_BUTTON_X2 + 1; i++)
    {
        _inputState.clickMouseButtonStates[i] = 0;

        _inputState.previousMouseButtonStates[i] = _inputState.currentMouseButtonStates[i];
    }

    _inputState.mouseWheel = 0;

    _sdlState.windowSizeChanged = false;

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            {
                _sdlState.shouldClose = true;
            } break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                _inputState.currentKeyStates[event.key.keysym.scancode] = (event.type == SDL_KEYDOWN);
                _inputState.repeatKeyStates[event.key.keysym.scancode] = event.key.repeat;
            } break;

            case SDL_MOUSEMOTION:
            {
                // NOTE: It seems it's better to update mouse position every frame
            } break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                _inputState.currentMouseButtonStates[event.button.button] = (event.type == SDL_MOUSEBUTTONDOWN);
                if (event.type == SDL_MOUSEBUTTONDOWN)
                {
                    _inputState.clickMouseButtonStates[event.button.button] = event.button.clicks;
                }
            } break;
            case SDL_MOUSEWHEEL:
            {
                // TODO: Maybe deal with event.wheel.direction field on other platforms
                _inputState.mouseWheel += event.wheel.y; // NOTE: Add y, because it's likely there were more than one event between frames
            } break;

            case SDL_WINDOWEVENT:
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    _sdlState.windowSize = V2((f32) event.window.data1, (f32) event.window.data2);
                    _sdlState.windowSizeChanged = true;
                }
            } break;
            default: break;
        }
    }

    int mouseX, mouseY, mouseRelX, mouseRelY;
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);
    _inputState.mousePos = V2((f32) mouseX, (f32) mouseY);
    _inputState.mouseRelPos = V2((f32) mouseRelX, (f32) mouseRelY);
}

// Timing
void SetTargetFPS(f32 fps)
{
    _sdlState.limitFps = true;
    _sdlState.targetFps = fps;
    _sdlState.targetDelta = 1.0 / _sdlState.targetFps;
    TraceLogEngine("INFO", "Target FPS: %.0f (%.6f ms)", _sdlState.targetFps, _sdlState.targetDelta);
}

void BeginFrameTiming()
{
    if (_sdlState.lastCounter != 0)
    {
        u64 currentCounter = SDL_GetPerformanceCounter();
        u64 counterElapsed = currentCounter - _sdlState.lastCounter;
        _sdlState.lastCounter = currentCounter;
        _sdlState.prevDelta = (f64) counterElapsed / _sdlState.perfCounterFreq;

        _sdlState.deltaSamples[_sdlState.currentTimeStatSample++] = _sdlState.prevDelta;
        if (_sdlState.currentTimeStatSample >= TIME_STAT_AVG_COUNT)
        {
            _sdlState.avgDelta = getAvgDelta(_sdlState.deltaSamples, TIME_STAT_AVG_COUNT);
            _sdlState.currentTimeStatSample = 0;
        }
    }
    else
    {
        _sdlState.lastCounter = SDL_GetPerformanceCounter();
    }

    _sdlState.currentFrame++;
}


void EndFrameTiming()
{
    if (_sdlState.limitFps)
    {
        u64 counterElapsed = SDL_GetPerformanceCounter() - _sdlState.lastCounter;
        f64 elapsedMs = (f64) counterElapsed / _sdlState.perfCounterFreq;

        #if (PRINT_FRAME_TIME_UTIL == 1)
        f64 frameTimeUtilization = elapsedMs / _sdlState.targetDelta;
        TraceLogEngine("INFO", "Frame time utilization: %f", frameTimeUtilization * 100.0);
        #endif

        u64 targetElapsed = (u64) (_sdlState.targetDelta * _sdlState.perfCounterFreq);

        int sleepForMs = (int) (1000.0 * (_sdlState.targetDelta - elapsedMs)) - 2;
        if (sleepForMs > 1)
        {
            Sleep((DWORD) sleepForMs);

            counterElapsed = SDL_GetPerformanceCounter() - _sdlState.lastCounter;
            if (counterElapsed > targetElapsed)
            {
                TraceLogEngine("WARNING", "SLEEP MISSED TARGET BY %f SEC", (f64) (counterElapsed - targetElapsed) / _sdlState.perfCounterFreq);
            }
        }

        while (counterElapsed < targetElapsed)
        {
            counterElapsed = SDL_GetPerformanceCounter() - _sdlState.lastCounter;
        }
    }
}

u64 GetCurrentFrame()
{
    return _sdlState.currentFrame;
}

f64 GetDeltaFixed()
{
    return _sdlState.targetDelta;
}

f64 GetDeltaPrev()
{
    return _sdlState.prevDelta;
}

f64 GetDeltaAvg()
{
    return _sdlState.avgDelta;
}

f64 GetFPSPrev()
{
    if (_sdlState.prevDelta > 0.0)
    {
        return 1.0 / _sdlState.prevDelta;
    }
    else
    {
        return 0.0;
    }
}

f64 GetFPSAvg()
{
    if (_sdlState.avgDelta > 0.0)
    {
        return 1.0 / _sdlState.avgDelta;
    }
    else
    {
        return 0.0;
    }
}

// Window util
b32 WindowShouldClose()
{
    return _sdlState.shouldClose;
}

void SetWindowTitle(const char *title)
{
    SDL_SetWindowTitle(_sdlState.window, title);
}

v2 GetWindowSize()
{
    return _sdlState.windowSize;
}

b32 WindowSizeChanged()
{
    return _sdlState.windowSizeChanged;
}

void SetWindowBorderless(b32 borderless)
{
    if (borderless)
    {
        int x, y;
        SDL_GetWindowPosition(_sdlState.window, &x, &y);
        _sdlState.windowRectBeforeBorderless = MakeRect((f32) x, (f32) y, _sdlState.windowSize.x, _sdlState.windowSize.y);
    }
    
    SDL_SetWindowBordered(_sdlState.window, (SDL_bool) !borderless);
    
    if (borderless)
    {
        SDL_MaximizeWindow(_sdlState.window);
    }
    else
    {
        SDL_RestoreWindow(_sdlState.window);

        // TODO: This is very hacky, maybe there's a better way to do this with SDL
        if (_sdlState.windowRectBeforeBorderless.w > 0)
        {
            SDL_SetWindowSize(_sdlState.window, (int) _sdlState.windowRectBeforeBorderless.w, (int) _sdlState.windowRectBeforeBorderless.h);
            SDL_SetWindowPosition(_sdlState.window, (int) _sdlState.windowRectBeforeBorderless.x, (int) _sdlState.windowRectBeforeBorderless.y);
            _sdlState.windowRectBeforeBorderless = {};
        }
    }
}

void ToggleWindowBorderless()
{
    _sdlState.borderless = !_sdlState.borderless;
    SetWindowBorderless(_sdlState.borderless);
}

// Input
b32 KeyDown(int key)
{
    return (b32) _inputState.currentKeyStates[key];
}

b32 KeyPressed(int key)
{
    return (b32) (_inputState.currentKeyStates[key] && !_inputState.previousKeyStates[key]);
}

b32 KeyReleased(int key)
{
    return (b32) (!_inputState.currentKeyStates[key] && _inputState.previousKeyStates[key]);
}

b32 KeyRepeat(int key)
{
    return (b32) _inputState.repeatKeyStates[key];
}

b32 KeyPressedOrRepeat(int key)
{
    return (b32) ((_inputState.currentKeyStates[key] && !_inputState.previousKeyStates[key]) || _inputState.repeatKeyStates[key]);
}

b32 GetMouseRelativeMode()
{
    return (b32) SDL_GetRelativeMouseMode();
}

void SetMouseRelativeMode(b32 enabled)
{
    _inputState.isRelMouse = enabled;
    SDL_SetRelativeMouseMode((SDL_bool) _inputState.isRelMouse);
}

v2 MousePos()
{
    return _inputState.mousePos;
}

v2 MouseRelPos()
{
    return _inputState.mouseRelPos;
}

b32 MouseDown(int button)
{
    return (b32) _inputState.currentMouseButtonStates[button];
}

b32 MousePressed(int button)
{
    return (b32) (_inputState.currentMouseButtonStates[button] && !_inputState.previousMouseButtonStates[button]);
}

b32 MouseReleased(int button)
{
    return (b32) (!_inputState.currentMouseButtonStates[button] && _inputState.previousMouseButtonStates[button]);
}

b32 MouseClicks(int button, int clicks)
{
    return (b32) (_inputState.clickMouseButtonStates[button] == clicks);
}

i32 MouseWheel()
{
    return _inputState.mouseWheel;
}

// Graphics
void PrepareGpuData(u32 *vbo, u32 *vao, u32 *ebo, int maxVertCount, int maxIndexCount,
    u32 *matricesUbo, u32 matricesUboBindingPoint)
{
    size_t BytesPerVertex = (3 + 4 + 4) * sizeof(float);
    
    glGenVertexArrays(1, vao);
    Assert(*vao);
                    
    glGenBuffers(1, vbo);
    Assert(*vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, maxVertCount * BytesPerVertex, NULL, GL_DYNAMIC_DRAW);

    glBindVertexArray(*vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(v3), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(v4), (void *) (maxVertCount * sizeof(v3)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(v4), (void *) (maxVertCount * (sizeof(v3) + sizeof(v4))));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, ebo);
    Assert(*ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxIndexCount * sizeof(u32), NULL, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    size_t matricesUboSize = sizeof(m4);
    glGenBuffers(1, matricesUbo);
    Assert(*matricesUbo);
    glBindBuffer(GL_UNIFORM_BUFFER, *matricesUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(m4), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, matricesUboBindingPoint, *matricesUbo);
}

void setCurrentMvp()
{
    m4 mvp = _glState.projectionStack[_glState.projectionStackCurrent] * _glState.modelViewStack[_glState.modelViewStackCurrent];
    
    glBindBuffer(GL_UNIFORM_BUFFER, _glState.matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m4), &mvp);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void PushProjection(m4 projection)
{
    Assert(_glState.projectionStackCurrent < MVP_MATRIX_STACK_COUNT - 2);
    _glState.projectionStack[++_glState.projectionStackCurrent] = projection;
    setCurrentMvp();
}

void PushModelView(m4 modelView)
{
    Assert(_glState.modelViewStackCurrent < MVP_MATRIX_STACK_COUNT - 2);
    _glState.modelViewStack[++_glState.modelViewStackCurrent] = modelView;
    setCurrentMvp();
}

void PopProjection()
{
    Assert(_glState.projectionStackCurrent > 0);
    --_glState.projectionStackCurrent;
    setCurrentMvp();
}

void PopModelView()
{
    Assert(_glState.modelViewStackCurrent > 0);
    --_glState.modelViewStackCurrent;
    setCurrentMvp();
}

void BeginDraw()
{
    Assert(!_glState.drawModeActive);

    glViewport(0, 0, (int) _sdlState.windowSize.x, (int) _sdlState.windowSize.y);
    PushProjection(
        GetOrthographicProjection(
            0.0f,
            _sdlState.windowSize.x,
            _sdlState.windowSize.y,
            0.0f, -1.0f, 1.0f)
    );

    _glState.drawModeActive = true;
    _glState.canDraw = true;
}

void DrawTexture()
{
}

void EndDraw()
{
    Assert(_glState.drawModeActive);

    PopProjection();

    _glState.drawModeActive = true;
    _glState.canDraw = false;
}

void SavSwapBuffers()
{
    SDL_GL_SwapWindow(_sdlState.window);
}

// Util
const char *TextFormat(const char *format, ...)
{
    // TODO: There are quite a few cases where this won't work, maybe do as macro
    local_persist char result[STRING_BUFFER];

    va_list varArgs;
    va_start(varArgs, format);
    vsprintf_s(result, STRING_BUFFER - 1, format, varArgs);
    va_end(varArgs);

    return result;
}

void TraceLog(const char *format, ...)
{
    char formatBuf[STRING_BUFFER];
    sprintf_s(formatBuf, "GAME: INFO: [F %06zu] %s\n", _sdlState.currentFrame, format);
    
    va_list varArgs;
    va_start(varArgs, format);
    vprintf_s(formatBuf, varArgs);
    va_end(varArgs);
}

void TraceError(const char *format, ...)
{
    char formatBuf[STRING_BUFFER];
    sprintf_s(formatBuf, "GAME: ERROR: [F %06zu] %s\n", _sdlState.currentFrame, format);
    
    va_list varArgs;
    va_start(varArgs, format);
    vprintf_s(formatBuf, varArgs);
    va_end(varArgs);
}

void TraceLogEngine(const char *severity, const char *format, ...)
{
    char formatBuf[STRING_BUFFER];
    sprintf_s(formatBuf, "ENGINE: %s: [F %06zu] %s\n", severity, _sdlState.currentFrame, format);
    
    va_list varArgs;
    va_start(varArgs, format);
    vprintf_s(formatBuf, varArgs);
    va_end(varArgs);
}

#endif

#endif