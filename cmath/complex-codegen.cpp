// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -fexperimental-new-constant-interpreter -emit-llvm -o - %s | FileCheck %s -check-prefix=NEW

#include <complex>

constexpr std::complex<double> z(3.0, 4.0);
constexpr std::complex<double> neg_zero_imag(0.0, -0.0);
constexpr std::complex<double> inf_real(__builtin_inf(), 0.0);

// CHECK-LABEL: define{{.*}} @test_abs
// CHECK: ret double 5.000000e+00
double test_abs() { return std::abs(z); }

// NEW-LABEL: define{{.*}} @test_abs
// NEW: ret double 5.000000e+00

// CHECK-LABEL: define{{.*}} @test_norm
// CHECK: ret double 2.500000e+01
double test_norm() { return std::norm(z); }

// NEW-LABEL: define{{.*}} @test_norm
// NEW: ret double 2.500000e+01

// CHECK-LABEL: define{{.*}} @test_conj
// CHECK: ret { double, double } { double 3.000000e+00, double -4.000000e+00 }
std::complex<double> test_conj() { return std::conj(z); }

// NEW-LABEL: define{{.*}} @test_conj
// NEW: ret { double, double } { double 3.000000e+00, double -4.000000e+00 }

// CHECK-LABEL: define{{.*}} @test_arg
// CHECK: ret double
double test_arg() { return std::arg(z); }

// NEW-LABEL: define{{.*}} @test_arg
// NEW: ret double

// CHECK-LABEL: define{{.*}} @test_polar
// CHECK: ret { double, double }
std::complex<double> test_polar() { return std::polar(5.0, std::arg(z)); }

// NEW-LABEL: define{{.*}} @test_polar
// NEW: ret { double, double }

// CHECK-LABEL: define{{.*}} @test_proj
// CHECK: ret { double, double } { double 0.000000e+00, double -0.000000e+00 }
std::complex<double> test_proj() { return std::proj(neg_zero_imag); }

// NEW-LABEL: define{{.*}} @test_proj
// NEW: ret { double, double } { double 0.000000e+00, double -0.000000e+00 }

// CHECK-LABEL: define{{.*}} @test_proj_inf
// CHECK: ret { double, double } { double 0x7FF0000000000000, double 0x0000000000000000 }
std::complex<double> test_proj_inf() { return std::proj(inf_real); }

// NEW-LABEL: define{{.*}} @test_proj_inf
// NEW: ret { double, double } { double 0x7FF0000000000000, double 0x0000000000000000 }

// CHECK-LABEL: define{{.*}} @test_runtime_fallback
// CHECK: call {{.*}} @std::abs
// CHECK: ret double
double test_runtime_fallback(double r, double i) { return std::abs(std::complex<double>(r, i)); }

// NEW-LABEL: define{{.*}} @test_runtime_fallback
// NEW: call {{.*}} @std::abs
// NEW: ret double
