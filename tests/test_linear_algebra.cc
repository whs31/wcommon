#include <whs31/common/angle.h>
#include <whs31/common/error.h>
#include <whs31/common/euler_angles.h>
#include <whs31/common/matrix3x3.h>
#include <whs31/common/quaternion.h>
#include <whs31/common/vector2.h>
#include <whs31/common/vector3.h>

#include <cmath>
#include <algorithm>
#include <array>
#include <bit>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include <fmt/format.h>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

using namespace whs31;

namespace
{
  auto constexpr unit_x = Vector3::unit_x();
  auto constexpr unit_y = Vector3::unit_y();
  auto constexpr unit_z = Vector3::unit_z();
  auto constexpr sample_vector = Vector3(0.3, -1.7, 2.4);
  auto constexpr inertia = Matrix3x3::
    from_rows({0.5, -0.02, 0.05}, {-0.02, 3.2, 0.01}, {0.05, 0.01, 3.0});

  [[nodiscard]] double max_difference(Vector3 const a, Vector3 const b)
  {
    return std::max({std::abs(a.x() - b.x()), std::abs(a.y() - b.y()), std::abs(a.z() - b.z())});
  }

  [[nodiscard]] double max_difference(Matrix3x3 const& a, Matrix3x3 const& b)
  {
    return std::max({
      max_difference(a.row(0), b.row(0)),
      max_difference(a.row(1), b.row(1)),
      max_difference(a.row(2), b.row(2)),
    });
  }

  [[nodiscard]] double max_difference(Quaternion const& a, Quaternion const& b)
  {
    return std::max(std::abs(a.w() - b.w()), max_difference(a.vector(), b.vector()));
  }

  template <typename T>
  class IsCloseTo : public Catch::Matchers::MatcherGenericBase
  {
   public:
    IsCloseTo(T const& expected, double const tolerance)
      : m_expected(expected)
      , m_tolerance(tolerance)
    {}

    [[nodiscard]] bool match(T const& actual) const
    {
      return max_difference(actual, this->m_expected) <= this->m_tolerance;
    }

    // Catch2 matchers override describe() publicly, as its documentation shows
    [[nodiscard]] std::string
      describe() const override  // NOLINT(misc-override-with-different-visibility)
    {
      return fmt::format("is within {} of {}", this->m_tolerance, this->m_expected);
    }

   private:
    T m_expected;
    double m_tolerance;
  };

  template <typename T>
  [[nodiscard]] IsCloseTo<T> is_close_to(T const& expected, double const tolerance = 1e-12)
  {
    return {expected, tolerance};
  }

  // rotations about skewed axes, so no component of a rotated vector is trivially zero
  [[nodiscard]] std::array<Quaternion, 4> sample_rotations()
  {
    return {
      Quaternion::from_axis_angle({1.0, 2.0, 3.0}, 0.7_rad),
      Quaternion::from_axis_angle({-0.3, 0.1, 0.9}, -2.1_rad),
      Quaternion::from_axis_angle({0.0, 1.0, -1.0}, 3_rad),
      Quaternion::from_axis_angle({4.0, -1.0, 0.5}, 0.01_rad),
    };
  }
}  // namespace

TEST_CASE("Vector3 components can be replaced and validated", "[geometry][linalg]")
{
  STATIC_CHECK(unit_x == Vector3(1.0, 0.0, 0.0));
  STATIC_CHECK(unit_y == Vector3(0.0, 1.0, 0.0));
  STATIC_CHECK(unit_z == Vector3(0.0, 0.0, 1.0));
  STATIC_CHECK(-unit_z == Vector3(0.0, 0.0, -1.0));
  STATIC_CHECK(Vector2::unit_x().cross(Vector2::unit_y()) == 1.0);

  STATIC_CHECK(sample_vector.with_x(1.0) == Vector3(1.0, -1.7, 2.4));
  STATIC_CHECK(sample_vector.with_y(1.0) == Vector3(0.3, 1.0, 2.4));
  STATIC_CHECK(sample_vector.with_z(1.0) == Vector3(0.3, -1.7, 1.0));
  STATIC_CHECK(Vector2(1.0, 2.0).with_y(5.0) == Vector2(1.0, 5.0));

  auto vector = Vector3();
  vector.set_x(1.0);
  vector.set_y(2.0);
  vector.set_z(3.0);
  CHECK(vector == Vector3(1.0, 2.0, 3.0));

  STATIC_CHECK(sample_vector.is_finite());
  STATIC_CHECK_FALSE(sample_vector.with_y(NaN).is_finite());
  STATIC_CHECK_FALSE(sample_vector.with_z(-Inf).is_finite());
  STATIC_CHECK_FALSE(Vector2(Inf, 0.0).is_finite());
}

TEST_CASE("Vectors keep their documented storage layout", "[geometry][linalg]")
{
  CHECK(std::bit_cast<std::array<double, 3>>(sample_vector) == std::array {0.3, -1.7, 2.4});
  CHECK(std::bit_cast<std::array<double, 2>>(Vector2(4.0, 5.0)) == std::array {4.0, 5.0});
  CHECK(
    std::bit_cast<std::array<double, 3>>(EulerAngles::from_radians(0.1, 0.2, 0.3))
    == std::array {0.1, 0.2, 0.3}
  );
  CHECK(
    std::bit_cast<std::array<Vector3, 3>>(inertia)
    == std::array {inertia.row(0), inertia.row(1), inertia.row(2)}
  );
  CHECK(
    std::bit_cast<std::array<double, 4>>(Quaternion(0.5, -0.5, 0.25, 1.0))
    == std::array {0.5, -0.5, 0.25, 1.0}
  );
}

TEST_CASE("Vector products are available as free functions", "[geometry][linalg]")
{
  auto constexpr a = Vector3(1.0, 2.0, 3.0);
  auto constexpr b = Vector3(-4.0, 0.5, 2.0);

  STATIC_CHECK(dot(a, b) == 3.0);
  STATIC_CHECK(cross(unit_x, unit_y) == unit_z);
  STATIC_CHECK(cross(unit_y, unit_z) == unit_x);
  STATIC_CHECK(cross(unit_z, unit_x) == unit_y);
  STATIC_CHECK(cross(sample_vector, a) == -cross(a, sample_vector));
  STATIC_CHECK(dot(Vector2(1.0, 2.0), Vector2(3.0, 4.0)) == 11.0);
  STATIC_CHECK(cross(Vector2(1.0, 0.0), Vector2(0.0, 1.0)) == 1.0);
}

TEST_CASE("try_normalized rejects degenerate vectors", "[geometry][linalg]")
{
  auto const unit = sample_vector.try_normalized();

  REQUIRE(unit.has_value());
  auto const normalized = unit.value_or(Vector3::zero());
  CHECK(normalized.length() == Catch::Approx(1.0));
  CHECK_THAT(normalized * sample_vector.length(), is_close_to(sample_vector));
  CHECK_FALSE(Vector3::zero().try_normalized().has_value());
  CHECK_FALSE(Vector3(NaN, 0.0, 0.0).try_normalized().has_value());
  CHECK_FALSE(Vector3(Inf, 0.0, 0.0).try_normalized().has_value());
  CHECK_FALSE(Vector2::zero().try_normalized().has_value());
  CHECK(Vector2(3.0, 4.0).try_normalized() == Vector2(0.6, 0.8));

  // normalized() keeps returning zero for a null vector
  CHECK(Vector3::zero().normalized() == Vector3::zero());
}

TEST_CASE("Vector formatting applies the format spec to every component", "[geometry][linalg]")
{
  CHECK(fmt::format("{}", sample_vector) == "(0.3, -1.7, 2.4)");
  CHECK(fmt::format("{:.2f}", sample_vector) == "(0.30, -1.70, 2.40)");
  CHECK(fmt::format("{:>5}", Vector2(1.0, 2.0)) == "(    1,     2)");
  CHECK(sample_vector.to_string() == "(0.3, -1.7, 2.4)");
}

TEST_CASE("Matrix accessors address rows, columns and elements", "[linalg][matrix]")
{
  auto constexpr m = Matrix3x3::from_rows({1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0});

  STATIC_CHECK(Matrix3x3() == Matrix3x3::zero());
  STATIC_CHECK(m.row(1) == Vector3(4.0, 5.0, 6.0));
  STATIC_CHECK(m.column(1) == Vector3(2.0, 5.0, 8.0));
  STATIC_CHECK(m.at(2, 0) == 7.0);
  STATIC_CHECK(m.diagonal() == Vector3(1.0, 5.0, 9.0));
  STATIC_CHECK(m.trace() == 15.0);
  STATIC_CHECK(Matrix3x3::from_columns(m.column(0), m.column(1), m.column(2)) == m);
  STATIC_CHECK(Matrix3x3::from_columns(m.row(0), m.row(1), m.row(2)) == m.transposed());
  STATIC_CHECK(Matrix3x3::diagonal({2.0, 3.0, 4.0}).diagonal() == Vector3(2.0, 3.0, 4.0));

  CHECK_THROWS_AS(m.row(3), std::out_of_range);
  CHECK_THROWS_AS(m.column(3), std::out_of_range);
  CHECK_THROWS_AS(m.at(0, 3), std::out_of_range);
  CHECK_THROWS_AS(m.at(3, 0), std::out_of_range);
}

TEST_CASE("Matrix products follow row-major linear algebra", "[linalg][matrix]")
{
  auto constexpr a = Matrix3x3::from_rows({1.0, 2.0, 3.0}, {0.0, 1.0, 4.0}, {5.0, 6.0, 0.0});

  STATIC_CHECK(a * unit_x == Vector3(1.0, 0.0, 5.0));
  STATIC_CHECK(Matrix3x3::identity() * sample_vector == sample_vector);
  STATIC_CHECK(Matrix3x3::identity() * a == a);
  STATIC_CHECK(a * Matrix3x3::identity() == a);
  STATIC_CHECK(
    Matrix3x3::diagonal({2.0, 3.0, 4.0}) * Vector3::splat(1.0) == Vector3(2.0, 3.0, 4.0)
  );
  STATIC_CHECK(Matrix3x3() * sample_vector == Vector3());

  // integer elements keep the products exact, so associativity holds bit for bit
  auto constexpr v = Vector3(1.0, -2.0, 3.0);
  STATIC_CHECK((a * a) * v == a * (a * v));
  STATIC_CHECK(a + a == 2.0 * a);
  STATIC_CHECK(a + a == a * 2.0);
  STATIC_CHECK((a * 2.0) / 2.0 == a);
  STATIC_CHECK(a - a == Matrix3x3());
  STATIC_CHECK(-a == -1.0 * a);
  STATIC_CHECK(+a == a);
  STATIC_CHECK(a.determinant() == 1.0);
}

TEST_CASE("Transpose and inverse satisfy their identities", "[linalg][matrix]")
{
  auto constexpr a = Matrix3x3::from_rows({1.0, 2.0, 3.0}, {0.0, 1.0, 4.0}, {5.0, 6.0, 0.0});

  STATIC_CHECK(a.transposed().transposed() == a);
  STATIC_CHECK((a * inertia).transposed() == inertia.transposed() * a.transposed());
  STATIC_CHECK(a.transposed().determinant() == a.determinant());
  // the inverse is constexpr, and exact for this unimodular integer matrix
  STATIC_CHECK(a.inverse() * a == Matrix3x3::identity());

  for(auto const& m : {a, inertia, 1e-6 * inertia, Matrix3x3::diagonal({2.0, -1.0, 0.5})}) {
    auto const inverse = m.inverse();

    CHECK(m.try_inverse() == inverse);
    CHECK_THAT(m * inverse, is_close_to(Matrix3x3::identity()));
    CHECK_THAT(inverse * m, is_close_to(Matrix3x3::identity()));
    CHECK_THAT(inverse.transposed(), is_close_to(m.transposed().inverse(), 1e-6));
  }
}

TEST_CASE("Inverting a singular or non-finite matrix fails", "[linalg][matrix]")
{
  auto constexpr dependent_rows = Matrix3x3::
    from_rows({1.0, 2.0, 3.0}, {2.0, 4.0, 6.0}, {0.0, 1.0, 1.0});
  auto const broken = Matrix3x3::diagonal({1.0, NaN, 1.0});

  STATIC_CHECK_FALSE(dependent_rows.is_invertible());
  STATIC_CHECK_FALSE(dependent_rows.try_inverse().has_value());
  CHECK_FALSE(Matrix3x3().try_inverse().has_value());
  CHECK_FALSE(broken.try_inverse().has_value());
  CHECK_THROWS_AS(Matrix3x3().inverse(), NumericError);
  CHECK_THROWS_AS(dependent_rows.inverse(), NumericError);
  CHECK_THROWS_AS(broken.inverse(), NumericError);
  CHECK_THROWS_AS(Matrix3x3::diagonal({1.0, Inf, 1.0}).inverse(), NumericError);

  // the singularity test is relative: tiny but well-conditioned matrices invert
  auto constexpr tiny = Matrix3x3::diagonal(Vector3::splat(1e-9));
  STATIC_CHECK(tiny.is_invertible());
  CHECK_NOTHROW(tiny.inverse());
  STATIC_CHECK(Matrix3x3::diagonal({1e-9, 1.0, 1e9}).is_invertible());
}

TEST_CASE("Cross-product matrix computes the cross product", "[linalg][matrix]")
{
  auto constexpr a = Vector3(1.0, 2.0, 3.0);

  STATIC_CHECK(Matrix3x3::cross_product(a) * sample_vector == a.cross(sample_vector));
  STATIC_CHECK(Matrix3x3::cross_product(a).transposed() == -Matrix3x3::cross_product(a));
}

TEST_CASE("Symmetry check accepts inertia tensors and rejects others", "[linalg][matrix]")
{
  STATIC_CHECK(inertia.is_symmetric());
  STATIC_CHECK(Matrix3x3().is_symmetric());

  // the inertia tensor in a rotated frame is symmetric up to rounding
  for(auto const& q : sample_rotations()) {
    auto const r = q.to_rotation_matrix();

    CHECK((r * inertia * r.transposed()).is_symmetric());
  }

  auto constexpr skewed = Matrix3x3::
    from_rows(inertia.row(0).with_y(inertia.at(0, 1) + 1e-6), inertia.row(1), inertia.row(2));
  STATIC_CHECK_FALSE(skewed.is_symmetric());
  STATIC_CHECK(skewed.is_symmetric(1e-6));
  CHECK_FALSE(
    Matrix3x3::from_rows(inertia.row(0), inertia.row(1).with_y(NaN), inertia.row(2)).is_symmetric()
  );
}

TEST_CASE("Matrices format, stream and hash", "[linalg][matrix]")
{
  CHECK(Matrix3x3::identity().to_string() == "[(1, 0, 0), (0, 1, 0), (0, 0, 1)]");
  CHECK(
    fmt::format("{:.1f}", Matrix3x3::diagonal({1.0, 2.0, 3.0}))
    == "[(1.0, 0.0, 0.0), (0.0, 2.0, 0.0), (0.0, 0.0, 3.0)]"
  );
  CHECK(Matrix3x3::identity().is_finite());
  CHECK_FALSE(Matrix3x3::diagonal({1.0, Inf, 1.0}).is_finite());

  auto const values = std::unordered_set<Matrix3x3> {inertia};
  CHECK(values.contains(inertia));
}

TEST_CASE("Hamilton product follows the quaternion basis rules", "[linalg][quaternion]")
{
  auto constexpr one = Quaternion();
  auto constexpr i = Quaternion(0.0, 1.0, 0.0, 0.0);
  auto constexpr j = Quaternion(0.0, 0.0, 1.0, 0.0);
  auto constexpr k = Quaternion(0.0, 0.0, 0.0, 1.0);

  STATIC_CHECK(one == Quaternion::identity());
  STATIC_CHECK(i * j == k);
  STATIC_CHECK(j * k == i);
  STATIC_CHECK(k * i == j);
  STATIC_CHECK(j * i == -k);
  STATIC_CHECK(i * i == -one);
  STATIC_CHECK(i * j * k == -one);

  auto constexpr q = Quaternion(0.5, -1.0, 2.0, 0.25);

  STATIC_CHECK(q * q.conjugate() == Quaternion(q.length_squared(), 0.0, 0.0, 0.0));
  STATIC_CHECK(q + q == 2.0 * q);
  STATIC_CHECK(q - q == Quaternion(0.0, Vector3()));
  STATIC_CHECK(q * 2.0 / 2.0 == q);
  STATIC_CHECK(Quaternion(0.5, Vector3(-1.0, 2.0, 0.25)) == q);
  STATIC_CHECK(q.vector() == Vector3(-1.0, 2.0, 0.25));
  STATIC_CHECK(q.is_finite());
  STATIC_CHECK_FALSE(Quaternion(NaN, 0.0, 0.0, 0.0).is_finite());
}

TEST_CASE("Axis-angle rotations are right-handed", "[linalg][quaternion]")
{
  auto const yaw = Quaternion::from_axis_angle(unit_z, 90_deg);

  CHECK_THAT(yaw.rotate(unit_x), is_close_to(unit_y));
  CHECK_THAT(yaw.rotate(unit_y), is_close_to(-unit_x));

  auto const about_x = Quaternion::from_axis_angle(2.0 * unit_x, 90_deg);

  CHECK_THAT(about_x.rotate(unit_y), is_close_to(unit_z));
  CHECK_THAT(Quaternion::from_axis_angle(unit_y, 0_rad), is_close_to(Quaternion()));
  CHECK_THROWS_AS(Quaternion::from_axis_angle(Vector3(), 1_rad), NumericError);
  CHECK_THROWS_AS(Quaternion::from_axis_angle(Vector3(NaN, 0, 0), 1_rad), NumericError);
}

TEST_CASE("Rotations compose as Hamilton products", "[linalg][quaternion]")
{
  for(auto const rotations = sample_rotations(); auto const& a : rotations) {
    for(auto const& b : rotations) {
      CHECK_THAT((a * b).rotate(sample_vector), is_close_to(a.rotate(b.rotate(sample_vector))));
      CHECK_THAT(
        (a * b).to_rotation_matrix(),
        is_close_to(a.to_rotation_matrix() * b.to_rotation_matrix())
      );
    }
  }
}

TEST_CASE(
  "Rotation matrix agrees with quaternion rotation and is orthonormal",
  "[linalg][quaternion]"
)
{
  for(auto const& q : sample_rotations()) {
    auto const r = q.to_rotation_matrix();

    CHECK_THAT(r * sample_vector, is_close_to(q.rotate(sample_vector)));
    CHECK_THAT(r.transposed() * sample_vector, is_close_to(q.inverse_rotate(sample_vector)));
    CHECK_THAT(q.inverse_rotate(q.rotate(sample_vector)), is_close_to(sample_vector));
    CHECK_THAT(r.transposed() * r, is_close_to(Matrix3x3::identity()));
    CHECK_THAT(r.inverse(), is_close_to(r.transposed()));
    CHECK(r.determinant() == Catch::Approx(1.0));
    CHECK_THAT((-q).to_rotation_matrix(), is_close_to(r));  // q and -q are the same rotation
  }
}

TEST_CASE("Rotation matrices convert back to their quaternion", "[linalg][quaternion]")
{
  auto rotations = std::vector<Quaternion>();
  for(auto const& q : sample_rotations())
    rotations.push_back(q);
  // half turns zero the trace, which exercises every branch of Shepperd's method
  for(auto const& axis : {unit_x, unit_y, unit_z, Vector3(1.0, 1.0, 0.0), Vector3(0.0, -1.0, 2.0)})
    rotations.push_back(Quaternion::from_axis_angle(axis, Angle::pi()));
  rotations.push_back(Quaternion::identity());

  for(auto const& q : rotations) {
    auto const r = q.to_rotation_matrix();
    auto const converted = Quaternion::from_rotation_matrix(r);

    CHECK(converted.length() == Catch::Approx(1.0));
    CHECK_THAT(converted.to_rotation_matrix(), is_close_to(r));
    // the sign is free, but the rotation is the same
    CHECK(std::abs(converted.dot(q)) == Catch::Approx(1.0));
  }
}

TEST_CASE("Normalizing a quaternion keeps its rotation", "[linalg][quaternion]")
{
  auto const q = sample_rotations().at(0);
  auto const drifted = 1.01 * q;

  CHECK(drifted.normalized().length() == Catch::Approx(1.0));
  CHECK_THAT(drifted.normalized(), is_close_to(q));
  CHECK(drifted.try_normalized() == drifted.normalized());
  CHECK_FALSE(Quaternion(0.0, 0.0, 0.0, 0.0).try_normalized().has_value());
  CHECK_THROWS_AS(Quaternion(0.0, 0.0, 0.0, 0.0).normalized(), NumericError);
  CHECK_THROWS_AS(Quaternion(NaN, 0.0, 0.0, 0.0).normalized(), NumericError);
}

TEST_CASE("Quaternion derivative integrates to the axis-angle rotation", "[linalg][quaternion]")
{
  auto constexpr omega_body = Vector3(0.4, -1.1, 2.3);
  auto constexpr duration = 2.0;
  auto constexpr steps = 2'000;
  constexpr auto dt = duration / steps;
  auto const initial = sample_rotations().at(1);

  // classic runge-kutta 4 on q' = derivative(q, omega), without renormalization
  auto q = initial;
  for(auto step = 0; step < steps; ++step) {
    auto const k1 = q.derivative(omega_body);
    auto const k2 = (q + dt / 2.0 * k1).derivative(omega_body);
    auto const k3 = (q + dt / 2.0 * k2).derivative(omega_body);
    auto const k4 = (q + dt * k3).derivative(omega_body);
    q += dt / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
  }

  // a constant body rate turns the body about the body-fixed axis: q(t) = q0 ⊗ q(ω t)
  auto const
    expected = initial
             * Quaternion::
                 from_axis_angle(omega_body, Angle::from_radians(omega_body.length() * duration));
  CHECK_THAT(q, is_close_to(expected, 1e-10));
  CHECK(q.length() == Catch::Approx(1.0).margin(1e-10));
  STATIC_CHECK(Quaternion().derivative(omega_body) == Quaternion(0.0, 0.2, -0.55, 1.15));
}

TEST_CASE("Euler angles turn the body FRD axes in NED as documented", "[linalg][quaternion]")
{
  // yaw +90°: the nose points east
  CHECK_THAT(
    Quaternion::from_euler_angles(EulerAngles::from_degrees(0.0, 0.0, 90.0)).rotate(unit_x),
    is_close_to(unit_y)
  );
  // pitch +90°: the nose points up, i.e. along -down
  CHECK_THAT(
    Quaternion::from_euler_angles(EulerAngles::from_degrees(0.0, 90.0, 0.0)).rotate(unit_x),
    is_close_to(-unit_z)
  );
  // roll +90°: the right side points down
  CHECK_THAT(
    Quaternion::from_euler_angles(EulerAngles::from_degrees(90.0, 0.0, 0.0)).rotate(unit_y),
    is_close_to(unit_z)
  );
}

TEST_CASE("Euler angles compose as yaw, then pitch, then roll", "[linalg][quaternion]")
{
  auto const roll = GENERATE(-170.0, -30.0, 0.0, 45.0, 179.0);
  auto const pitch = GENERATE(-89.0, -10.0, 0.0, 60.0);
  auto const yaw = GENERATE(-120.0, 0.0, 15.0, 180.0);
  auto const angles = EulerAngles::from_degrees(roll, pitch, yaw);

  auto const expected = Quaternion::from_axis_angle(unit_z, angles.yaw())
                      * Quaternion::from_axis_angle(unit_y, angles.pitch())
                      * Quaternion::from_axis_angle(unit_x, angles.roll());

  CHECK_THAT(Quaternion::from_euler_angles(angles), is_close_to(expected));
}

TEST_CASE("Euler angles round-trip away from the pitch singularity", "[linalg][quaternion]")
{
  auto const roll = GENERATE(-170.0, -30.0, 0.0, 45.0, 179.0);
  auto const pitch = GENERATE(-89.0, -10.0, 0.0, 60.0);
  auto const yaw = GENERATE(-120.0, 0.0, 15.0, 180.0);

  auto const angles = Quaternion::from_euler_angles(EulerAngles::from_degrees(roll, pitch, yaw))
                        .to_euler_angles();

  CHECK(angles.roll_degrees() == Catch::Approx(roll).margin(1e-9));
  CHECK(angles.pitch_degrees() == Catch::Approx(pitch).margin(1e-9));
  CHECK(std::remainder(angles.yaw_degrees() - yaw, 360.0) == Catch::Approx(0.0).margin(1e-9));
}

TEST_CASE(
  "Euler angles put the whole heading into yaw at the pitch singularity",
  "[linalg][quaternion]"
)
{
  auto const pitch = GENERATE(-90.0, 90.0);
  auto const attitude = Quaternion::from_euler_angles(EulerAngles::from_degrees(20.0, pitch, 50.0));
  auto const angles = attitude.to_euler_angles();

  REQUIRE(angles.is_finite());
  CHECK(angles.roll_degrees() == 0.0);
  CHECK(angles.pitch_degrees() == Catch::Approx(pitch));
  CHECK(angles.yaw_degrees() == Catch::Approx(pitch > 0.0 ? 30.0 : 70.0));
  CHECK_THAT(
    Quaternion::from_euler_angles(angles).to_rotation_matrix(),
    is_close_to(attitude.to_rotation_matrix(), 1e-12)
  );
}

TEST_CASE("Euler angles convert to and from quaternions", "[linalg][quaternion]")
{
  auto const angles = EulerAngles::from_degrees(-30.0, 10.0, 120.0);
  auto const attitude = angles.to_quaternion();

  CHECK(attitude == Quaternion::from_euler_angles(angles));
  CHECK(EulerAngles::from_quaternion(attitude) == attitude.to_euler_angles());

  auto const round_trip = EulerAngles::from_quaternion(attitude);
  CHECK(round_trip.roll_degrees() == Catch::Approx(-30.0));
  CHECK(round_trip.pitch_degrees() == Catch::Approx(10.0));
  CHECK(round_trip.yaw_degrees() == Catch::Approx(120.0));
  CHECK(EulerAngles::zero().to_quaternion() == Quaternion::identity());
}

TEST_CASE("Quaternions format, stream and hash", "[linalg][quaternion]")
{
  CHECK(Quaternion().to_string() == "(1, 0, 0, 0)");
  CHECK(fmt::format("{:.2f}", Quaternion(0.5, -0.5, 0.25, 1.0)) == "(0.50, -0.50, 0.25, 1.00)");

  auto const values = std::unordered_set<Quaternion> {Quaternion::identity()};
  CHECK(values.contains(Quaternion()));
}
