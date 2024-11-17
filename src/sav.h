#ifndef SAV_CORE_H
#define SAV_CORE_H

// #pragma clang diagnostic ignored "-Wmissing-field-initializers"
// #pragma clang diagnostic ignored "-Wunused-variable"
// #pragma clang diagnostic ignored "-Wmissing-braces"

#include <cstdint>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <sdl2/SDL_scancode.h>

// SECTION Types and macros
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

#define Max(x, y) (((x) > (y)) ? (x) : (y))
#define Min(x, y) (((x) < (y)) ? (x) : (y))
#define SavSwap(x, y, type) do { type temp = x; x = y; y = temp; } while(0)

#define global_var static
#define local_persist static
#define sav_func
#define api_func static
#define internal_func static

// SECTION Math
#define PI 3.14159265358979311599796346854
#define PI32 3.14159265358979311599796346854f
#define CMP_EPSILON 0.00001f

inline f32 ToRad(f32 d) { return (f32) (d * PI / 180.0f); }
inline f32 ToDeg(f32 r) { return (f32) (r / PI * 180.0f); }
inline f32 AbsF32(f32 a) { return fabs(a); }
inline i32 AbsI32(i32 a) { return abs(a); }

inline f32 ExponentialInterpolation(f32 min, f32 max, f32 t)
{
    f32 logMin = log(min);
    f32 logMax = log(max);
    f32 lerp = logMin + (logMax - logMin) * t;
    f32 result = exp(lerp);
    return result;
}

inline f32 ExponentialInterpolationWhereIs(f32 min, f32 max, f32 v)
{
    f32 logMin = log(min);
    f32 logMax = log(max);
    f32 logV = log(v);
    f32 result;
    if (logV >= logMax)
    {
        result = 1.0f;
    }
    else if (logV <= logMin)
    {
        result = 0.0f;
    }
    else
    {
        result = (logV - logMin) / (logMax - logMin);
    }
    return result;
}

inline i32 RoundF32ToI32(f32 a)
{
    i32 result = (i32) (a + 0.5f);
    return result;
}

inline f32 Sign(f32 value)
{
    if (value >= 0.0f)
    {
        return 1.0f;
    }
    else
    {
        return -1.0f;
    }
}

inline f32 Square(f32 value)
{
    return value * value;
}

inline i32 Square(i32 value)
{
    return value * value;
}

inline f32 MoveToward(f32 from, f32 to, f32 delta)
{
    f32 diff = AbsF32(from - to);
    if (diff <= delta)
    {
        return to;
    }
    else
    {
        return from + Sign(to - from) * delta;
    }
}

inline b32 MoveToward(f32 *current, f32 target, f32 delta)
{
    f32 diff = AbsF32(*current - target);
    if (diff <= delta)
    {
        *current = target;
        return true;
    }
    else
    {
        *current += Sign(target - *current) * delta;
        return false;
    }
}

inline b32 MoveTowardAngleDeg(f32 *current, f32 target, f32 delta)
{
    f32 diff = target - *current;

    while (diff > 180.0f)
    {
        target -= 360.0f;
        diff = target - *current;
    }

    while (diff < -180.0f)
    {
        target += 360.0f;
        diff = target - *current;
    }
    
    b32 result = MoveToward(current, target, delta);
    while (*current >= 360.0f)
    {
        *current -= 360.0f;
    }
    while (*current < 0.0f)
    {
        *current += 360.0f;
    }
    return result;
}

inline f32 GetAngleDegDiff(f32 a, f32 b)
{
    f32 diff = b - a;

    while (diff > 180.0f)
    {
        b -= 360.0f;
        diff = b - a;
    }

    while (diff < -180.0f)
    {
        b += 360.0f;
        diff = b - a;
    }

    return diff;
}

inline b32 MoveTowardAngleDegDamped(f32 *current, f32 target, f32 damping, f32 delta)
{
    f32 diff = target - *current;

    while (diff > 180.0f)
    {
        target -= 360.0f;
        diff = target - *current;
    }

    while (diff < -180.0f)
    {
        target += 360.0f;
        diff = target - *current;
    }
    
    diff *= damping;
    target = *current + diff;

    b32 result = MoveToward(current, target, delta);
    while (*current > 360.0f)
    {
        *current -= 360.0f;
    }
    while (*current < 0.0f)
    {
        *current += 360.0f;
    }
    return result;
}

union v2 { struct { f32 x, y; }; f32 e[2]; };
union v3 { struct { f32 x, y, z; }; struct { f32 r, g, b; }; f32 e[3]; };
union v4 { struct { f32 x, y, z, w; }; struct { f32 r, g, b, a; }; f32 e[4]; };
union v2i { struct { i32 x, y; }; i32 e[2]; };
struct m3 { f32 e[3][3]; };
struct m4 { f32 e[4][4]; };

inline v2 V2(f32 x, f32 y) { v2 result; result.x = x; result.y = y; return result; }
inline v2 V2(v2i v) { v2 result; result.x = (f32)v.x; result.y = (f32)v.y; return result; }
inline v3 V3(f32 x, f32 y, f32 z) { v3 result; result.x = x; result.y = y; result.z = z; return result; }
inline v3 V3(v2 v) { v3 result; result.x = v.x; result.y = v.y; result.z = 0.0f; return result; }
inline v4 V4(f32 x, f32 y, f32 z, f32 w ) { v4 result; result.x = x; result.y = y; result.z = z; result.w = w; return result; }
inline v4 V4(v2 v) { v4 result; result.x = v.x; result.y = v.y; result.z = 0.0f; result.w = 0.0f; return result; }
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
inline v2 VecNormalizeIN0(v2 v) { f32 l = sqrtf(v.x * v.x + v.y * v.y); if(l == 0.0f) return {}; return { v.x / l, v.y / l }; }
inline b32 VecEqual(v2 a, v2 b, f32 epsilon) { return (fabs(a.x - b.x) <= epsilon && fabs(a.y - b.y) <= epsilon); }
inline b32 VecEqualExact(v2 a, v2 b) { return a.x == b.x && a.y == b.y; }
inline f32 VecCross2(v2 a, v2 b) { return (a.x * b.y - b.x * a.y); }

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
inline v3 VecNormalizeIN0(v3 v) { f32 l = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); if (l == 0.0f) return {}; return { v.x / l, v.y / l, v.z / l }; } 
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

inline m3 operator*(m3 a, m3 b)
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

inline m4 operator*(m4 a, m4 b)
{
    m4 result = {};
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result.e[j][i] = (a.e[0][i]*b.e[j][0] +
                              a.e[1][i]*b.e[j][1] +
                              a.e[2][i]*b.e[j][2] +
                              a.e[3][i]*b.e[j][3]);
        }
    }
    return result;
}
inline v4 operator*(m4 m, v4 v)
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

inline m4 GetCamera2DView(v2 target, f32 s, f32 rot, v2 offset)
{
    /*
     * This is this but hardcoded:
       m4 translation = Mat4GetTranslation(-V3(target));
       m4 scale = GetScale(V3(s, s, 1.0f));
       m4 rotation = M4(GetRotationAroundAxis(V3(0,0,1), ToRad(rot)));
       m4 offset = GetTranslation(V3(offset));
       m4 calc = offset * rotation * scale * translation;
    */
    m4 result = M4(1);
    f32 cosRot = cosf(ToRad(rot));
    f32 sinRot = sinf(ToRad(rot));
    v2 t = -target;
    result.e[0][0] = s * cosRot;
    result.e[0][1] = s * sinRot;
    result.e[1][0] = -s * sinRot;
    result.e[1][1] = s * cosRot;
    result.e[3][0] = s * t.x * cosRot - s * t.y * sinRot + offset.x;
    result.e[3][1] = s * t.x * sinRot + s * t.y * cosRot + offset.y;
    return result;
}

inline m4 GetCamera2DViewInv(v2 target, f32 s, f32 rot, v2 offset)
{
    /*
     * This is this but hardcoded:
       m4 offsetInv = GetTranslation(-V3(offset));
       m4 rotationInv = M4(GetRotationAroundAxis(V3(0,0,1), ToRad(-rot)));
       m4 scaleInv = GetScale(V3(1.0f/s, 1.0f/s, 1.0f));
       m4 translationInv = GetTranslation(V3(target));
       m4 calcInv = translationInv * scaleInv * rotationInv * offsetInv;
    */
    m4 result = M4(1);
    f32 cosRot = cosf(ToRad(rot));
    f32 sinRot = sinf(ToRad(rot));
    v2 t = -target;
    f32 oneOverS = 1.0f / s;
    result.e[0][0] = oneOverS * cosRot;
    result.e[0][1] = -oneOverS * sinRot;
    result.e[1][0] = oneOverS * sinRot;
    result.e[1][1] = oneOverS * cosRot;
    result.e[3][0] = -offset.x * cosRot * oneOverS - offset.y * sinRot * oneOverS - t.x;
    result.e[3][1] = offset.x * sinRot * oneOverS - offset.y * cosRot * oneOverS - t.y;
    return result;
}

inline m4 GetCamera2DViewInvRel(f32 s, f32 rot)
{
    /*
     * This is this but hardcoded:
       m4 rotationInv = M4(GetRotationAroundAxis(V3(0,0,1), ToRad(-rot)));
       m4 scaleInv = GetScale(V3(1.0f/s, 1.0f/s, 1.0f));
       m4 CcalcInv = scaleInv * rotationInv;
    */
    m4 result = M4(1);
    f32 cosRot = cosf(ToRad(rot));
    f32 sinRot = sinf(ToRad(rot));
    f32 oneOverS = 1.0f / s;
    result.e[0][0] = oneOverS * cosRot;
    result.e[0][1] = -oneOverS * sinRot;
    result.e[1][0] = oneOverS * sinRot;
    result.e[1][1] = oneOverS * cosRot;
    return result;
}

struct FourV2
{
    v2 e[4];
};
struct FourV3
{
    v3 e[4];
};
struct FourV4
{
    v4 e[4];
};
inline FourV3 ConvertFourV2V3(FourV2 a)
{
    FourV3 result;
    result.e[0] = V3(a.e[0].x, a.e[0].y, 0.0f);
    result.e[1] = V3(a.e[1].x, a.e[1].y, 0.0f);
    result.e[2] = V3(a.e[2].x, a.e[2].y, 0.0f);
    result.e[3] = V3(a.e[3].x, a.e[3].y, 0.0f);
    return result;
}
inline FourV4 ConvertFourV2V4(FourV2 a)
{
    FourV4 result;
    result.e[0] = V4(a.e[0].x, a.e[0].y, 0.0f, 0.0f);
    result.e[1] = V4(a.e[1].x, a.e[1].y, 0.0f, 0.0f);
    result.e[2] = V4(a.e[2].x, a.e[2].y, 0.0f, 0.0f);
    result.e[3] = V4(a.e[3].x, a.e[3].y, 0.0f, 0.0f);
    return result;
}

inline void RotateFourPointsAroundOrigin(FourV2 *points, v2 origin, f32 rotation)
{
    f32 c = cosf(ToRad(rotation));
    f32 s = sinf(ToRad(rotation));
    for (int i = 0; i < 4; i++)
    {
        points->e[i] -= origin;
        f32 x = points->e[i].x;
        f32 y = points->e[i].y;
        points->e[i] = V2(c*x - s*y, s*x + c*y);
        points->e[i] += origin;
    }
}


struct Rect { f32 x, y, w, h; };
inline Rect MakeRect(f32 x, f32 y, f32 w, f32 h) { Rect r; r.x = x; r.y = y; r.w = w; r.h = h; return r; };
inline Rect RectMinMax(v2 min, v2 max) { return { min.x, min.y, max.x - min.x, max.y - min.y }; }
inline v2 RectGetMin(Rect r) { return { r.x, r.y }; }
inline v2 RectGetMax(Rect r) { return { r.x + r.w, r.y + r.h }; }
inline v2 RectGetMid(Rect r) { return { r.x + r.w * 0.5f, r.y + r.h * 0.5f }; }

struct RectInt { i32 x, y, w, h; };
inline RectInt MakeRect(i32 x, i32 y, i32 w, i32 h) { RectInt r; r.x = x; r.y = y; r.w = w; r.h = h; return r; };
inline RectInt RectMinMax(v2i min, v2i max) { return { min.x, min.y, max.x - min.x, max.y - min.y }; }
inline v2i RectGetMin(RectInt r) { return { r.x, r.y }; }
inline v2i RectGetMax(RectInt r) { return { r.x + r.w, r.y + r.h }; }
inline v2i RectGetMid(RectInt r) { return { r.x + r.w / 2, r.y + r.h / 2 }; }

inline Rect RectIntersect(Rect r1, Rect r2)
{
    v2 r1Min = RectGetMin(r1);
    v2 r2Min = RectGetMin(r2);
    v2 r1Max = RectGetMax(r1);
    v2 r2Max = RectGetMax(r2);
    v2 min = V2(Max(r1Min.x, r2Min.x), Max(r1Min.y, r2Min.y));
    v2 max = V2(Min(r1Max.x, r2Max.x), Min(r1Max.y, r2Max.y));
    Rect result = RectMinMax(min, max);
    return result;
}

inline RectInt RectIntersect(RectInt r1, RectInt r2)
{
    v2i r1Min = RectGetMin(r1);
    v2i r2Min = RectGetMin(r2);
    v2i r1Max = RectGetMax(r1);
    v2i r2Max = RectGetMax(r2);
    v2i min = V2I(Max(r1Min.x, r2Min.x), Max(r1Min.y, r2Min.y));
    v2i max = V2I(Min(r1Max.x, r2Max.x), Min(r1Max.y, r2Max.y));
    RectInt result = RectMinMax(min, max);
    return result;
}

inline FourV2 RectGetPoints(Rect r)
{
    FourV2 points;
    points.e[0] = { r.x, r.y };
    points.e[1] = { r.x, r.y + r.h };
    points.e[2] = { r.x + r.w, r.y + r.h };
    points.e[3] = { r.x + r.w, r.y };
    return points;
}

inline b32 InBounds(v2 min, v2 max, v2 p)
{
    return (p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y);
}

inline v2 GetUnitVecFromAngle(f32 rads)
{
    v2 result;
    result.x = cosf(rads);
    result.y = sinf(rads);
    return result;
}

inline v2 GetUnitVecFromAngleFlipY(f32 rads)
{
    v2 result;
    result.x = cosf(rads);
    result.y = -sinf(rads);
    return result;
}

inline f32 GetVecFlippedYAngle(v2 v)
{
    f32 angle = atan2f(-v.y, v.x);
    if (angle < 0.0f) angle += 2.0f * PI32;
    return angle;
}

inline b32 MoveToward(v2 *position, v2 target, f32 dP)
{
    v2 diff = target - *position;
    f32 dist = VecLength(diff);
    if (dist <= dP || dist < (f32)CMP_EPSILON)
    {
        *position = target;
        return true;
    }
    else
    {
        *position += diff / dist * dP;
        return false;
    }
}

inline b32 MoveTowardDamped(v2 *position, v2 target, f32 damping, f32 dP)
{
    v2 diff = target - *position;
    diff *= damping;
    target = *position + diff;
    
    f32 dist = VecLength(diff);
    if (dist <= dP || dist < (f32)CMP_EPSILON)
    {
        *position = target;
        return true;
    }
    else
    {
        *position += diff / dist * dP;
        return false;
    }
}

inline v2 MoveToward(v2 from, v2 to, f32 dP)
{
    v2 diff = to - from;
    f32 dist = VecLength(diff);
    if (dist <= dP || dist < (f32)CMP_EPSILON)
    {
        return to;
    }
    else
    {
        return from + diff / dist * dP;
    }
}

// SECTION String
struct CountedString
{
    size_t size;
    char *string;
};

inline CountedString MakeCountedString(char *string, size_t size)
{
    CountedString result;
    result.size = size;
    result.string = string;
    return result;
}

inline CountedString MakeCountedString(char *string)
{
    CountedString result;
    result.size = strlen(string);
    result.string = string;
    return result;
}

struct StringBuffer
{
    size_t size;
    char *string;
};

inline StringBuffer MakeStringBuffer(char *string, size_t size)
{
    StringBuffer result;
    result.size = size;
    result.string = string;
    return result;
}

#define MakeStringBufferOnStack(BUFFER_NAME, SIZE) char BUFFER_NAME ## _localVar[SIZE]; \
    StringBuffer BUFFER_NAME = MakeStringBuffer(BUFFER_NAME ## _localVar, SIZE);

// SECTION Memory arena
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
    Assert(arena->freezeCount == 0);
    arena->freezeCount++;
    arena->frozenUsed = arena->used;
    arena->frozenPrevUsed = arena->prevUsed;
}

inline void MemoryArenaRevert(MemoryArena *arena)
{
    Assert(arena->freezeCount > 0);
    arena->used = arena->frozenUsed;
    arena->prevUsed = arena->frozenPrevUsed;
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

// SECTION Colors
union SavColor { struct { u8 a; u8 b; u8 g; u8 r; }; u32 c32; };
inline SavColor MakeColor(u8 r, u8 g, u8 b, u8 a) { SavColor c; c.r = r; c.g = g; c.b = b; c.a = a; return c; }
inline SavColor MakeColor(f32 r, f32 g, f32 b, f32 a) { SavColor c; c.r = (u8) (r * 255.0f); c.g = (u8) (g * 255.0f); c.b = (u8) (b * 255.0f); c.a = (u8) (a * 255.0f); return c; }
inline SavColor MakeColor(u32 c32) { SavColor c; c.c32 = c32; return c; }
inline v4 GetColorV4(SavColor c) { f32 oo255 = 0.0039215686274509803921568627451f; v4 result = V4(c.r * oo255, c.g * oo255, c.b * oo255, c.a * oo255); return result; }
inline SavColor ColorAlpha(SavColor c, f32 a) { c.a = (u8) (a * 255.0f); return c; }

#define SAV_COLOR_ALICEBLUE MakeColor(0xF0F8FFFF)
#define SAV_COLOR_ANTIQUEWHITE MakeColor(0xFAEBD7FF)
#define SAV_COLOR_AQUA MakeColor(0x00FFFFFF)
#define SAV_COLOR_AQUAMARINE MakeColor(0x7FFFD4FF)
#define SAV_COLOR_AZURE MakeColor(0xF0FFFFFF)
#define SAV_COLOR_BEIGE MakeColor(0xF5F5DCFF)
#define SAV_COLOR_BISQUE MakeColor(0xFFE4C4FF)
#define SAV_COLOR_BLACK MakeColor(0x000000FF)
#define SAV_COLOR_BLANCHEDALMOND MakeColor(0xFFEBCDFF)
#define SAV_COLOR_BLUE MakeColor(0x0000FFFF)
#define SAV_COLOR_BLUEVIOLET MakeColor(0x8A2BE2FF)
#define SAV_COLOR_BROWN MakeColor(0xA52A2AFF)
#define SAV_COLOR_BURLYWOOD MakeColor(0xDEB887FF)
#define SAV_COLOR_CADETBLUE MakeColor(0x5F9EA0FF)
#define SAV_COLOR_CHARTREUSE MakeColor(0x7FFF00FF)
#define SAV_COLOR_CHOCOLATE MakeColor(0xD2691EFF)
#define SAV_COLOR_CORAL MakeColor(0xFF7F50FF)
#define SAV_COLOR_CORNFLOWERBLUE MakeColor(0x6495EDFF)
#define SAV_COLOR_CORNSILK MakeColor(0xFFF8DCFF)
#define SAV_COLOR_CRIMSON MakeColor(0xDC143CFF)
#define SAV_COLOR_CYAN MakeColor(0x00FFFFFF)
#define SAV_COLOR_DARKBLUE MakeColor(0x00008BFF)
#define SAV_COLOR_DARKCYAN MakeColor(0x008B8BFF)
#define SAV_COLOR_DARKGOLDENROD MakeColor(0xB8860BFF)
#define SAV_COLOR_DARKGRAY MakeColor(0xA9A9A9FF)
#define SAV_COLOR_DARKGREEN MakeColor(0x006400FF)
#define SAV_COLOR_DARKGREY MakeColor(0xA9A9A9FF)
#define SAV_COLOR_DARKKHAKI MakeColor(0xBDB76BFF)
#define SAV_COLOR_DARKMAGENTA MakeColor(0x8B008BFF)
#define SAV_COLOR_DARKOLIVEGREEN MakeColor(0x556B2FFF)
#define SAV_COLOR_DARKORANGE MakeColor(0xFF8C00FF)
#define SAV_COLOR_DARKORCHID MakeColor(0x9932CCFF)
#define SAV_COLOR_DARKRED MakeColor(0x8B0000FF)
#define SAV_COLOR_DARKSALMON MakeColor(0xE9967AFF)
#define SAV_COLOR_DARKSEAGREEN MakeColor(0x8FBC8FFF)
#define SAV_COLOR_DARKSLATEBLUE MakeColor(0x483D8BFF)
#define SAV_COLOR_DARKSLATEGRAY MakeColor(0x2F4F4FFF)
#define SAV_COLOR_DARKSLATEGREY MakeColor(0x2F4F4FFF)
#define SAV_COLOR_DARKTURQUOISE MakeColor(0x00CED1FF)
#define SAV_COLOR_DARKVIOLET MakeColor(0x9400D3FF)
#define SAV_COLOR_DEEPPINK MakeColor(0xFF1493FF)
#define SAV_COLOR_DEEPSKYBLUE MakeColor(0x00BFFFFF)
#define SAV_COLOR_DIMGRAY MakeColor(0x696969FF)
#define SAV_COLOR_DIMGREY MakeColor(0x696969FF)
#define SAV_COLOR_DODGERBLUE MakeColor(0x1E90FFFF)
#define SAV_COLOR_FIREBRICK MakeColor(0xB22222FF)
#define SAV_COLOR_FLORALWHITE MakeColor(0xFFFAF0FF)
#define SAV_COLOR_FORESTGREEN MakeColor(0x228B22FF)
#define SAV_COLOR_FUCHSIA MakeColor(0xFF00FFFF)
#define SAV_COLOR_GAINSBORO MakeColor(0xDCDCDCFF)
#define SAV_COLOR_GHOSTWHITE MakeColor(0xF8F8FFFF)
#define SAV_COLOR_GOLD MakeColor(0xFFD700FF)
#define SAV_COLOR_GOLDENROD MakeColor(0xDAA520FF)
#define SAV_COLOR_GRAY MakeColor(0x808080FF)
#define SAV_COLOR_GREEN MakeColor(0x008000FF)
#define SAV_COLOR_GREENYELLOW MakeColor(0xADFF2FFF)
#define SAV_COLOR_GREY MakeColor(0x808080FF)
#define SAV_COLOR_HONEYDEW MakeColor(0xF0FFF0FF)
#define SAV_COLOR_HOTPINK MakeColor(0xFF69B4FF)
#define SAV_COLOR_INDIANRED MakeColor(0xCD5C5CFF)
#define SAV_COLOR_INDIGO MakeColor(0x4B0082FF)
#define SAV_COLOR_IVORY MakeColor(0xFFFFF0FF)
#define SAV_COLOR_KHAKI MakeColor(0xF0E68CFF)
#define SAV_COLOR_LAVENDER MakeColor(0xE6E6FAFF)
#define SAV_COLOR_LAVENDERBLUSH MakeColor(0xFFF0F5FF)
#define SAV_COLOR_LAWNGREEN MakeColor(0x7CFC00FF)
#define SAV_COLOR_LEMONCHIFFON MakeColor(0xFFFACDFF)
#define SAV_COLOR_LIGHTBLUE MakeColor(0xADD8E6FF)
#define SAV_COLOR_LIGHTCORAL MakeColor(0xF08080FF)
#define SAV_COLOR_LIGHTCYAN MakeColor(0xE0FFFFFF)
#define SAV_COLOR_LIGHTGOLDENRODYELLOW MakeColor(0xFAFAD2FF)
#define SAV_COLOR_LIGHTGRAY MakeColor(0xD3D3D3FF)
#define SAV_COLOR_LIGHTGREEN MakeColor(0x90EE90FF)
#define SAV_COLOR_LIGHTGREY MakeColor(0xD3D3D3FF)
#define SAV_COLOR_LIGHTPINK MakeColor(0xFFB6C1FF)
#define SAV_COLOR_LIGHTSALMON MakeColor(0xFFA07AFF)
#define SAV_COLOR_LIGHTSEAGREEN MakeColor(0x20B2AAFF)
#define SAV_COLOR_LIGHTSKYBLUE MakeColor(0x87CEFAFF)
#define SAV_COLOR_LIGHTSLATEGRAY MakeColor(0x778899FF)
#define SAV_COLOR_LIGHTSLATEGREY MakeColor(0x778899FF)
#define SAV_COLOR_LIGHTSTEELBLUE MakeColor(0xB0C4DEFF)
#define SAV_COLOR_LIGHTYELLOW MakeColor(0xFFFFE0FF)
#define SAV_COLOR_LIME MakeColor(0x00FF00FF)
#define SAV_COLOR_LIMEGREEN MakeColor(0x32CD32FF)
#define SAV_COLOR_LINEN MakeColor(0xFAF0E6FF)
#define SAV_COLOR_MAGENTA MakeColor(0xFF00FFFF)
#define SAV_COLOR_MAROON MakeColor(0x800000FF)
#define SAV_COLOR_MEDIUMAQUAMARINE MakeColor(0x66CDAAFF)
#define SAV_COLOR_MEDIUMBLUE MakeColor(0x0000CDFF)
#define SAV_COLOR_MEDIUMORCHID MakeColor(0xBA55D3FF)
#define SAV_COLOR_MEDIUMPURPLE MakeColor(0x9370DBFF)
#define SAV_COLOR_MEDIUMSEAGREEN MakeColor(0x3CB371FF)
#define SAV_COLOR_MEDIUMSLATEBLUE MakeColor(0x7B68EEFF)
#define SAV_COLOR_MEDIUMSPRINGGREEN MakeColor(0x00FA9AFF)
#define SAV_COLOR_MEDIUMTURQUOISE MakeColor(0x48D1CCFF)
#define SAV_COLOR_MEDIUMVIOLETRED MakeColor(0xC71585FF)
#define SAV_COLOR_MIDNIGHTBLUE MakeColor(0x191970FF)
#define SAV_COLOR_MINTCREAM MakeColor(0xF5FFFAFF)
#define SAV_COLOR_MISTYROSE MakeColor(0xFFE4E1FF)
#define SAV_COLOR_MOCCASIN MakeColor(0xFFE4B5FF)
#define SAV_COLOR_NAVAJOWHITE MakeColor(0xFFDEADFF)
#define SAV_COLOR_NAVY MakeColor(0x000080FF)
#define SAV_COLOR_OLDLACE MakeColor(0xFDF5E6FF)
#define SAV_COLOR_OLIVE MakeColor(0x808000FF)
#define SAV_COLOR_OLIVEDRAB MakeColor(0x6B8E23FF)
#define SAV_COLOR_ORANGE MakeColor(0xFFA500FF)
#define SAV_COLOR_ORANGERED MakeColor(0xFF4500FF)
#define SAV_COLOR_ORCHID MakeColor(0xDA70D6FF)
#define SAV_COLOR_PALEGOLDENROD MakeColor(0xEEE8AAFF)
#define SAV_COLOR_PALEGREEN MakeColor(0x98FD98FF)
#define SAV_COLOR_PALETURQUOISE MakeColor(0xAFEEEEFF)
#define SAV_COLOR_PALEVIOLETRED MakeColor(0xDB7093FF)
#define SAV_COLOR_PAPAYAWHIP MakeColor(0xFFEFD5FF)
#define SAV_COLOR_PEACHPUFF MakeColor(0xFFDAB9FF)
#define SAV_COLOR_PERU MakeColor(0xCD853FFF)
#define SAV_COLOR_PINK MakeColor(0xFFC0CDFF)
#define SAV_COLOR_PLUM MakeColor(0xDDA0DDFF)
#define SAV_COLOR_POWDERBLUE MakeColor(0xB0E0E6FF)
#define SAV_COLOR_PURPLE MakeColor(0x800080FF)
#define SAV_COLOR_RED MakeColor(0xFF0000FF)
#define SAV_COLOR_ROSYBROWN MakeColor(0xBC8F8FFF)
#define SAV_COLOR_ROYALBLUE MakeColor(0x4169E1FF)
#define SAV_COLOR_SADDLEBROWN MakeColor(0x8B4513FF)
#define SAV_COLOR_SALMON MakeColor(0xFA8072FF)
#define SAV_COLOR_SANDYBROWN MakeColor(0xF4A460FF)
#define SAV_COLOR_SEAGREEN MakeColor(0x2E8B57FF)
#define SAV_COLOR_SEASHELL MakeColor(0xFFF5EEFF)
#define SAV_COLOR_SIENNA MakeColor(0xA0522DFF)
#define SAV_COLOR_SILVER MakeColor(0xC0C0C0FF)
#define SAV_COLOR_SKYBLUE MakeColor(0x87CEEBFF)
#define SAV_COLOR_SLATEBLUE MakeColor(0x6A5ACDFF)
#define SAV_COLOR_SLATEGRAY MakeColor(0x708090FF)
#define SAV_COLOR_SLATEGREY MakeColor(0x708090FF)
#define SAV_COLOR_SNOW MakeColor(0xFFFAFAFF)
#define SAV_COLOR_SPRINGGREEN MakeColor(0x00FF7FFF)
#define SAV_COLOR_STEELBLUE MakeColor(0x4682B4FF)
#define SAV_COLOR_TAN MakeColor(0xD2B48CFF)
#define SAV_COLOR_TEAL MakeColor(0x008080FF)
#define SAV_COLOR_THISTLE MakeColor(0xD8BFD8FF)
#define SAV_COLOR_TOMATO MakeColor(0xFF6347FF)
#define SAV_COLOR_TURQUOISE MakeColor(0x40E0D0FF)
#define SAV_COLOR_SADDLEBROWN MakeColor(0x8B4513FF)
#define SAV_COLOR_VIOLET MakeColor(0xEE82EEFF)
#define SAV_COLOR_WHEAT MakeColor(0xF5DEB3FF)
#define SAV_COLOR_WHITE MakeColor(0xFFFFFFFF)
#define SAV_COLOR_WHITESMOKE MakeColor(0xF5F5F5FF)
#define SAV_COLOR_YELLOW MakeColor(0xFFFF00FF)
#define SAV_COLOR_YELLOWGREEN MakeColor(0x9ACD32FF)

#define SAV_COLOR_CROWBLACK MakeColor(0x0D0907FF)
#define SAV_COLOR_ASHGRAY MakeColor(0x666362FF)
#define SAV_COLOR_BLACKFOREST MakeColor(0x2c3227FF)
#define SAV_COLOR_CHARCOAL MakeColor(0x36454FFF)
#define SAV_COLOR_OIL MakeColor(0x3B3131FF)
#define SAV_COLOR_CARBONBLACK MakeColor(0x0C0A00FF)
#define SAV_COLOR_SABLE MakeColor(0x060606FF)
#define SAV_COLOR_MIDNIGHT MakeColor(0x2B1B17FF)
#define SAV_COLOR_DARKDARKGRAY MakeColor(0x202020FF)
// SECTION SAV header
struct GameMemory
{
    void *data;
    size_t size;
};

struct SavImage
{
    void *data;
    int w, h, pitch;
    void *_dataToFree;
};

struct SavShader
{
    // TODO: Should keep track of shader uniform locations
    u32 id;
};

struct SavTexture
{
    u32 id;
    i32 w, h;
};

struct SavTextureAtlas
{
    SavTexture texture;
    i32 cellW, cellH;
    int cellHorizontalCount, cellVerticalCount;
};

struct SavRenderTexture
{
    u32 fbo;
    u32 depthRbo;
    SavTexture texture;
};

enum TexWrapMode
{
    SAV_CLAMP_TO_EDGE,
    SAV_REPEAT
};

enum TexFilterMode
{
    SAV_LINEAR,
    SAV_NEAREST
};

#define CAMERA_MAX_ZOOM_STEPS 16

struct Camera2D
{
    v2 target;
    v2 offset;
    f32 rotation;

    b32 shouldBound;
    Rect bounds;
    v2 screenDim;
    
    f32 zoom;
    f32 zoomMin;
    f32 zoomMax;
    f32 zoomLog;
    f32 zoomLogSteps[CAMERA_MAX_ZOOM_STEPS];
    int zoomLogStepsCurrent;
    int zoomLogStepsCount;
};


struct GlyphInfo
{
    v2 glyphUVs[4];

    int minX;
    int maxX;
    int minY;
    int maxY;
    int advance;
};

struct SavFont
{
    int glyphCount;
    GlyphInfo *glyphInfos;

    u32 atlasTextureId;
    f32 pointSize;
    int height;
};

enum SavDrawMode
{
    SAV_DRAW_TRIANGLES,
    SAV_DRAW_LINES,
    SAV_DRAW_POINTS
};

enum VertexAttribType
{
    SAV_VA_TYPE_FLOAT,
    SAV_VA_TYPE_INT
};

struct VertexAttrib
{
    b32 isEnabled;
    b32 isInteger;
    int componentCount;
    VertexAttribType type;
    size_t byteSize;
    size_t byteOffset;
};

#define VERTEX_BATCH_MAX_ATTRIB 16
struct VertexBatchSpec
{
    VertexAttrib attribs[VERTEX_BATCH_MAX_ATTRIB];
    int attribMax;
    size_t vertBufferByteSize;
    int vertMax;

    int indexMax;
    size_t indexByteSize;
};

struct VertexBatch
{
    VertexBatchSpec spec;
    u32 vbo;
    u32 vao;
    u32 ebo;

    b32 dataSubStarted;
    b32 readyToDraw;
    int vertCount;
    int indexCount;
};

struct VertexCountedData
{
    void *data;
    size_t elemSize;
    size_t elemCount;
};

enum DefaultVertAttribs
{
    DEFAULT_VERT_POSITIONS = 0,
    DEFAULT_VERT_TEXCOORDS,
    DEFAULT_VERT_COLORS
};
#define DEFAULT_VERTEX_BATCH NULL

inline FourV2 ConvertRectPointsToTexCoords(SavTexture texture, FourV2 points)
{
    FourV2 result;
    f32 oneOverWidth = 1.0f / texture.w;
    f32 oneOverHeight = 1.0f / texture.h;
    for (int i = 0; i < 4; i++)
    {
        result.e[i] = V2(points.e[i].x * oneOverWidth, points.e[i].y * oneOverHeight);
    }
    return result;
}

inline void FlipTexCoords(FourV2 *texCoords)
{
    for (int i = 0; i < 4; i++)
    {
        texCoords->e[i].y = 1.0f - texCoords->e[i].y;
    }
}

inline FourV2 GetTextureRectTexCoords(SavTexture texture, Rect rect)
{
    FourV2 points = RectGetPoints(rect);
    FourV2 texCoords = ConvertRectPointsToTexCoords(texture, points);
    FlipTexCoords(&texCoords);
    return texCoords;
}

inline Rect GetAtlasSourceRect(SavTextureAtlas atlas, int i)
{
    int atlasPxX = (i % atlas.cellHorizontalCount) * atlas.cellW;
    int atlasPxY = (i / atlas.cellHorizontalCount) * atlas.cellH;
    Rect r = MakeRect((f32) atlasPxX, (f32) atlasPxY, (f32) atlas.cellW, (f32) atlas.cellH);
    return r;
}

sav_func GameMemory AllocGameMemory(size_t size);
sav_func MemoryArena AllocArena(size_t size);
sav_func MemoryArena *MemoryArenaScratch(MemoryArena *dontCollideWithThis);
sav_func void MemoryArenaResetScratch();

sav_func void InitWindow(const char *title, int width, int height);
sav_func void Quit();
sav_func void PollEvents();

sav_func void SetTargetFPS(f32 fps);
sav_func void BeginFrameTiming();
sav_func void EndFrameTiming();
sav_func u64 GetCurrentFrame();
sav_func f64 GetDeltaFixed();
sav_func f64 GetDeltaPrev();
sav_func f64 GetDeltaAvg();
sav_func f64 GetFPSPrev();
sav_func f64 GetFPSAvg();

sav_func b32 WindowShouldClose();
sav_func void SetWindowTitle(const char *title);
sav_func v2 GetWindowSize();
sav_func b32 WindowSizeChanged();
sav_func void SetWindowBorderless(b32 borderless);
sav_func void ToggleWindowBorderless();

sav_func b32 KeyDown(int key);
sav_func b32 KeyPressed(int key);
sav_func b32 KeyReleased(int key);
sav_func b32 KeyRepeat(int key);
sav_func b32 KeyPressedOrRepeat(int key);
sav_func b32 GetMouseRelativeMode();
sav_func void SetMouseRelativeMode(b32 enabled);
sav_func v2 MousePos();
sav_func v2 MouseRelPos();
sav_func b32 MouseDown(int button);
sav_func b32 MousePressed(int button);
sav_func b32 MouseReleased(int button);
sav_func b32 MouseClicks(int button, int clicks);
sav_func i32 MouseWheel();

sav_func SavShader BuildCustomShader(const char *vertPath, const char *fragPath);
sav_func void DeleteShader(SavShader *shader);
sav_func void BeginShaderMode(SavShader shader);
sav_func void EndShaderMode();
sav_func void SetShaderMatricesBindingPoint(SavShader shader, const char *uboName);
sav_func void SetUniformMat4(const char *uniformName, f32 *value);
sav_func void SetUniformVec3(const char *uniformName, f32 *value);
sav_func void SetUniformVec4(const char *uniformName, f32 *value);
sav_func void SetUniformI(const char *uniformName, int value);

sav_func void PushProjection(m4 projection);
sav_func void PushModelView(m4 modelView);
sav_func void PopProjection();
sav_func void PopModelView();

sav_func Camera2D MakeCamera(f32 rotation, v2 offset, v2 target, f32 zoomMin, f32 zoomMax, int zoomStepCount);
sav_func void CameraSetBounds(Camera2D *camera, f32 screenWidth, f32 screenHeight, f32 boundsX, f32 boundsY, f32 boundsWidth, f32 boundsHeight);
sav_func void BeginCameraMode(Camera2D *camera);
sav_func void EndCameraMode();
sav_func void CameraMoveTarget(Camera2D *camera, v2 dP);
sav_func v2 CameraWorldToScreen(Camera2D *camera, v2 world);
sav_func v2 CameraScreenToWorld(Camera2D *camera, v2 screen);
sav_func v2 CameraScreenToWorldRel(Camera2D *camera, v2 screenDelta);
sav_func v2 ScreenToRectCoords(Rect screenRect, f32 scaledW, f32 scaledH, v2 screenCoords);
sav_func v2 RectToScreenCoords(Rect screenRect, f32 scaledW, f32 scaledH, v2 rectCoords);
sav_func void CameraIncreaseLogZoom(Camera2D *camera, f32 zoomDelta);
sav_func void CameraInitLogZoomSteps(Camera2D *camera, f32 min, f32 max, int stepCount);
sav_func void CameraIncreaseLogZoomSteps(Camera2D *camera, int steps);

sav_func SavImage SavLoadImage(const char *path);
sav_func void SavFreeImage(SavImage *image);
sav_func SavTexture SavLoadTexture(const char *path);
sav_func SavTexture SavLoadTextureFromImage(SavImage image);
sav_func SavTexture SavLoadTextureFromData(void *data, int width, int height);
sav_func SavTextureAtlas SavLoadTextureAtlas(const char *path, int cellHorizontalCount, int cellHorizontalWidth);
sav_func void SavSetTextureWrapMode(SavTexture texture, TexWrapMode wrapMode);
sav_func void SavSetTextureFilterMode(SavTexture texture, TexFilterMode filterMode);
sav_func SavRenderTexture SavLoadRenderTexture(int width, int height, b32 filterNearest);
sav_func void SavDeleteRenderTexture(SavRenderTexture *renderTexture);
sav_func void BeginTextureMode(SavRenderTexture renderTexture, Rect renderTextureScreenRect);
sav_func void EndTextureMode();
sav_func void BindTextureSlot(int slot, SavTexture texture);
sav_func void UnbindTextureSlot(int slot);

sav_func VertexBatchSpec BeginVertexBatchSpec(int vertMax, int indexMax, size_t indexByteSize);
sav_func void VertexBatchSpecAddAttrib(VertexBatchSpec *spec, int attribIndex, b32 isInteger, int componentCount, VertexAttribType type, size_t byteSize);
sav_func void EndVertexBatchSpec(VertexBatchSpec *spec);
sav_func VertexBatch PrepareVertexBatch(VertexBatchSpec spec);
sav_func void VertexBatchBeginSub(VertexBatch *batch, int vertCount, int indexCount);
sav_func VertexCountedData MakeVertexCountedData(void *data, size_t elemSize, size_t elemCount);
sav_func void VertexBatchSubVertexData(VertexBatch *batch, int attribIndex, VertexCountedData data);
sav_func void VertexBatchSubIndexData(VertexBatch *batch, VertexCountedData data);
sav_func void VertexBatchEndSub(VertexBatch *batch);
sav_func void DrawVertexBatch(VertexBatch *batch);
sav_func void SetPointSize(f32 size);
sav_func void SetLineWidth(f32 size);
sav_func void SetDrawMode(SavDrawMode drawMode);

sav_func void ClearBackground(SavColor c);
sav_func void BeginDraw();
sav_func void EndDraw();
sav_func void SavSwapBuffers();
sav_func void DrawTexture(SavTexture texture, Rect dest, Rect source, v2 origin, f32 rotation, SavColor color);
sav_func void DrawRect(Rect rect, SavColor color);
sav_func void DrawAtlasCell(SavTextureAtlas atlas, int x, int y, Rect destRect, SavColor color);

sav_func SavFont SavLoadFont(const char *path, u32 pointSize);
sav_func v2 GetStringDimensions(const char *string, SavFont *font, f32 pointSize);
sav_func void DrawString(const char *string, SavFont *font, f32 pointSize, SavColor color, f32 x, f32 y, f32 maxWidth, MemoryArena *arena);
sav_func void SavFreeFont(SavFont *font);

sav_func char *SavReadTextFile(const char *path);
sav_func void SavFreeString(char **text);

sav_func const char *TextFormat(const char *format, ...);
sav_func void TraceLog(const char *format, ...);
sav_func void TraceLogPartialStart(const char *format, ...);
sav_func void TraceLogPartial(const char *format, ...);
sav_func void TraceError(const char *format, ...);
sav_func void Memset(void *ptr, int value, size_t num);
sav_func void Strcpy(char *dest, char *source);
sav_func void StringFormat(char *format, StringBuffer outputBuffer, ...);
sav_func void StringFormat(char *format, StringBuffer outputBuffer, va_list varArgs);

sav_func int GetRandomValue(int min, int max);
sav_func f32 GetRandomFloat();
sav_func b32 RandomChane(f32 chance);
sav_func v2 GetRandomVec(f32 length);

#ifdef SAV_IMPLEMENTATION

#include <windows.h>
#include <shellscalingapi.h>
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include <sdl2/SDL_ttf.h>
#include <glad/glad.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>

// SECTION SAV settings
#define TIME_STAT_AVG_COUNT 32
#define PRINT_FRAME_TIME_UTIL 0
#define STRING_BUFFER 1024
#define MVP_MATRIX_STACK_COUNT 32
#define MAX_VERTEX_BATCH_COUNT 64
#define MAX_SCRATCH_ARENAS 2
#define SCRATCH_ARENA_SIZE Megabytes(4)

// SECTION Sav internal data
struct SavState
{
    MemoryArena scratchArenas[MAX_SCRATCH_ARENAS];
};

struct SdlState
{
    SDL_Window *window;
    v2 windowSize;
    v2 windowOriginalSize;
    b32 windowSizeChanged;
    b32 shouldClose;
    b32 borderless;
    Rect windowRectBeforeBorderless;

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
    SavShader defaultShader;
    VertexBatch defaultVertexBatch;
    u32 matricesUbo;
    u32 matricesUboBindingPoint;

    SavTexture defaultTexture;

    m4 projectionStack[MVP_MATRIX_STACK_COUNT];
    m4 modelViewStack[MVP_MATRIX_STACK_COUNT];
    int projectionStackCurrent;
    int modelViewStackCurrent;

    b32 cameraModeActive;
    b32 shaderModeActive;
    b32 textureModeActive;
    b32 drawModeActive;
    b32 canDraw;

    Rect currentRenderTextureScreenRect;
    SavRenderTexture currentRenderTexture;

    SavShader currentShader;

    GLenum drawMode;
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

global_var SavState *_savState;
global_var SdlState *_sdlState;
global_var GlState *_glState;
global_var InputState *_inputState;

internal_func void traceLogEngine(const char *severity, const char *format, ...)
{
    char formatBuf[STRING_BUFFER];
    sprintf_s(formatBuf, "ENGINE: %s: [F %06zu] %s\n", severity, _sdlState->currentFrame, format);

    va_list varArgs;
    va_start(varArgs, format);
    vprintf_s(formatBuf, varArgs);
    va_end(varArgs);
}

internal_func void *win32AllocMemory(size_t size)
{
    void *memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (memory == NULL)
    {
        traceLogEngine("ERROR", "Win32: Could not VirtualAlloc (%zu bytes).\n", size);
    }
    Assert(memory);
    return memory;
}

// SECTION Memory management

sav_func GameMemory AllocGameMemory(size_t size)
{
    GameMemory gameMemory;
    gameMemory.size = size;
    gameMemory.data = win32AllocMemory(gameMemory.size);
    return gameMemory;
}

sav_func MemoryArena AllocArena(size_t size)
{
    void *allocatedMemory = win32AllocMemory(size);
    return MakeMemoryArena((u8 *)allocatedMemory, size);
}

sav_func MemoryArena *MemoryArenaScratch(MemoryArena *dontCollideWithThis)
{
    MemoryArena *scratch = _savState->scratchArenas;
    if (scratch == dontCollideWithThis)
    {
        scratch++;
    }
    if (scratch->base == NULL)
    {
        *scratch = AllocArena(SCRATCH_ARENA_SIZE);
    }

    return scratch;
}

sav_func void MemoryArenaResetScratch()
{
    for (int i = 0; i < MAX_SCRATCH_ARENAS; i++)
    {
        MemoryArenaReset(_savState->scratchArenas + i);
    }
}

// SECTION Window fundamentals

internal_func SavShader buildBasicShader();

#define DEFAULT_BATCH_MAX_VERT_COUNT 65536
#define DEFAULT_BATCH_MAX_INDEX_COUNT 393216

internal_func void initGlDefaults()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    u32 white = 0xFFFFFFFF;
    SavTexture defaultTexture = SavLoadTextureFromData(&white, 1, 1);
    _glState->defaultTexture = defaultTexture;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _glState->defaultTexture.id);

    VertexBatchSpec defaultSpec = BeginVertexBatchSpec(DEFAULT_BATCH_MAX_VERT_COUNT, DEFAULT_BATCH_MAX_INDEX_COUNT, sizeof(u32));
    VertexBatchSpecAddAttrib(&defaultSpec, DEFAULT_VERT_POSITIONS, false, 3, SAV_VA_TYPE_FLOAT, sizeof(v3));
    VertexBatchSpecAddAttrib(&defaultSpec, DEFAULT_VERT_TEXCOORDS, false, 4, SAV_VA_TYPE_FLOAT, sizeof(v4));
    VertexBatchSpecAddAttrib(&defaultSpec, DEFAULT_VERT_COLORS, false, 4, SAV_VA_TYPE_FLOAT, sizeof(v4));
    EndVertexBatchSpec(&defaultSpec);
    _glState->defaultVertexBatch = PrepareVertexBatch(defaultSpec);

    _glState->matricesUboBindingPoint = 0;
    size_t matricesUboSize = sizeof(m4);
    glGenBuffers(1, &_glState->matricesUbo);
    Assert(_glState->matricesUbo);
    glBindBuffer(GL_UNIFORM_BUFFER, _glState->matricesUbo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(m4), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, _glState->matricesUboBindingPoint, _glState->matricesUbo);

    _glState->defaultShader = buildBasicShader();
    _glState->currentShader = _glState->defaultShader;
    glUseProgram(_glState->currentShader.id);
    SetShaderMatricesBindingPoint(_glState->defaultShader, "Matrices");

    _glState->projectionStackCurrent = -1;
    _glState->modelViewStackCurrent = -1;
    PushProjection(M4(1));
    PushModelView(M4(1));

    _glState->drawMode = GL_TRIANGLES;

    SDL_GL_SetSwapInterval(0);
}

sav_func void InitWindow(const char *title, int width, int height)
{
    // TODO: Let user provide memory (maybe optionally!)
    //       Also use arena here and don't use crt allocation?
    _savState = (SavState *)calloc(1, sizeof(SavState));
    _sdlState = (SdlState *)calloc(1, sizeof(SdlState));
    _glState = (GlState *)calloc(1, sizeof(GlState));
    _inputState = (InputState *)calloc(1, sizeof(InputState));

    if (_savState == NULL || _sdlState == NULL || _glState == NULL || _inputState == NULL)
    {
        traceLogEngine("ERROR", "Failed to allocate memory for sav state.");
        InvalidCodePath;
        return;
    }

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        _sdlState->window = SDL_CreateWindow(title,
                                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                            width, height,
                                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        if (_sdlState->window)
        {
            int sdlImageFlags = IMG_INIT_JPG | IMG_INIT_PNG;
            int imgInitResult = IMG_Init(sdlImageFlags);
            if (!(imgInitResult & sdlImageFlags))
            {
                traceLogEngine("ERROR", "Failed to initialize SDL IMG.");
                InvalidCodePath;
                return;
            }

            if (TTF_Init() != 0)
            {
                traceLogEngine("ERROR", "SDL failed to init SDL_ttf.");
                InvalidCodePath;
                return;
            }

            int actualWidth, actualHeight;
            SDL_GetWindowSize(_sdlState->window, &actualWidth, &actualHeight);
            _sdlState->windowSize = V2((f32) actualWidth, (f32) actualHeight);
            _sdlState->windowOriginalSize = _sdlState->windowSize;

            SDL_GLContext glContext = SDL_GL_CreateContext(_sdlState->window);

            if (glContext)
            {
                gladLoadGLLoader(SDL_GL_GetProcAddress);
                traceLogEngine("INFO", "OpenGL loaded");
                traceLogEngine("INFO", "Vendor: %s", glGetString(GL_VENDOR));
                traceLogEngine("INFO", "Renderer: %s", glGetString(GL_RENDERER));
                traceLogEngine("INFO", "Version: %s", glGetString(GL_VERSION));

                _sdlState->perfCounterFreq = SDL_GetPerformanceFrequency();

                TIMECAPS devCaps;
                MMRESULT devCapsGetResult = timeGetDevCaps(&devCaps, sizeof(devCaps));
                if (devCapsGetResult == MMSYSERR_NOERROR)
                {
                    traceLogEngine("INFO", "Available DevCaps range: [%u, %u] ms", devCaps.wPeriodMin, devCaps.wPeriodMax);
                }

                _sdlState->desiredSchedulerMs = 1;
                _sdlState->sleepIsGranular = (timeBeginPeriod(_sdlState->desiredSchedulerMs) == TIMERR_NOERROR);
                traceLogEngine("INFO", "Sleep is granular: %d", _sdlState->sleepIsGranular);

                srand((unsigned) time(NULL));

                initGlDefaults();

                // Setup Dear ImGui context
                IMGUI_CHECKVERSION();
                ImGui::CreateContext();
                ImGuiIO& io = ImGui::GetIO();
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

                // Setup Platform/Renderer backends
                ImGui_ImplSDL2_InitForOpenGL(_sdlState->window, glContext);
                ImGui_ImplOpenGL3_Init();
            }
            else
            {
                traceLogEngine("ERROR", "Failed to initialize GL context.");
                InvalidCodePath;
                return;
            }
        }
        else
        {
            traceLogEngine("ERROR", "Failed to initialize SDL window.");
            InvalidCodePath;
            return;
        }
    }
    else
    {
        traceLogEngine("ERROR", "Failed to initialize SDL.");
        InvalidCodePath;
        return;
    }
}

sav_func void Quit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
 
    if (_sdlState->sleepIsGranular)
    {
        timeEndPeriod(_sdlState->desiredSchedulerMs);
    }

    if (_sdlState->window)
    {
        SDL_DestroyWindow(_sdlState->window);
    }

    SDL_Quit();
}

sav_func void PollEvents()
{
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        _inputState->repeatKeyStates[i] = 0;

        _inputState->previousKeyStates[i] = _inputState->currentKeyStates[i];
    }

    for (int i = 0; i < SDL_BUTTON_X2 + 1; i++)
    {
        _inputState->clickMouseButtonStates[i] = 0;

        _inputState->previousMouseButtonStates[i] = _inputState->currentMouseButtonStates[i];
    }

    _inputState->mouseWheel = 0;

    _sdlState->windowSizeChanged = false;

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
            {
                _sdlState->shouldClose = true;
            } break;

            case SDL_WINDOWEVENT:
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    _sdlState->windowSize = V2((f32) event.window.data1, (f32) event.window.data2);
                    _sdlState->windowSizeChanged = true;
                }
            } break;

            default: break;
        }

        if (!ImGui::GetIO().WantCaptureKeyboard)
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                {
                    _inputState->currentKeyStates[event.key.keysym.scancode] = (event.type == SDL_KEYDOWN);
                    _inputState->repeatKeyStates[event.key.keysym.scancode] = event.key.repeat;
                } break;
            }
        }

        if (!ImGui::GetIO().WantCaptureMouse)
        {
            switch (event.type)
            {
                case SDL_MOUSEMOTION:
                {
                    // NOTE: It seems it's better to update mouse position every frame
                } break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                {
                    _inputState->currentMouseButtonStates[event.button.button] = (event.type == SDL_MOUSEBUTTONDOWN);
                    if (event.type == SDL_MOUSEBUTTONDOWN)
                    {
                        _inputState->clickMouseButtonStates[event.button.button] = event.button.clicks;
                    }
                } break;
                case SDL_MOUSEWHEEL:
                {
                    // TODO: Maybe deal with event.wheel.direction field on other platforms
                    _inputState->mouseWheel += event.wheel.y; // NOTE: Add y, because it's likely there were more than one event between frames
                } break;

                default: break;
            }
        }
    }

    int mouseX, mouseY, mouseRelX, mouseRelY;
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);
    _inputState->mousePos = V2((f32) mouseX, (f32) mouseY);
    _inputState->mouseRelPos = V2((f32) mouseRelX, (f32) mouseRelY);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

// Timing
sav_func void SetTargetFPS(f32 fps)
{
    _sdlState->limitFps = true;
    _sdlState->targetFps = fps;
    _sdlState->targetDelta = 1.0 / _sdlState->targetFps;
    traceLogEngine("INFO", "Target FPS: %.0f (%.6f ms)", _sdlState->targetFps, _sdlState->targetDelta);
}

internal_func inline f64 getAvgDelta(f64 *samples, int sampleCount)
{
    f64 accum = 0.0f;
    for (int i = 0; i < sampleCount; i++)
    {
        accum += samples[i];
    }
    return accum / TIME_STAT_AVG_COUNT;
}

sav_func void BeginFrameTiming()
{
    if (_sdlState->lastCounter != 0)
    {
        u64 currentCounter = SDL_GetPerformanceCounter();
        u64 counterElapsed = currentCounter - _sdlState->lastCounter;
        _sdlState->lastCounter = currentCounter;
        _sdlState->prevDelta = (f64) counterElapsed / _sdlState->perfCounterFreq;

        _sdlState->deltaSamples[_sdlState->currentTimeStatSample++] = _sdlState->prevDelta;
        if (_sdlState->currentTimeStatSample >= TIME_STAT_AVG_COUNT)
        {
            _sdlState->avgDelta = getAvgDelta(_sdlState->deltaSamples, TIME_STAT_AVG_COUNT);
            _sdlState->currentTimeStatSample = 0;
        }
    }
    else
    {
        _sdlState->lastCounter = SDL_GetPerformanceCounter();
    }

    _sdlState->currentFrame++;
}

sav_func void EndFrameTiming()
{
    if (_sdlState->limitFps)
    {
        u64 counterElapsed = SDL_GetPerformanceCounter() - _sdlState->lastCounter;
        f64 elapsedMs = (f64) counterElapsed / _sdlState->perfCounterFreq;

        #if (PRINT_FRAME_TIME_UTIL == 1)
        f64 frameTimeUtilization = elapsedMs / _sdlState->targetDelta;
        traceLogEngine("INFO", "Frame time utilization: %f", frameTimeUtilization * 100.0);
        #endif

        u64 targetElapsed = (u64) (_sdlState->targetDelta * _sdlState->perfCounterFreq);

        int sleepForMs = (int) (1000.0 * (_sdlState->targetDelta - elapsedMs)) - 2;
        if (sleepForMs > 1)
        {
            Sleep((DWORD) sleepForMs);

            counterElapsed = SDL_GetPerformanceCounter() - _sdlState->lastCounter;
            if (counterElapsed > targetElapsed)
            {
                traceLogEngine("WARNING", "SLEEP MISSED TARGET BY %f SEC", (f64) (counterElapsed - targetElapsed) / _sdlState->perfCounterFreq);
            }
        }

        while (counterElapsed < targetElapsed)
        {
            counterElapsed = SDL_GetPerformanceCounter() - _sdlState->lastCounter;
        }
    }
}

sav_func u64 GetCurrentFrame()
{
    return _sdlState->currentFrame;
}

sav_func f64 GetDeltaFixed()
{
    return _sdlState->targetDelta;
}

sav_func f64 GetDeltaPrev()
{
    return _sdlState->prevDelta;
}

sav_func f64 GetDeltaAvg()
{
    return _sdlState->avgDelta;
}

sav_func f64 GetFPSPrev()
{
    if (_sdlState->prevDelta > 0.0)
    {
        return 1.0 / _sdlState->prevDelta;
    }
    else
    {
        return 0.0;
    }
}

sav_func f64 GetFPSAvg()
{
    if (_sdlState->avgDelta > 0.0)
    {
        return 1.0 / _sdlState->avgDelta;
    }
    else
    {
        return 0.0;
    }
}

// Window util
sav_func b32 WindowShouldClose()
{
    return _sdlState->shouldClose;
}

sav_func void SetWindowTitle(const char *title)
{
    SDL_SetWindowTitle(_sdlState->window, title);
}

sav_func v2 GetWindowSize()
{
    return _sdlState->windowSize;
}

sav_func b32 WindowSizeChanged()
{
    return _sdlState->windowSizeChanged;
}

sav_func void SetWindowBorderless(b32 borderless)
{
    if (borderless)
    {
        int x, y;
        SDL_GetWindowPosition(_sdlState->window, &x, &y);
        _sdlState->windowRectBeforeBorderless = MakeRect((f32) x, (f32) y, _sdlState->windowSize.x, _sdlState->windowSize.y);
    }
    
    SDL_SetWindowBordered(_sdlState->window, (SDL_bool) !borderless);
    
    if (borderless)
    {
        SDL_MaximizeWindow(_sdlState->window);
    }
    else
    {
        SDL_RestoreWindow(_sdlState->window);

        // TODO: This is very hacky, maybe there's a better way to do this with SDL
        if (_sdlState->windowRectBeforeBorderless.w > 0)
        {
            SDL_SetWindowSize(_sdlState->window, (int) _sdlState->windowRectBeforeBorderless.w, (int) _sdlState->windowRectBeforeBorderless.h);
            SDL_SetWindowPosition(_sdlState->window, (int) _sdlState->windowRectBeforeBorderless.x, (int) _sdlState->windowRectBeforeBorderless.y);
            _sdlState->windowRectBeforeBorderless = {};
        }
    }
}

sav_func void ToggleWindowBorderless()
{
    _sdlState->borderless = !_sdlState->borderless;
    SetWindowBorderless(_sdlState->borderless);
}

// Input
sav_func b32 KeyDown(int key)
{
    return (b32) _inputState->currentKeyStates[key];
}

sav_func b32 KeyPressed(int key)
{
    return (b32) (_inputState->currentKeyStates[key] && !_inputState->previousKeyStates[key]);
}

sav_func b32 KeyReleased(int key)
{
    return (b32) (!_inputState->currentKeyStates[key] && _inputState->previousKeyStates[key]);
}

sav_func b32 KeyRepeat(int key)
{
    return (b32) _inputState->repeatKeyStates[key];
}

sav_func b32 KeyPressedOrRepeat(int key)
{
    return (b32) ((_inputState->currentKeyStates[key] && !_inputState->previousKeyStates[key]) || _inputState->repeatKeyStates[key]);
}

sav_func b32 GetMouseRelativeMode()
{
    return (b32) SDL_GetRelativeMouseMode();
}

sav_func void SetMouseRelativeMode(b32 enabled)
{
    _inputState->isRelMouse = enabled;
    SDL_SetRelativeMouseMode((SDL_bool) _inputState->isRelMouse);
}

sav_func v2 MousePos()
{
    return _inputState->mousePos;
}

sav_func v2 MouseRelPos()
{
    return _inputState->mouseRelPos;
}

sav_func b32 MouseDown(int button)
{
    return (b32) _inputState->currentMouseButtonStates[button];
}

sav_func b32 MousePressed(int button)
{
    return (b32) (_inputState->currentMouseButtonStates[button] && !_inputState->previousMouseButtonStates[button]);
}

sav_func b32 MouseReleased(int button)
{
    return (b32) (!_inputState->currentMouseButtonStates[button] && _inputState->previousMouseButtonStates[button]);
}

sav_func b32 MouseClicks(int button, int clicks)
{
    return (b32) (_inputState->clickMouseButtonStates[button] == clicks);
}

sav_func i32 MouseWheel()
{
    return _inputState->mouseWheel;
}

// SECTION: Graphics: shaders
internal_func u32 buildShadersFromStr(const char *vertSource, const char *fragSource)
{
    u32 vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSource, NULL);
    glCompileShader(vertShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        traceLogEngine("ERROR", "Vertex shader compilation error:\n%s\n\n", infoLog);
    }

    u32 fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        traceLogEngine("ERROR", "Fragment shader compilation error:\n%s\n\n", infoLog);
    }

    u32 program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        traceLogEngine("ERROR", "Linking error:\n%s\n\n", infoLog);
    }
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

internal_func SavShader buildBasicShader()
{
    traceLogEngine("INFO", "Building basic shader program");

    const char *vertSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 vertPosition;\n"
        "layout (location = 1) in vec4 vertTexCoord;\n"
        "layout (location = 2) in vec4 vertColor;\n"
        "out vec4 fragTexCoord;\n"
        "out vec4 fragColor;\n"
        "layout (std140) uniform Matrices\n"
        "{\n"
        "   mat4 mvp;\n"
        "};\n"
        "void main()\n"
        "{\n"
        "   fragTexCoord = vertTexCoord;\n"
        "   fragColor = vertColor;\n"
        "   gl_Position = mvp * vec4(vertPosition, 1.0);\n"
        "}\0";

    const char *fragSource =
        "#version 330 core\n"
        "in vec4 fragTexCoord;\n"
        "in vec4 fragColor;\n"
        "out vec4 finalColor;\n"
        "uniform sampler2D texture0;\n"
        "void main()\n"
        "{\n"
        "   vec4 texelColor = texture(texture0, fragTexCoord.xy);\n"
        "   finalColor = fragColor * texelColor;\n"
        "}\n\0";

    u32 shaderID = buildShadersFromStr(vertSource, fragSource);

    SavShader result;
    result.id = shaderID;
    return result;
}

sav_func SavShader BuildCustomShader(const char *vertPath, const char *fragPath)
{
    traceLogEngine("INFO", "Building custom shader program with shaders: %s, %s", vertPath, fragPath);
    
    char *vertSource = SavReadTextFile(vertPath);
    char *fragSource = SavReadTextFile(fragPath);

    u32 shaderID = buildShadersFromStr(vertSource, fragSource);

    SavFreeString(&vertSource);
    SavFreeString(&fragSource);

    SavShader result;
    result.id = shaderID;

    return result;
}

sav_func void DeleteShader(SavShader *shader)
{
    glDeleteProgram(shader->id);
    shader->id = 0;
}

sav_func void BeginShaderMode(SavShader shader)
{
    Assert(!_glState->shaderModeActive);
    _glState->currentShader = shader;
    glUseProgram(_glState->currentShader.id);
    _glState->shaderModeActive = true;
}

sav_func void EndShaderMode()
{
    Assert(_glState->shaderModeActive);
    _glState->currentShader = _glState->defaultShader;
    glUseProgram(_glState->currentShader.id);
    _glState->shaderModeActive = false;
}

sav_func void SetShaderMatricesBindingPoint(SavShader shader, const char *uboName)
{
    u32 blockIndex = glGetUniformBlockIndex(shader.id, uboName);
    glUniformBlockBinding(shader.id, blockIndex, _glState->matricesUboBindingPoint);
}

internal_func int getUniformLocation(u32 shader, const char *uniformName)
{
    int uniformLocation = glGetUniformLocation(_glState->currentShader.id, uniformName);

    #ifdef SAV_DEBUG
    if (uniformLocation == -1)
    {
        GLenum error = glGetError();
        TraceLog("Failed to get uniform \"%s\" for ShaderID %d. Error code: %d", UniformName, shader, error);
    }
    #endif

    return uniformLocation;
}

sav_func void SetUniformMat4(const char *uniformName, f32 *value)
{
    int uniformLocation = getUniformLocation(_glState->currentShader.id, uniformName);
    glUniformMatrix4fv(uniformLocation, 1, false, value);
}

sav_func void SetUniformVec3(const char *uniformName, f32 *value)
{
    int uniformLocation = getUniformLocation(_glState->currentShader.id, uniformName);
    glUniform3fv(uniformLocation, 1, value);
}

sav_func void SetUniformVec4(const char *uniformName, f32 *value)
{
    int uniformLocation = getUniformLocation(_glState->currentShader.id, uniformName);
    glUniform4fv(uniformLocation, 1, value);
}

sav_func void SetUniformI(const char *uniformName, int value)
{
    int uniformLocation = getUniformLocation(_glState->currentShader.id, uniformName);
    glUniform1i(uniformLocation, value);
}

// SECTION Graphics: MVP, Camera
internal_func inline void setCurrentMvp()
{
    m4 mvp = _glState->projectionStack[_glState->projectionStackCurrent] * _glState->modelViewStack[_glState->modelViewStackCurrent];
    
    glBindBuffer(GL_UNIFORM_BUFFER, _glState->matricesUbo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m4), &mvp);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

sav_func void PushProjection(m4 projection)
{
    Assert(_glState->projectionStackCurrent < MVP_MATRIX_STACK_COUNT - 2);
    _glState->projectionStack[++_glState->projectionStackCurrent] = projection;
    setCurrentMvp();
}

sav_func void PushModelView(m4 modelView)
{
    Assert(_glState->modelViewStackCurrent < MVP_MATRIX_STACK_COUNT - 2);
    _glState->modelViewStack[++_glState->modelViewStackCurrent] = modelView;
    setCurrentMvp();
}

sav_func void PopProjection()
{
    Assert(_glState->projectionStackCurrent > 0);
    --_glState->projectionStackCurrent;
    setCurrentMvp();
}

sav_func void PopModelView()
{
    Assert(_glState->modelViewStackCurrent > 0);
    --_glState->modelViewStackCurrent;
    setCurrentMvp();
}

sav_func Camera2D MakeCamera(f32 rotation, v2 offset, v2 target, f32 zoomMin, f32 zoomMax, int zoomStepCount)
{
    Camera2D camera = {};
    camera.rotation = rotation;
    camera.offset = offset;
    camera.target = target;

    CameraInitLogZoomSteps(&camera, zoomMin, zoomMax, zoomStepCount);
    return camera;
}

internal_func void cameraApplyBounds(Camera2D *camera)
{
    if (camera->shouldBound && camera->zoom >= 1.0f)
    {
        f32 halfWidth = camera->screenDim.x * 0.5f / camera->zoom;
        f32 halfHeight = camera->screenDim.y * 0.5f / camera->zoom;
    
        f32 minX = camera->bounds.x + halfWidth;
        f32 maxX = camera->bounds.x + camera->bounds.w - halfWidth;
        f32 minY = camera->bounds.y + halfHeight;
        f32 maxY = camera->bounds.y + camera->bounds.h - halfHeight;

        if (camera->target.x < minX)
        {
            camera->target.x = minX;
        }
        if (camera->target.x > maxX)
        {
            camera->target.x = maxX;
        }
        if (camera->target.y < minY)
        {
            camera->target.y = minY;
        }
        if (camera->target.y > maxY)
        {
            camera->target.y = maxY;
        }
    }
}

sav_func void CameraSetBounds(Camera2D *camera, f32 screenWidth, f32 screenHeight, f32 boundsX, f32 boundsY, f32 boundsWidth, f32 boundsHeight)
{
    camera->shouldBound = true;
    camera->bounds = MakeRect(boundsX, boundsY, boundsWidth, boundsHeight);
    camera->screenDim = V2(screenWidth, screenHeight);
    cameraApplyBounds(camera);
}

sav_func void BeginCameraMode(Camera2D *camera)
{
    Assert(!_glState->cameraModeActive);
    PushModelView(GetCamera2DView(camera->target, camera->zoom, camera->rotation, camera->offset));
    _glState->cameraModeActive = true;
}

sav_func void EndCameraMode()
{
    Assert(_glState->cameraModeActive);
    PopModelView();
    _glState->cameraModeActive = false;
}

sav_func void CameraMoveTarget(Camera2D *camera, v2 dP)
{
    camera->target -= dP;
    cameraApplyBounds(camera);
}

sav_func v2 CameraWorldToScreen(Camera2D *camera, v2 world)
{
    m4 view = GetCamera2DView(camera->target, camera->zoom, camera->rotation, camera->offset);
    v4 result = view * V4(world.x, world.y, 0.0f, 1.0f);
    return V2(result.x, result.y);
}

sav_func v2 CameraScreenToWorld(Camera2D *camera, v2 screen)
{
    m4 viewInv = GetCamera2DViewInv(camera->target, camera->zoom, camera->rotation, camera->offset);
    v4 result = viewInv * V4(screen.x, screen.y, 0.0f, 1.0f);
    return V2(result.x, result.y);
}

sav_func v2 CameraScreenToWorldRel(Camera2D *camera, v2 screenDelta)
{
    m4 viewInvRel = GetCamera2DViewInvRel(camera->zoom, camera->rotation);
    v4 result = viewInvRel * V4(screenDelta.x, screenDelta.y, 0.0f, 1.0f);
    return V2(result.x, result.y);
}

sav_func v2 ScreenToRectCoords(Rect screenRect, f32 scaledW, f32 scaledH, v2 screenCoords)
{
    v2 rectCoords = V2(((screenCoords.x - screenRect.x) / screenRect.w) * scaledW,
        ((screenCoords.y - screenRect.y) / screenRect.h) * scaledH);
    return rectCoords;
}

sav_func v2 RectToScreenCoords(Rect screenRect, f32 scaledW, f32 scaledH, v2 rectCoords)
{
    v2 screenCoords = V2((rectCoords.x / scaledW) * screenRect.w + screenRect.x,
        (rectCoords.y / scaledH) * screenRect.h + screenRect.y);
    return screenCoords;
}

sav_func void CameraIncreaseLogZoom(Camera2D *camera, f32 zoomDelta)
{
    camera->zoomLog += zoomDelta;
    if (camera->zoomLog > 1.0f)
    {
        camera->zoomLog = 1.0f;
    }
    else if (camera->zoomLog < 0.0f)
    {
        camera->zoomLog = 0.0f;
    }
    camera->zoom = ExponentialInterpolation(camera->zoomMin, camera->zoomMax, camera->zoomLog);
}

sav_func void CameraInitLogZoomSteps(Camera2D *camera, f32 min, f32 max, int stepCount)
{
    Assert(stepCount > 1 && stepCount < CAMERA_MAX_ZOOM_STEPS);
    f32 stepDelta = 1.0f / (stepCount - 1);
    f32 logZoomNeutral = ExponentialInterpolationWhereIs(min, max, 1.0f);

    int iClosestToNeutral = 0;
    f32 distToNeutral = FLT_MAX;

    f32 currentDelta = 0.0f;
    for (int i = 0; i < stepCount; i++)
    {
        f32 currDistToNeutral = fabs(currentDelta - logZoomNeutral);
        if (currDistToNeutral < distToNeutral)
        {
            distToNeutral = currDistToNeutral;
            iClosestToNeutral = i;
        }

        camera->zoomLogSteps[camera->zoomLogStepsCount++] = currentDelta;
        currentDelta += stepDelta;
    }

    camera->zoomMin = min;
    camera->zoomMax = max;
    camera->zoomLogStepsCurrent = iClosestToNeutral + 1;
    camera->zoomLogSteps[iClosestToNeutral] = logZoomNeutral;
    CameraIncreaseLogZoomSteps(camera, 0);
}

sav_func void CameraIncreaseLogZoomSteps(Camera2D *camera, int steps)
{
    camera->zoomLogStepsCurrent += steps;
    if (camera->zoomLogStepsCurrent < 0) camera->zoomLogStepsCurrent = 0;
    if (camera->zoomLogStepsCurrent > (camera->zoomLogStepsCount - 1)) camera->zoomLogStepsCurrent = (camera->zoomLogStepsCount - 1);
    camera->zoom = ExponentialInterpolation(camera->zoomMin, camera->zoomMax, camera->zoomLogSteps[camera->zoomLogStepsCurrent]);

    cameraApplyBounds(camera);
}

// SECTION: Image/texture loading
sav_func SavImage SavLoadImage(const char *path)
{
    SDL_Surface *originalSurface = IMG_Load(path);
    if (!originalSurface)
    {
        const char *error = SDL_GetError();
        traceLogEngine("ERROR", "Could not load image at %s:\n%s", path, error);
        InvalidCodePath;
    }
    else
    {
        traceLogEngine("INFO", "Loaded image at %s", path);
    }

    SDL_Surface *rgbaSurface = SDL_ConvertSurfaceFormat(originalSurface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(originalSurface);

    if (rgbaSurface->pitch != rgbaSurface->format->BytesPerPixel * rgbaSurface->w)
    {
        traceLogEngine("ERROR", "SDL Loaded image, but format could not be converted.");
        InvalidCodePath;
    }

    // NOTE: Flip rows of pixels, because opengl expects textures to have bottom rows first
    {
        SDL_Surface *surface = rgbaSurface;
        char *pixels = (char *) surface->pixels;
        for (int y = 0; y < surface->h / 2; y++)
        {
            for (int x = 0; x < surface->pitch; x++)
            {
                int oppY = surface->h - y  - 1;
                char temp = pixels[oppY * surface->pitch + x];
                pixels[oppY * surface->pitch + x] = pixels[y * surface->pitch + x];
                pixels[y * surface->pitch + x] = temp;
            }
        }
    }

    SavImage image = {};
    image.data = rgbaSurface->pixels;
    image.w = rgbaSurface->w;
    image.h = rgbaSurface->h;
    image.pitch = rgbaSurface->pitch;
    image._dataToFree = rgbaSurface;
    return image;
}

sav_func void SavFreeImage(SavImage *image)
{
    SDL_FreeSurface((SDL_Surface *) image->_dataToFree);
    image->data = 0;
    image->_dataToFree = 0;
}

sav_func void SavSaveImage(const char *path, void *data, int width, int height, b32 flip, u32 rMask, u32 gMask, u32 bMask, u32 aMask)
{
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data,
                                                    width,
                                                    height,
                                                    32,
                                                    width * 4,
                                                    rMask,
                                                    gMask,
                                                    bMask,
                                                    aMask);

    // NOTE: Flip rows of pixels. Opengl gives pixels starting from left bottom corner
    if (flip)
    {
        char *pixels = (char *) surface->pixels;
        for (int y = 0; y < surface->h / 2; y++)
        {
            for (int x = 0; x < surface->pitch; x++)
            {
                int oppY = surface->h - y  - 1;
                char Temp = pixels[oppY * surface->pitch + x];
                pixels[oppY * surface->pitch + x] = pixels[y * surface->pitch + x];
                pixels[y * surface->pitch + x] = Temp;
            }
        }
    }
    
    if (IMG_SavePNG(surface, path) != 0)
    {
        TraceLog("SDL failed to save image to %s", path);
    }
}

sav_func SavTexture SavLoadTexture(const char *path)
{
    SavImage image = SavLoadImage(path);
    SavTexture texture = SavLoadTextureFromImage(image);
    SavFreeImage(&image);
    return texture;
}

sav_func SavTexture SavLoadTextureFromImage(SavImage image)
{
    return SavLoadTextureFromData(image.data, image.w, image.h);
}

sav_func SavTexture SavLoadTextureFromData(void *data, int width, int height)
{
    u32 id;
    glGenTextures(1, &id);
    Assert(id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, data);

    // TODO: Set these dynamically
    // glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // ---------
    glBindTexture(GL_TEXTURE_2D, _glState->defaultTexture.id);

    SavTexture texture = {};
    texture.id = id;
    texture.w = width;
    texture.h = height;
    return texture;
}

sav_func SavTextureAtlas SavLoadTextureAtlas(const char *path, int cellHorizontalCount, int cellVerticalCount)
{
    SavTextureAtlas result;
    result.texture = SavLoadTexture(path);
    result.cellHorizontalCount = cellHorizontalCount;
    result.cellVerticalCount = cellVerticalCount;
    result.cellW = result.texture.w / cellHorizontalCount;
    result.cellH = result.texture.h / cellVerticalCount;
    return result;
}

sav_func void SavSetTextureWrapMode(SavTexture texture, TexWrapMode wrapMode)
{
    glBindTexture(GL_TEXTURE_2D, texture.id);
    switch (wrapMode)
    {
        case SAV_CLAMP_TO_EDGE:
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } break;

        case SAV_REPEAT:
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        } break;

        default: break;
    }
    glBindTexture(GL_TEXTURE_2D, _glState->defaultTexture.id);
}

sav_func void SavSetTextureFilterMode(SavTexture texture, TexFilterMode filterMode)
{
    glBindTexture(GL_TEXTURE_2D, texture.id);
    switch (filterMode)
    {
        case SAV_LINEAR:
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } break;

        case SAV_NEAREST:
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        } break;

        default: break;
    }
    glBindTexture(GL_TEXTURE_2D, _glState->defaultTexture.id);
}

sav_func SavRenderTexture SavLoadRenderTexture(int width, int height, b32 filterNearest)
{
    u32 fbo;
    u32 texture;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    GLenum filterType = filterNearest ? GL_NEAREST : GL_LINEAR;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    u32 depthRbo;
    glGenRenderbuffers(1, &depthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width,  height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        traceLogEngine("ERROR", "GL Framebuffer is not complete.");
        InvalidCodePath;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, _glState->defaultTexture.id);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    SavRenderTexture renderTexture;
    renderTexture.fbo = fbo;
    renderTexture.depthRbo = depthRbo;
    renderTexture.texture.id = texture;
    renderTexture.texture.w = width;
    renderTexture.texture.h = height;
    return renderTexture;
}

sav_func void SavDeleteRenderTexture(SavRenderTexture *renderTexture)
{
    glDeleteTextures(1, &renderTexture->texture.id);
    glDeleteRenderbuffers(1, &renderTexture->depthRbo);
    glDeleteFramebuffers(1, &renderTexture->fbo);

    renderTexture->fbo = 0;
    renderTexture->depthRbo = 0;
    renderTexture->texture.id = 0;
    renderTexture->texture.w = 0;
    renderTexture->texture.h = 0;
}

sav_func void BeginTextureMode(SavRenderTexture renderTexture, Rect renderTextureScreenRect)
{
    Assert(!_glState->textureModeActive);
    Assert(!_glState->drawModeActive);
    glBindFramebuffer(GL_FRAMEBUFFER, renderTexture.fbo);
    glViewport(0, 0, renderTexture.texture.w, renderTexture.texture.h);
    PushProjection(GetOrthographicProjection(0.0f,
                                             (f32) renderTexture.texture.w,
                                             (f32) renderTexture.texture.h,
                                             0.0f, -1.0f, 1.0f));
    _glState->textureModeActive = true;
    _glState->currentRenderTextureScreenRect = renderTextureScreenRect;
    _glState->currentRenderTexture = renderTexture;
    _glState->canDraw = true;
}

sav_func void EndTextureMode()
{
    Assert(_glState->textureModeActive);
    PopProjection(); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _glState->textureModeActive = false;
    _glState->canDraw = false;
}

sav_func void BindTextureSlot(int slot, SavTexture texture)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

sav_func void UnbindTextureSlot(int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, _glState->defaultTexture.id);
}

// SECTION Graphics: batch
sav_func VertexBatchSpec BeginVertexBatchSpec(int vertMax, int indexMax, size_t indexByteSize)
{
    VertexBatchSpec spec = {};
    spec.attribMax = VERTEX_BATCH_MAX_ATTRIB;
    spec.vertMax = vertMax;
    spec.indexMax = indexMax;
    spec.indexByteSize = indexByteSize;
    return spec;
}

sav_func void VertexBatchSpecAddAttrib(VertexBatchSpec *spec, int attribIndex, b32 isInteger, int componentCount, VertexAttribType type, size_t byteSize)
{
    Assert(spec != NULL);
    Assert(spec->vertMax > 0);
    Assert(attribIndex >= 0 && attribIndex < spec->attribMax);
    Assert(!spec->attribs[attribIndex].isEnabled);
    Assert(componentCount > 0);
    Assert(byteSize > 0);

    VertexAttrib *attrib = spec->attribs + attribIndex;
    attrib->isEnabled = true;
    attrib->isInteger = isInteger;
    attrib->componentCount = componentCount;
    attrib->type = type;
    attrib->byteSize = byteSize;
}

sav_func void EndVertexBatchSpec(VertexBatchSpec *spec)
{
    size_t byteOffset = 0;
    for (int i = 0; i < spec->attribMax; i++)
    {
        VertexAttrib *attrib = spec->attribs + i;
        if (attrib->isEnabled)
        {
            attrib->byteOffset = byteOffset;
            byteOffset += attrib->byteSize * spec->vertMax;
        }
    }
    spec->vertBufferByteSize = byteOffset;
}

internal_func void genBuffers(VertexBatch *batch, b32 genEbo)
{
    glGenVertexArrays(1, &batch->vao);
    Assert(batch->vao);
    glGenBuffers(1, &batch->vbo);
    Assert(batch->vbo);
    if (genEbo)
    {
        glGenBuffers(1, &batch->ebo);
        Assert(batch->ebo);
    }
    else
    {
        batch->ebo = 0;
    }
}

internal_func void bindBatch(VertexBatch *batch)
{
    if (batch != NULL)
    {
        glBindVertexArray(batch->vao);
        glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
        if (batch->ebo != 0)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->ebo);
        }
    }
    else
    {
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

internal_func GLenum getGlAttribType(VertexAttribType type)
{
    switch (type)
    {
        case SAV_VA_TYPE_FLOAT: return GL_FLOAT;
        case SAV_VA_TYPE_INT: return GL_INT;
        default: InvalidCodePath; return 0;
    }
}

internal_func void configureGlVertAttribs(VertexBatchSpec spec)
{
    for (int i = 0; i < spec.attribMax; i++)
    {
        VertexAttrib *attrib = spec.attribs + i;
        if (attrib->isEnabled)
        {
            if (!attrib->isInteger)
            {
                glVertexAttribPointer(i, attrib->componentCount, getGlAttribType(attrib->type), GL_FALSE, attrib->byteSize, (void *)attrib->byteOffset);
            }
            else
            {
                glVertexAttribIPointer(i, attrib->componentCount, getGlAttribType(attrib->type), attrib->byteSize, (void *)attrib->byteOffset);
            }
            glEnableVertexAttribArray(i);
        }
    }
}

sav_func VertexBatch PrepareVertexBatch(VertexBatchSpec spec)
{
    VertexBatch batch = {};
    batch.spec = spec;
    b32 genEbo = (spec.indexMax > 0);
    genBuffers(&batch, genEbo);
    bindBatch(&batch);
    glBufferData(GL_ARRAY_BUFFER, spec.vertBufferByteSize, NULL, GL_DYNAMIC_DRAW);
    if (genEbo)
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, spec.indexMax * spec.indexByteSize, NULL, GL_DYNAMIC_DRAW);
    }
    configureGlVertAttribs(spec);
    bindBatch(NULL);
    return batch;
}

sav_func void VertexBatchBeginSub(VertexBatch *batch, int vertCount, int indexCount)
{
    if (batch == NULL)
    {
        batch = &_glState->defaultVertexBatch;
    }
    Assert(!batch->dataSubStarted);
    Assert(!batch->readyToDraw);
    Assert(vertCount <= batch->spec.vertMax);
    Assert(indexCount <= batch->spec.indexMax);
    Assert(batch->vbo > 0);

    batch->dataSubStarted = true;
    batch->vertCount = vertCount;
    batch->indexCount = indexCount;

    bindBatch(batch);
}

sav_func VertexCountedData MakeVertexCountedData(void *data, size_t elemCount, size_t elemSize)
{
    VertexCountedData result;
    result.data = data;
    result.elemCount = elemCount;
    result.elemSize = elemSize;
    return result;
}

sav_func void VertexBatchSubVertexData(VertexBatch *batch, int attribIndex, VertexCountedData data)
{
    if (batch == NULL)
    {
        batch = &_glState->defaultVertexBatch;
    }
    Assert(batch->dataSubStarted);
    Assert(attribIndex >= 0 && attribIndex < batch->spec.attribMax);

    VertexAttrib *attrib = batch->spec.attribs + attribIndex;
    Assert(attrib->isEnabled);

    size_t dataSize = batch->vertCount * attrib->byteSize;
    Assert(dataSize == data.elemSize * data.elemCount);
    Assert(data.data != NULL);

    glBufferSubData(GL_ARRAY_BUFFER, attrib->byteOffset, dataSize, data.data);
}

sav_func void VertexBatchSubIndexData(VertexBatch *batch, VertexCountedData data)
{
    if (batch == NULL)
    {
        batch = &_glState->defaultVertexBatch;
    }
    Assert(batch->dataSubStarted);
    Assert(batch->ebo > 0);

    size_t dataSize =  batch->indexCount * batch->spec.indexByteSize;
    Assert(dataSize == data.elemSize * data.elemCount);
    Assert(data.data != NULL);

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, dataSize, data.data);
}

sav_func void VertexBatchEndSub(VertexBatch *batch)
{
    if (batch == NULL)
    {
        batch = &_glState->defaultVertexBatch;
    }
    Assert(batch->dataSubStarted);

    batch->dataSubStarted = false;
    batch->readyToDraw = true;

    bindBatch(NULL);
}

sav_func void DrawVertexBatch(VertexBatch *batch)
{
    if (batch == NULL)
    {
        batch = &_glState->defaultVertexBatch;
    }
    Assert(_glState->canDraw);
    Assert(batch->readyToDraw);

    Assert(batch->indexCount > 0); // TODO: Handle no ebo case
    glBindVertexArray(batch->vao);
    glDrawElements(_glState->drawMode, batch->indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // TODO: We may not want to reset the batch automatically after every draw call
    batch->readyToDraw = false;
    batch->vertCount = 0;
    batch->indexCount = 0;
}

sav_func void SetPointSize(f32 size)
{
    glPointSize(size);
}

sav_func void SetLineWidth(f32 size)
{
    glLineWidth(size);
}

sav_func void SetDrawMode(SavDrawMode drawMode)
{
    switch (drawMode)
    {
        case SAV_DRAW_TRIANGLES:
        {
            _glState->drawMode = GL_TRIANGLES;
        } break;

        case SAV_DRAW_LINES:
        {
            _glState->drawMode = GL_LINES;
        } break;

        case SAV_DRAW_POINTS:
        {
            _glState->drawMode = GL_POINTS;
        } break;

        default: InvalidCodePath;
    }
}

// SECTION Graphics: drawing
sav_func void ClearBackground(SavColor c)
{
    v4 cv4 = GetColorV4(c);
    glClearColor(cv4.r, cv4.g, cv4.b, cv4.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

sav_func void BeginDraw()
{
    Assert(!_glState->drawModeActive);

    glViewport(0, 0, (i32) _sdlState->windowSize.x, (i32) _sdlState->windowSize.y);
    m4 orthoProjection = GetOrthographicProjection(
        0.0f,
        _sdlState->windowSize.x,
        _sdlState->windowSize.y,
        0.0f, -1.0f, 1.0f);
    PushProjection(orthoProjection);

    _glState->drawModeActive = true;
    _glState->canDraw = true;
}

sav_func void EndDraw()
{
    Assert(_glState->drawModeActive);
    PopProjection();
    _glState->drawModeActive = false;
    _glState->canDraw = false;
}

sav_func void SavSwapBuffers()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(_sdlState->window);
}

sav_func void DrawTexture(SavTexture texture, Rect dest, Rect source, v2 origin, f32 rotation, SavColor color)
{
    v2 absOrigin = V2(dest.x, dest.y);
    
    dest.x -= origin.x;
    dest.y -= origin.y;
    
    FourV2 points = RectGetPoints(dest);
    if (rotation != 0.0f)
    {
        RotateFourPointsAroundOrigin(&points, absOrigin, rotation);
    }
    FourV3 pointsV3 = ConvertFourV2V3(points);

    FourV4 texCoords = ConvertFourV2V4(GetTextureRectTexCoords(texture, source));

    v4 cv4 = GetColorV4(color);
    v4 colors[] = { cv4, cv4, cv4, cv4 };
    u32 indices[] = { 0, 1, 2, 2, 3, 0 };

    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, ArrayCount(pointsV3.e), ArrayCount(indices));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(pointsV3.e, ArrayCount(pointsV3.e), sizeof(pointsV3.e[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_TEXCOORDS, MakeVertexCountedData(texCoords.e, ArrayCount(texCoords.e), sizeof(texCoords.e[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colors, ArrayCount(colors), sizeof(colors[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, ArrayCount(indices), sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    DrawVertexBatch(DEFAULT_VERTEX_BATCH);

    glBindTexture(GL_TEXTURE_2D, _glState->defaultTexture.id);
}

sav_func void DrawRect(Rect rect, SavColor color)
{
    FourV3 points = ConvertFourV2V3(RectGetPoints(rect));
    v4 texCoords[4] = {};
    v4 cv4 = GetColorV4(color);
    v4 colors[] = { cv4, cv4, cv4, cv4 };
    u32 indices[] = { 0, 1, 2, 2, 3, 0 };

    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, ArrayCount(points.e), ArrayCount(indices));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(points.e, ArrayCount(points.e), sizeof(points.e[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_TEXCOORDS, MakeVertexCountedData(texCoords, ArrayCount(texCoords), sizeof(texCoords[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colors, ArrayCount(colors), sizeof(colors[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, ArrayCount(indices), sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);

    DrawVertexBatch(DEFAULT_VERTEX_BATCH);
}

sav_func void DrawAtlasCell(SavTextureAtlas atlas, int x, int y, Rect destRect, SavColor color)
{
    int atlasPxX = x * atlas.cellW;
    int atlasPxY = y * atlas.cellH;
    Rect atlasRect = MakeRect((f32) atlasPxX, (f32) atlasPxY, (f32) atlas.cellW, (f32) atlas.cellH);
    DrawTexture(atlas.texture, destRect, atlasRect, {}, 0.0f, color);
}

// SECTION Fonts and text rendering
internal_func u32 loadTextureFromFont(void *data, u32 width, u32 height)
{
    u32 id;
    glGenTextures(1, &id);
    Assert(id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return id;
}

sav_func SavFont SavLoadFont(const char *path, u32 pointSize)
{
    SavFont font = {};

    font.glyphCount = 128;
    font.glyphInfos = (GlyphInfo *)calloc(1, font.glyphCount * sizeof(GlyphInfo));
    if (font.glyphInfos == NULL)
    {
        traceLogEngine("ERROR", "Failed to allocate for font glyph infos");
        InvalidCodePath;
        return {};
    }

    SDL_Surface **glyphImages = (SDL_Surface **)calloc(1, font.glyphCount * sizeof(SDL_Surface *));
    if (glyphImages == NULL)
    {
        traceLogEngine("ERROR", "Failed to allocate for glyph images");
        free(font.glyphInfos);
        InvalidCodePath;
        return {};
    }
    
    TTF_Font *sdlFont = TTF_OpenFont(path, pointSize);
    if (sdlFont == NULL)
    {
        const char *Error = SDL_GetError();
        traceLogEngine("ERROR", "SDL failed to load ttf font at %s:\n%s", path, Error);
        free(font.glyphInfos);
        free(glyphImages);
        InvalidCodePath;
        return {};
    }
    
    font.pointSize = (f32)pointSize;
    font.height = TTF_FontHeight(sdlFont);
    int bytesPerPixel = 4;

    int maxGlyphWidth = 0;
    int maxGlyphHeight = 0;
    for (u8 glyphChar = 32; glyphChar < font.glyphCount; glyphChar++)
    {
        GlyphInfo *glyphInfo = font.glyphInfos + glyphChar;

        TTF_GlyphMetrics(sdlFont, (char)glyphChar,
                         &glyphInfo->minX, &glyphInfo->maxX, &glyphInfo->minY, &glyphInfo->maxY, &glyphInfo->advance);

        SDL_Surface *glyphImage = TTF_RenderGlyph_Blended(sdlFont, (char)glyphChar, { 255, 255, 255, 255 });
        if (glyphImage->w > maxGlyphWidth)
        {
            maxGlyphWidth = glyphImage->w;
        }
        if (glyphImage->h > maxGlyphHeight)
        {
            maxGlyphHeight = glyphImage->h;
        }
        Assert(glyphImage->format->BytesPerPixel == bytesPerPixel);

        glyphImages[glyphChar] = glyphImage;
    }

    // NOTE: 12x8 = 96 -> for the 95 visible glyphs
    u32 atlasColumns = 12;
    u32 atlasRows = 8;
    u32 atlasPxWidth = atlasColumns * maxGlyphWidth;
    u32 atlasPxHeight = atlasRows * maxGlyphHeight;
    u32 atlasPitch = bytesPerPixel * atlasPxWidth;
    u8 *atlasBytes = (u8 *)calloc(1, atlasPitch * atlasPxHeight * sizeof(u8));
    if (atlasBytes == NULL)
    {
        traceLogEngine("ERROR", "Failed to allocate memory for font atlas");
        free(font.glyphInfos);
        free(glyphImages);
        TTF_CloseFont(sdlFont);
        InvalidCodePath;
        return {};
    }
    
    u32 currentGlyphIndex = 0;
    for (u8 glyphChar = 32; glyphChar < font.glyphCount; glyphChar++)
    {
        SDL_Surface *glyphImage = glyphImages[glyphChar];
        Assert(glyphImage->w > 0);
        Assert(glyphImage->w <= maxGlyphWidth);
        Assert(glyphImage->h > 0);
        Assert(glyphImage->h <= maxGlyphHeight);
        Assert(glyphImage->format->BytesPerPixel == bytesPerPixel);
        Assert(glyphImage->pixels);
        
        GlyphInfo *glyphInfo = font.glyphInfos + glyphChar;

        u32 currentAtlasCol = currentGlyphIndex % atlasColumns;
        u32 currentAtlasRow = currentGlyphIndex / atlasColumns;
        u32 atlasPxX = currentAtlasCol * maxGlyphWidth;
        u32 atlasPxY = currentAtlasRow * font.height;
        size_t atlasByteOffset = (atlasPxY * atlasPxWidth + atlasPxX) * bytesPerPixel;

        // NOTE: Hack solution to texture bleed in some fonts. 127 is DEL char, some fonts put a big rectangle for that, that fills
        //       the whole glyph height, and can bleed into surrounding glyphs.
        if (glyphChar != 127)
        {
            u8 *dest = atlasBytes + atlasByteOffset;
            u8 *source = (u8 *) glyphImage->pixels;
            for (int glyphPxY = 0; glyphPxY < glyphImage->h; glyphPxY++)
            {
                u8 *destByte = dest;
                u8 *sourceByte = source;
            
                for (int glyphPxX = 0; glyphPxX < glyphImage->w; glyphPxX++)
                {
                    for (int pixelByte = 0; pixelByte < bytesPerPixel; pixelByte++)
                    {
                        *destByte++ = *sourceByte++;
                    }
                }

                dest += atlasPitch;
                source += glyphImage->pitch;
            }
        }

        // NOTE:Use the atlas position and width/height to calculate UVs for each glyph
        // NOTE: It seems that SDL_ttf embeds MinX into the rendered glyph, but also it's ignored if it's less than 0
        //       Need to shift where to place glyph if MinX is negative, but if not negative, it's already included
        //       in the rendered glyph. This works but seems very finicky
        u32 glyphTexWidth = ((glyphInfo->minX >= 0) ? (glyphInfo->maxX) : (glyphInfo->maxX - glyphInfo->minX));
        u32 glyphTexHeight = maxGlyphHeight;
        
        f32 oneOverAtlasPxWidth = 1.0f / (f32)atlasPxWidth;
        f32 oneOverAtlasPxHeight = 1.0f / (f32)atlasPxHeight;
        f32 uvLeft = (f32)atlasPxX * oneOverAtlasPxWidth;
        f32 uvTop = (f32)atlasPxY * oneOverAtlasPxHeight;
        f32 uvRight = (f32)(atlasPxX + glyphTexWidth) * oneOverAtlasPxWidth;
        f32 uvBottom = (f32)(atlasPxY + glyphTexHeight) * oneOverAtlasPxHeight;
        glyphInfo->glyphUVs[0] = V2(uvLeft, uvTop);
        glyphInfo->glyphUVs[1] = V2(uvLeft, uvBottom);
        glyphInfo->glyphUVs[2] = V2(uvRight, uvBottom);
        glyphInfo->glyphUVs[3] = V2(uvRight, uvTop);
        
        SDL_FreeSurface(glyphImage);

        currentGlyphIndex++;
    }

    font.atlasTextureId = loadTextureFromFont(atlasBytes, atlasPxWidth, atlasPxHeight);

    traceLogEngine("INFO", "Loaded font at %s", path);
    
    #if 1
    const char *debugFilePath = "temp/fontAtlasTest.png";
    traceLogEngine("INFO", "Saved debug font atlas at %s", debugFilePath);
    SavSaveImage(debugFilePath, atlasBytes, atlasPxWidth, atlasPxHeight, false, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    #endif

    free(glyphImages);
    TTF_CloseFont(sdlFont);
    free(atlasBytes);
    
    return font;
}

sav_func v2 GetStringDimensions(const char *string,
                                SavFont *font, f32 pointSize)
{
    f32 sizeRatio = pointSize / font->pointSize;
    f32 lineHeight = sizeRatio * font->height;
    
    f32 width = 0.0f;
    f32 height = lineHeight;
    
    for (int stringI = 0; string[stringI] != '\0'; stringI++)
    {
        char glyph = string[stringI];
        if (glyph == '\n')
        {
            height += lineHeight;
        }
        else
        {
            width += sizeRatio * font->glyphInfos[glyph].advance;
        }
    }

    return V2(width, height);
}

sav_func void DrawString(const char *string,
                         SavFont *font, f32 pointSize, SavColor color,
                         f32 x, f32 y, f32 maxWidth,
                         MemoryArena *arena)
{
    // TODO: Line heigth based on ratio is probably not right
    f32 sizeRatio = pointSize / font->pointSize;

    int stringCount;
    for (stringCount = 0; string[stringCount] != '\0'; stringCount++) {}
    
    MemoryArenaFreeze(arena);

    enum WrapPoint : u8
    {
        WRAP_POINT_NONE,
        WRAP_POINT_SPACE,
        WRAP_POINT_GLYPH
    };
    
    WrapPoint *wrapPoints = MemoryArenaPushArrayAndZero(arena, stringCount, WrapPoint);
    
    if (maxWidth > 0.0f)
    {
        int prevSpaceI = 0;
        f32 wrapXAccum = 0.0f;
        for (int stringI = 0; stringI < stringCount; stringI++)
        {
            // TODO: This doesn't handle new lines in the string
            char glyph = string[stringI];

            GlyphInfo *glyphInfo = font->glyphInfos + glyph;

            wrapXAccum += sizeRatio * glyphInfo->advance;

            if (glyph == ' ')
            {
                prevSpaceI = stringI;
            }

            if (wrapXAccum > maxWidth)
            {
                if (prevSpaceI != 0)
                {
                    wrapPoints[prevSpaceI] = WRAP_POINT_SPACE;
                    stringI = prevSpaceI;
                    wrapXAccum = 0.0f;
                    prevSpaceI = 0;
                }
                else
                {
                    wrapPoints[stringI] = WRAP_POINT_GLYPH;
                    wrapXAccum = 0.0f;
                }
            }
        }
    }
    
    f32 currentX = x;
    f32 maxX = x;
    f32 currentY = y;

    int stringVisibleCount = 0;
    for (int stringI = 0; stringI < stringCount; ++stringI)
    {
        if (string[stringI] != '\n' && wrapPoints[stringI] != 1)
        {
            stringVisibleCount++;
        }
    }

    int vertCount = stringVisibleCount * 4;
    int indexCount = stringVisibleCount * 6;
    v3 *positions = MemoryArenaPushArray(arena, vertCount, v3);
    v4 *texCoords = MemoryArenaPushArray(arena, vertCount, v4);
    v4 *colors = MemoryArenaPushArray(arena, vertCount, v4);
    u32 *indices = MemoryArenaPushArray(arena, indexCount, u32);

    int vertsAdded = 0;
    int indicesAdded = 0;
    for (int stringIndex = 0; stringIndex < stringCount; stringIndex++)
    {
        char glyph = string[stringIndex];

        f32 scaledHeight = sizeRatio * font->height;
        
        if (glyph == '\n' || wrapPoints[stringIndex] > WRAP_POINT_NONE)
        {
            if (currentX > maxX)
            {
                maxX = currentX;
            }
            currentX = x;
            currentY += scaledHeight;
            if (wrapPoints[stringIndex] == WRAP_POINT_GLYPH)
            {
                // Wrap point is a printable glyph, so render it
            }
            else
            {
                // Wrap point is a space, so skip it
                continue;
            }
        }

        Assert(glyph >= 32 && glyph < font->glyphCount);
        GlyphInfo *glyphInfo = font->glyphInfos + glyph;

        f32 scaledMinX = sizeRatio * glyphInfo->minX;
        f32 scaledMaxX = sizeRatio * glyphInfo->maxX;

        f32 pxX = ((scaledMinX >= 0) ? currentX : (currentX + scaledMinX));
        f32 pxY = currentY;
        f32 pxWidth = (f32)((scaledMinX >= 0) ? scaledMaxX : (scaledMaxX - scaledMinX));
        f32 pxHeight = scaledHeight;

        v4 colorV4 = GetColorV4(color);
        
        u32 indexBase = vertsAdded;
        positions[vertsAdded] = V3(pxX, pxY, 0);
        colors[vertsAdded] = colorV4;
        texCoords[vertsAdded] = V4(glyphInfo->glyphUVs[0]);
        vertsAdded++;

        positions[vertsAdded] = V3(pxX, pxY + pxHeight, 0);
        colors[vertsAdded] = colorV4;
        texCoords[vertsAdded] = V4(glyphInfo->glyphUVs[1]);
        vertsAdded++;
        
        positions[vertsAdded] = V3(pxX + pxWidth, pxY + pxHeight, 0);
        colors[vertsAdded] = colorV4;
        texCoords[vertsAdded] = V4(glyphInfo->glyphUVs[2]);
        vertsAdded++;

        positions[vertsAdded] = V3(pxX + pxWidth, pxY, 0);
        colors[vertsAdded] = colorV4;
        texCoords[vertsAdded] = V4(glyphInfo->glyphUVs[3]);
        vertsAdded++;

        u32 localIndices[] = { 0, 1, 3, 3, 1, 2 };

        for (int i = 0; i < ArrayCount(localIndices); i++)
        {
            indices[indicesAdded++] = indexBase + localIndices[i];
        }

        currentX += sizeRatio * glyphInfo->advance;
    }
    Assert(vertsAdded == vertCount);
    Assert(indicesAdded == indexCount);

    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexCount);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_TEXCOORDS, MakeVertexCountedData(texCoords, vertCount, sizeof(texCoords[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colors, vertCount, sizeof(colors[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->atlasTextureId);

    DrawVertexBatch(DEFAULT_VERTEX_BATCH);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _glState->defaultTexture.id);

    MemoryArenaUnfreeze(arena);
}

sav_func void SavFreeFont(SavFont *font)
{
    free(font->glyphInfos);
    *font = {};
}

// SECTION File IO
sav_func char *SavReadTextFile(const char *path)
{
    FILE *file;
    fopen_s(&file, path, "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);
        Assert(fileSize > 0);
        fseek(file, 0, SEEK_SET);
        char *result = (char *) malloc(fileSize + 1);
        if (result)
        {
            size_t elementsRead = fread(result, fileSize, 1, file);
            Assert(elementsRead == 1);
            result[fileSize] = '\0';
            fclose(file);
            return result;
        }
        else
        {
            traceLogEngine("ERROR", "Failed to alloc memory when reading file at %s", path);
        }
    }
    else
    {
        traceLogEngine("ERROR", "Failed to open file at %s", path);
    }
    return 0;
}

sav_func void SavFreeString(char **text)
{
    free(*text);
    *text = 0;
}

// SECTION Util
sav_func const char *TextFormat(const char *format, ...)
{
    // TODO: There are quite a few cases where this won't work, maybe do as macro
    local_persist char result[STRING_BUFFER];

    va_list varArgs;
    va_start(varArgs, format);
    vsprintf_s(result, STRING_BUFFER - 1, format, varArgs);
    va_end(varArgs);

    return result;
}

sav_func void TraceLog(const char *format, ...)
{
    char formatBuf[STRING_BUFFER];
    sprintf_s(formatBuf, "GAME: INFO: [F %06zu] %s\n", _sdlState->currentFrame, format);
    
    va_list varArgs;
    va_start(varArgs, format);
    vprintf_s(formatBuf, varArgs);
    va_end(varArgs);
}

sav_func void TraceLogPartialStart(const char *format, ...)
{
    char formatBuf[STRING_BUFFER];
    sprintf_s(formatBuf, "GAME: INFO: [F %06zu] %s", _sdlState->currentFrame, format);
    
    va_list varArgs;
    va_start(varArgs, format);
    vprintf_s(formatBuf, varArgs);
    va_end(varArgs);
}

sav_func void TraceLogPartial(const char *format, ...)
{
    va_list varArgs;
    va_start(varArgs, format);
    vprintf_s(format, varArgs);
    va_end(varArgs);
}

sav_func void TraceError(const char *format, ...)
{
    char formatBuf[STRING_BUFFER];
    sprintf_s(formatBuf, "GAME: ERROR: [F %06zu] %s\n", _sdlState->currentFrame, format);
    
    va_list varArgs;
    va_start(varArgs, format);
    vprintf_s(formatBuf, varArgs);
    va_end(varArgs);
}

sav_func void Memset(void *ptr, int value, size_t num)
{
    memset(ptr, value, num);
}

sav_func void Strcpy(char *dest, char *source)
{
    strcpy(dest, source);
}

sav_func void StringFormat(char *format, StringBuffer outputBuffer, ...)
{
    va_list varArgs;
    va_start(varArgs, outputBuffer);
    vsnprintf(outputBuffer.string, outputBuffer.size, format, varArgs);
    va_end(varArgs);
}

sav_func void StringFormat(char *format, StringBuffer outputBuffer, va_list varArgs)
{
   vsnprintf(outputBuffer.string, outputBuffer.size, format, varArgs);
}

// SECTION Random
sav_func int GetRandomValue(int min, int max)
{
    if (max - min <= 0)
    {
        return 0;
    }
    return (min + rand() % (max - min));
}

sav_func f32 GetRandomFloat()
{
    return (rand() / (f32) RAND_MAX);
}

sav_func b32 RandomChane(f32 chance)
{
    i32 threshold = (i32)(RAND_MAX * chance);
    return rand() > threshold;
}

sav_func v2 GetRandomVec(f32 length)
{
    f32 angle = GetRandomFloat() * 2.0f * (f32) PI;
    v2 result;
    result.x = cosf(angle);
    result.y = sinf(angle);
    result *= length;
    return result;
}

#endif

#endif
