// static.h
//
#pragma once
#ifndef __MONIK_COMMON_STATIC_H__
#define __MONIK_COMMON_STATIC_H__

#include "monik/common/config.h"

#if defined(MONIK_OS_WIN32)
#pragma warning(disable: 4996) //warning C4996: 'mbstowcs': This function or variable may be unsafe.
#endif

#if defined(MONIK_OS_WIN32) && (_MSC_VER == 1800)
#error constexpr does not compiled in Visual Studio 2013 
#endif

namespace monik {

using int8 = std::int8_t;
using uint8 = std::uint8_t;
using int16 = std::int16_t;
using uint16 = std::uint16_t;
using int32 = std::int32_t;
using uint32 = std::uint32_t;
using int64 = std::int64_t;
using uint64 = std::uint64_t;

struct limits {
    limits() = delete;
    static constexpr double fepsilon = 1e-12;
    static constexpr int double_max_digits10 = std::numeric_limits<double>::max_digits10; // = 17
};

inline bool is_str_valid(const char * str)
{
    return str && str[0];
}

inline bool is_str_valid(const wchar_t * str)
{
    return str && str[0];
}

inline bool is_str_empty(const char * str)
{
    return !is_str_valid(str);
}

inline bool is_str_empty(const wchar_t * str)
{
    return !is_str_valid(str);
}

template <class T, T b> inline constexpr T a_min(const T a)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    return (a < b) ? a : b;
}

template <class T, T b> inline constexpr T a_max(const T a)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    return (b < a) ? a : b;
}

template <class T> inline constexpr T a_min(const T a, const T b)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    return (a < b) ? a : b;
}

template <class T> inline constexpr T a_max(const T a, const T b)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    return (b < a) ? a : b;
}

template <class T, T min, T max> inline constexpr T a_min_max(const T a)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    static_assert(min < max, "");
    return (a < min) ? min : ((a < max) ? a : max);
}

template <class T> inline constexpr T a_min_max(const T a, const T min, const T max)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    return (a < min) ? min : ((a < max) ? a : max);
}

template <class T> inline void set_min(T & a, const T b)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    if (b < a) a = b;
}

template <class T> inline void set_max(T & a, const T b)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    if (a < b) a = b;
}

template <class T> inline void set_max_min(T & a, const T x1, const T x2)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    set_max(a, a_min(x1, x2));
}

template <class T> inline void set_min_max(T & a, const T x1, const T x2)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    set_min(a, a_max(x1, x2));
}

template <class T> inline constexpr T a_abs(const T a)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    return (a < 0) ? (-a) : a;
}

template <class T> constexpr int a_sign(const T v)
{
    static_assert(sizeof(T) <= sizeof(double), "");
    return (v > 0) ? 1 : ((v < 0) ? -1 : 0);
}

inline constexpr bool fequal(double const f1, double const f2)
{
    return a_abs(f1 - f2) <= limits::fepsilon;
}

inline bool positive_fzero(double const f1) {
    MONIK_ASSERT(f1 >= 0);
    return f1 <= limits::fepsilon;
}

inline constexpr bool fzero(double const f1)
{
    return a_abs(f1) <= limits::fepsilon;
}

inline constexpr bool fless_eq(double const f1, double const f2)
{
    return f1 <= (f2 + limits::fepsilon);
}

template <int f1> inline constexpr bool fless_eq(double const f2)
{
    return f1 <= (f2 + limits::fepsilon);
}

inline constexpr bool fless(double const f1, double const f2)
{
    return (f1 + limits::fepsilon) < f2;
}

inline constexpr bool frange(double const x, double const left, double const right) 
{
    return fless_eq(left, x) && fless_eq(x, right);
}

inline constexpr int fsign(double const v) {
    return (v > 0) ? 1 : ((v < 0) ? -1 : 0);
}

inline double fatan2(double const y, double const x) {
    if (fzero(y) && fzero(x))
        return 0;
    return std::atan2(y, x);
}

inline constexpr uint32 reverse_bytes(uint32 const x) {
    return ((x & 0xFF) << 24)
        | ((x & 0xFF00) << 8)
        | ((x & 0xFF0000) >> 8)
        | ((x & 0xFF000000) >> 24);
}

template<typename T>
inline constexpr bool is_odd(T const x) {
    static_assert(std::is_integral<T>::value, "is_integral");
    return (x & 1) != 0;
}

template<typename T>
inline constexpr bool is_pod(T const &) {
    return std::is_pod<T>::value;
}

template<unsigned int x> struct is_power_2
{
    enum { value = x && !(x & (x - 1)) };
};

inline constexpr bool is_power_two(unsigned int const x)
{
    return (x > 0) && !(x & (x - 1));
}

template<unsigned int x> 
struct power_of  {
    static_assert(is_power_2<x>::value, "power_of");
    enum { value = 1 + power_of<x/2>::value };
};

template<> struct power_of<1> {
    enum { value = 0 };
};

template<size_t N> struct kilobyte
{
    enum { value = N * (1 << 10) };
};

template<size_t N> struct megabyte
{
    enum { value = N * (1 << 20) };
};

template<size_t N> struct min_to_sec
{
    enum { value = N * 60 };
};

template<size_t N> struct hour_to_sec
{
    enum { value = N * 60 * 60 };
};

template<size_t N> struct day_to_sec
{
    enum { value = 24 * hour_to_sec<N>::value };
};

// This template function declaration is used in defining A_ARRAY_SIZE.
// Note that the function doesn't need an implementation, as we only use its type.
//template <typename T, size_t N> char(&A_ARRAY_SIZE_HELPER(T const(&array)[N]))[N];
//#define A_ARRAY_SIZE(a) (sizeof(A_ARRAY_SIZE_HELPER(a)))

template <typename T> struct array_info;
template <typename T, size_t N>
struct array_info<T[N]>
{
    typedef T value_type;
    enum { size = N };
};

//Alternative of macros A_ARRAY_SIZE
//This function template can't be instantiated with pointer argument, only array.
template< class Type, size_t n > inline constexpr
size_t count_of(Type const(&)[n])
{
    return n;
}

template <unsigned long N> struct binary;

template <> struct binary<0>
{
    static unsigned const value = 0;
};

template <unsigned long N>
struct binary
{
    static unsigned const value = 
        (binary<N / 10>::value << 1)    // prepend higher bits
            | (N % 10);                 // to lowest bit
};

template<class T>
inline void memset_pod(T& dest, int value) noexcept
{
    M_STATIC_ASSERT_IS_POD(T);
    memset(&dest, value, sizeof(T));
}

template<class T> 
inline void memset_zero(T& dest) noexcept
{
    M_STATIC_ASSERT_IS_POD(T);
    memset(&dest, 0, sizeof(T));
}

template<class Dest, class Source> inline 
void memcpy_pod(Dest & dest, Source const & src) noexcept
{
    M_STATIC_ASSERT_IS_POD(Dest);
    M_STATIC_ASSERT_IS_POD(Source);
    static_assert(sizeof(Dest) == sizeof(Source), "");
    memcpy(&dest, &src, sizeof(dest));
}

template<class Type, size_t size> inline 
void memcpy_array(Type(&dest)[size], Type const(&src)[size], size_t const count) noexcept
{
    static_check_is_trivially_copyable(dest);
    static_check_is_trivially_copyable(src);
    MONIK_ASSERT(count <= size);
    memcpy(&dest, &src, sizeof(Type) * a_min(count, size));
}

template<class T1, class T2> inline 
int memcmp_pod(T1 const & x, T2 const & y) noexcept
{
    M_STATIC_ASSERT_IS_POD(T1);
    M_STATIC_ASSERT_IS_POD(T2);
    static_assert(sizeof(T1) == sizeof(T2), "");
    return memcmp(&x, &y, sizeof(x));
}

template<class T1, class T2>
inline void assign_static_cast(T1 & dest, const T2 & src) {
    dest = static_cast<T1>(src);
}

#if 0 // std::make_unique available since C++14
template<typename T, typename... Ts> inline
std::unique_ptr<T> make_unique(Ts&&... params) {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}
#endif

template<typename pointer, typename... Ts> inline
void reset_new(pointer & dest, Ts&&... params) {
    using T = typename pointer::element_type;
    dest.reset(new T(std::forward<Ts>(params)...));
}

template<class T, class Base, typename... Ts> inline
void reset_shared(std::shared_ptr<Base> & dest, Ts&&... params) {
    static_assert(std::is_base_of<Base, T>::value, "");
    static_assert(std::has_virtual_destructor<Base>::value, "");
    dest.reset(new T(std::forward<Ts>(params)...));
}

class sdl_exception : public std::logic_error {
    using base_type = std::logic_error;
public:
    sdl_exception(): base_type("unknown exception"){}
    explicit sdl_exception(const char* s): base_type(s){
        MONIK_ASSERT(s);
    }
    explicit sdl_exception(const std::string & s): base_type(s){}
};

template<class T>
class sdl_exception_t : public sdl_exception {
public:
    using type = T;
    explicit sdl_exception_t(const char* s) : sdl_exception(s){}
};

template<typename T, typename... Ts> inline
void throw_error(Ts&&... params) {
    static_assert(std::is_base_of<sdl_exception, T>::value, "is_base_of");
    MONIK_ASSERT_WIN32(!"throw_error");
    throw T(std::forward<Ts>(params)...);
}

template<typename T, typename... Ts> inline
void throw_error_if(const bool condition, Ts&&... params) {
    static_assert(std::is_base_of<sdl_exception, T>::value, "is_base_of");
    if (condition) {
        MONIK_ASSERT_WIN32(!"throw_error_if");
        throw T(std::forward<Ts>(params)...);
    }
}

template<typename T, typename... Ts> inline
void throw_error_if_not(const bool condition, Ts&&... params) {
    static_assert(std::is_base_of<sdl_exception, T>::value, "is_base_of");
    if (!condition) {
        MONIK_ASSERT_WIN32(!"throw_error_if_not");
        throw T(std::forward<Ts>(params)...);
    }
}

inline void sdl_throw_error_if(bool condition, const char * what) {
    throw_error_if<sdl_exception>(condition, what);
}

template<class T> using vector_unique_ptr = std::vector<std::unique_ptr<T>>;
template<class T> using vector_shared_ptr = std::vector<std::shared_ptr<T>>;

template<class T>
using remove_reference_t = typename std::remove_reference<T>::type; // since C++14

template <typename T> struct identity
{
    typedef T type;
};

template<bool b> using bool_constant = std::integral_constant<bool, b>;

template<typename T> 
inline bool is_found(T && key, std::initializer_list<T> a) {
    return std::find(a.begin(), a.end(), key) != a.end();
}

enum class break_or_continue {
    break_,
    continue_
};

using bc = break_or_continue;

inline constexpr bc make_break_or_continue(bc const t) {
    return t;
}
inline constexpr bc make_break_or_continue(bool const t) {
    return static_cast<break_or_continue>(t);
}
template<class T> bc make_break_or_continue(T) = delete;

template<class T> inline bool is_break(T t) {
    return make_break_or_continue(t) == bc::break_;
}

#if 0
template<class T> 
struct is_nothrow_copy_assignable {
    static constexpr bool value = std::is_nothrow_copy_assignable<T>::value;
};
#endif

} // monik

#endif // __MONIK_COMMON_STATIC_H__
