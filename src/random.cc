#include <whs31/common/random.h>

#include <cmath>

// The floating-point code below must round every operation as IEEE 754 prescribes. The build
// compiles this file with contraction into fused multiply-adds disabled (see CMakeLists.txt);
// value-changing optimizations cannot be undone here, so refuse them outright.
#ifdef __FAST_MATH__
#  error "deterministic random streams cannot be built with -ffast-math"
#endif

WHS31_COMMON_NAMESPACE_BEGIN

double portable_log(double const x) noexcept
{
  // ln 2 split in two: the high part ends in 21 zero bits, so `k * ln2_hi` is exact for the
  // exponent k of any double
  auto constexpr ln2_hi = 0x1.62e42fee00000p-1;  // NOLINT(modernize-use-std-numbers)
  auto constexpr ln2_lo = 0x1.a39ef35793c76p-33;
  auto constexpr lg1 = 0x1.5555555555593p-1;
  auto constexpr lg2 = 0x1.999999997fa04p-2;
  auto constexpr lg3 = 0x1.2492494229359p-2;
  auto constexpr lg4 = 0x1.c71c51d8e78afp-3;
  auto constexpr lg5 = 0x1.7466496cb03dep-3;
  auto constexpr lg6 = 0x1.39a09d078c69fp-3;
  auto constexpr lg7 = 0x1.2f112df3e5244p-3;
  auto constexpr sqrt_half = 0x1.6a09e667f3bcdp-1;

  auto exponent = 0;
  auto mantissa = std::frexp(x, &exponent);
  if(mantissa < sqrt_half) {
    mantissa *= 2.0;
    --exponent;
  }
  auto const f = mantissa - 1.0;
  auto const half_f_squared = 0.5 * f * f;
  auto const s = f / (2.0 + f);
  auto const z = s * s;
  auto const w = z * z;
  auto const t1 = w * (lg2 + w * (lg4 + w * lg6));
  auto const t2 = z * (lg1 + w * (lg3 + w * (lg5 + w * lg7)));
  auto const r = t2 + t1;
  auto const k = static_cast<double>(exponent);
  return s * (half_f_squared + r) + k * ln2_lo - half_f_squared + f + k * ln2_hi;
}

double portable_exp(double const x) noexcept
{
  auto constexpr ln2_hi = 0x1.62e42fee00000p-1;  // NOLINT(modernize-use-std-numbers)
  auto constexpr ln2_lo = 0x1.a39ef35793c76p-33;
  auto constexpr inv_ln2 = 0x1.71547652b82fep0;  // NOLINT(modernize-use-std-numbers)
  auto constexpr p1 = 0x1.555555555553ep-3;
  auto constexpr p2 = -0x1.6c16c16bebd93p-9;
  auto constexpr p3 = 0x1.1566aaf25de2cp-14;
  auto constexpr p4 = -0x1.bbd41c5d26bf1p-20;
  auto constexpr p5 = 0x1.6376972bea4d0p-25;

  // ln2_hi ends in 21 zero bits, so `k * ln2_hi` and the reduction `x - k * ln2_hi` are exact
  auto const k = std::round(x * inv_ln2);
  auto const hi = x - k * ln2_hi;
  auto const lo = k * ln2_lo;
  auto const r = hi - lo;
  auto const t = r * r;
  auto const c = r - t * (p1 + t * (p2 + t * (p3 + t * (p4 + t * p5))));
  auto const y = 1.0 - ((lo - (r * c) / (2.0 - c)) - hi);
  return std::ldexp(y, static_cast<int>(k));
}

double RandomStream::uniform(double const lo, double const hi) noexcept
{
  return lo + (hi - lo) * this->uniform();
}

double RandomStream::normal() noexcept
{
  while(true) {
    auto const u = 2.0 * this->uniform() - 1.0;
    auto const v = 2.0 * this->uniform() - 1.0;
    if(auto const s = u * u + v * v; s > 0.0 and s < 1.0)
      return u * std::sqrt(-2.0 * portable_log(s) / s);
  }
}

double RandomStream::normal(double const mean, double const stddev) noexcept
{
  return mean + stddev * this->normal();
}

WHS31_COMMON_NAMESPACE_END
