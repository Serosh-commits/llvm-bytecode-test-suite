// RUN: %clang_cc1 -fexperimental-new-constant-interpreter -verify -std=c++23 %s
// RUN: %clang_cc1 -verify -std=c++23 %s
// expected-no-diagnostics

#include <complex>
#include <bit>
#include <cmath>

template<typename T>
struct Bytes { unsigned char b[sizeof(T)]; bool operator==(const Bytes&) const = default; };

template<typename T>
constexpr bool bitEqual(T a, T b) {
  return std::bit_cast<Bytes<T>>(a) == std::bit_cast<Bytes<T>>(b);
}

constexpr double nanP  = __builtin_nan("0x1337");
constexpr float  nanPf = __builtin_nanf("0x1337");
constexpr double inf   = __builtin_inf();

template<typename T>
constexpr bool test_construction_and_access() {
  std::complex<T> z1;
  assert(z1.real() == T{} && z1.imag() == T{});

  std::complex<T> z2(T(3), T(4));
  assert(z2.real() == T(3) && z2.imag() == T(4));

  std::complex<T> z3 = z2;
  assert(z3.real() == T(3) && z3.imag() == T(4));

  return true;
}

template<typename T>
constexpr bool test_conj_norm_abs() {
  std::complex<T> z(T(3), T(4));

  auto c = std::conj(z);
  assert(c.real() == T(3) && c.imag() == T(-4));

  assert(std::norm(z) == T(25));

  assert(std::abs(z) == T(5));

  return true;
}

template<typename T>
constexpr bool test_arg_polar() {
  std::complex<T> z(T(3), T(4));
  T theta = std::arg(z);

  T expected = std::atan2(T(4), T(3));
  assert(std::abs(theta - expected) < T(1e-9));

  auto p = std::polar(T(5), theta);
  assert(std::abs(p.real() - T(3)) < T(1e-9));
  assert(std::abs(p.imag() - T(4)) < T(1e-9));

  auto p0 = std::polar(T(7), T(0));
  assert(p0.real() == T(7) && p0.imag() == T(0));

  return true;
}

template<typename T>
constexpr bool test_proj() {
  assert(std::proj(std::complex<T>(inf, T(100))).real() == inf);
  assert(std::proj(std::complex<T>(inf, T(100))).imag() == T(0));

  assert(std::proj(std::complex<T>(T(-100), inf)).real() == inf);
  assert(std::proj(std::complex<T>(T(-100), inf)).imag() == T(0));

  assert(std::proj(std::complex<T>(-inf, T(5))).real() == inf);
  assert(std::proj(std::complex<T>(-inf, T(5))).imag() == T(0));

  std::complex<T> fin(T(2), T(-3));
  auto pr = std::proj(fin);
  assert(pr.real() == T(2) && pr.imag() == T(-3));

  return true;
}

template<typename T>
constexpr bool test_special_values() {
  constexpr T nan = T(__builtin_nan("0x1337"));
  constexpr T inf = T(__builtin_inf());

  std::complex<T> z_nan(nan, T(1));
  assert(std::isnan(std::real(z_nan)));
  assert(std::isnan(std::imag(z_nan)) == false);

  auto c = std::conj(z_nan);
  assert(bitEqual(std::real(c), nan));

  assert(std::isnan(std::abs(z_nan)));
  assert(std::isnan(std::norm(z_nan)));

  assert(std::arg(std::complex<T>(T(0), T(0))) == T(0));

  assert(std::arg(std::complex<T>(T(1), T(0))) == T(0));
  assert(std::arg(std::complex<T>(T(1), -T(0))) == -T(0));

  return true;
}

constexpr bool test_all() {
  return test_construction_and_access<float>() && test_construction_and_access<double>() && test_construction_and_access<long double>() &&
         test_conj_norm_abs<float>() && test_conj_norm_abs<double>() && test_conj_norm_abs<long double>() &&
         test_arg_polar<float>() && test_arg_polar<double>() && test_arg_polar<long double>() &&
         test_proj<float>() && test_proj<double>() && test_proj<long double>() &&
         test_special_values<float>() && test_special_values<double>() && test_special_values<long double>();
}

#pragma STDC FENV_ROUND FE_TOWARDZERO
static_assert(std::abs(std::complex<double>(3.0, 4.0)) == 5.0);
static_assert(std::arg(std::complex<double>(1.0, 0.0)) == 0.0);
static_assert(std::proj(std::complex<double>(0.0, -0.0)).imag() == -0.0);
