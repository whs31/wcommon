# Random streams {#random_streams}

Randomness from `whs31/common/random.h` is reproducible: the same seed gives bit-identical
draws on Linux, MinGW and MSVC builds, however many streams a program creates and in whatever
order. Use it wherever results must replay exactly: simulations, regression tests, synthetic target
generators.

## Using a stream

```cpp
#include <whs31/common/noise.h>
#include <whs31/common/random.h>

using namespace whs31;
using namespace std::chrono_literals;

// when the owner is created: one stream per independent noise source
auto gyro_noise = RandomStream::derive(world_seed, "vt30-1", "imu.gyro");
auto gyro_bias = GaussMarkov::with(0.002, 300s, 10ms);

// every sample
auto const noise_rad_per_s = gyro_noise.normal(0.0, sigma_rad_per_s);
auto const bias_rad_per_s = gyro_bias.advance(gyro_noise);
```

- **Seed**: `stream_seed(seed, scope, stream)` is a length-prefixed 64-bit FNV-1a hash of a
  top-level seed and two stable names, expanded into the generator state by SplitMix64.
  `RandomStream::derive` does both. Use names that do not depend on creation order, e.g. a
  vehicle name rather than an entity ID, so adding another owner or stream leaves every existing
  stream unchanged.
- **Stream names**: one stream per independent source, e.g. `imu.gyro`, `imu.accel`,
  `imu.dropout`. Adding a draw to one source then never shifts another.
- **State**: `RandomStream` is the 32-byte xoshiro256** state in a trivially copyable aggregate.
  Keep it inside the object that draws from it, never in a global, so a snapshot of that object
  also restores its noise. Copying a stream forks it: both copies give the same draws. The JSON
  adapter in `whs31/common/json/random.h` saves and restores the state.
- **Samplers**: `next()` (64 bits), `uniform()` in `[0, 1)`, `uniform(lo, hi)`, `below(n)` (a whole
  number in `[0, n)` without modulo bias), `normal()` and `normal(mean, stddev)`.
- **Noise processes** (`whs31/common/noise.h`): `WhiteNoise` (from a power spectral density),
  `GaussMarkov` (first order, by correlation time), `RandomWalk` and `turn_on_bias`. Each sample
  draws exactly once from its stream, whatever the parameters.

## Why not `<random>`

- `std::normal_distribution` and the other distributions are implementation-defined: libstdc++
  and the MSVC STL give different sequences from the same engine.
- `std::log`, `std::sin` and `std::cos` are not correctly rounded and may differ by an ulp between
  glibc and the Windows C runtimes. `normal()` is therefore the Marsaglia polar method (no sine or
  cosine) with its own logarithm, `portable_log`, the fdlibm algorithm within 1 ulp, built only
  from operations IEEE 754 rounds exactly: `+`, `-`, `*`, `/`, `sqrt`, `frexp`. The noise
  processes take their coefficients from `portable_exp`, the fdlibm exponential, for the same
  reason.

## Keeping it bit-identical

The golden-value test (`tests/test_random.cc`) pins the first draws of
`derive(1, "vt30-1", "imu.gyro")`, as computed by an independent Python implementation. If it
fails on a platform, the platform rounds differently; do not update the values to make it pass.
Known causes:

- `-ffast-math`, `/fp:fast` or any flag that reorders floating-point arithmetic. `random.cc` and
  `noise.cc` refuse to build with `-ffast-math`.
- Contraction of `a * b + c` into a fused multiply-add. Clang contracts by default, and so does
  GCC in GNU mode, whenever the target has FMA (e.g. `-march=x86-64-v3` or ARM64). The library
  build compiles `random.cc` and `noise.cc` with `-ffp-contract=off` (`/fp:precise` on MSVC), and
  every floating-point sampler is compiled there rather than inline, so the consumer's flags do
  not change the results.

Changing an algorithm here changes every sequence in every recorded run; do it only on purpose and
say so in the changelog.
