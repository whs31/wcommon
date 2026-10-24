#pragma once

#include <array>
#include <bit>
#include <cmath>
#include <functional>
#include <iosfwd>
#include <optional>
#include <ostream>
#include <string>
#include <type_traits>

#include <fmt/format.h>

#include <whs31/common/angle.h>
#include <whs31/common/config.h>
#include <whs31/common/detail/export.h>
#include <whs31/common/detail/format.h>
#include <whs31/common/detail/namespace.h>
#include <whs31/common/error.h>
#include <whs31/common/euler_angles.h>
#include <whs31/common/math.h>
#include <whs31/common/matrix3x3.h>
#include <whs31/common/vector3.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qquaternion.h>
#  include <qstring.h>
#endif

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Hamilton quaternion of doubles, scalar first, used for rotations and attitudes.
 * @details The default value is the identity rotation.
 *
 * @par Conventions
 * - **Hamilton product**: `i² = j² = k² = ijk = −1`, so `i * j == k`. The product `a * b` is the
 *   rotation `b` followed by the rotation `a`.
 * - **Active rotation**: rotate() turns the vector, not the frame:
 *   `q.rotate(v) == q ⊗ (0, v) ⊗ q*`. Rotations are right-handed about their axis.
 * - **Body-to-reference attitude**: an attitude rotates vectors from the frame it describes into
 *   the reference frame: `v_ned = attitude_body_to_ned.rotate(v_body)` and
 *   `v_body = attitude_body_to_ned.inverse_rotate(v_ned)`. Compositions chain the frame names:
 *   `attitude_a_to_c = attitude_b_to_c * attitude_a_to_b`.
 * - to_rotation_matrix() is the same rotation as a matrix: `m * v == q.rotate(v)`.
 * - `q` and `−q` are the same rotation; no sign of `w` is enforced.
 * - Euler angles use the aerospace ZYX sequence, see from_euler_angles().
 *
 * Only unit quaternions are rotations. Integrating derivative() drifts off the unit sphere;
 * renormalize after each step with normalized().
 *
 * @par Layout
 * The storage is exactly four contiguous doubles in `w, x, y, z` order, with no padding. The class
 * is standard layout and trivially copyable, so it can be reflected by offset or bit-cast to
 * `std::array<double, 4>`. `Q_GADGET` adds no data members and does not affect the layout. The
 * layout is part of the API and is checked by `static_assert`s in this header.
 *
 * @code {.cpp}
 * using namespace whs31;
 *
 * auto attitude_body_to_ned = Quaternion::from_euler_angles(EulerAngles::from_degrees(0, 5, 90));
 * auto const nose_ned = attitude_body_to_ned.rotate(Vector3::unit_x());  // about east
 *
 * // one explicit Euler step of the attitude kinematics
 * auto const omega_body = Vector3(0.0, 0.0, 0.1);
 * attitude_body_to_ned += 0.01 * attitude_body_to_ned.derivative(omega_body);
 * attitude_body_to_ned = attitude_body_to_ned.normalized();
 * @endcode
 *
 * @note Qt properties, conversions, and metatype registration are available only when wcommon is
 * built with `with_qt=True`.
 * @ingroup core_math
 * @see Matrix3x3
 * @see EulerAngles
 */
class WHS31_COMMON_API Quaternion
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double w READ w CONSTANT FINAL)
  Q_PROPERTY(double x READ x CONSTANT FINAL)
  Q_PROPERTY(double y READ y CONSTANT FINAL)
  Q_PROPERTY(double z READ z CONSTANT FINAL)
  Q_PROPERTY(double length READ length CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Creates the identity rotation.
   */
  constexpr Quaternion() noexcept = default;

  constexpr Quaternion(double const w, double const x, double const y, double const z) noexcept
    : m_w(w)
    , m_x(x)
    , m_y(y)
    , m_z(z)
  {}

  /**
   * @brief Creates the quaternion with the scalar part @p w and the vector part @p vector.
   */
  constexpr Quaternion(double const w, Vector3 const vector) noexcept
    : m_w(w)
    , m_x(vector.x())
    , m_y(vector.y())
    , m_z(vector.z())
  {}

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  explicit Quaternion(QQuaternion const& quaternion) noexcept;
#endif

  [[nodiscard]] constexpr double w() const noexcept { return this->m_w; }

  [[nodiscard]] constexpr double x() const noexcept { return this->m_x; }

  [[nodiscard]] constexpr double y() const noexcept { return this->m_y; }

  [[nodiscard]] constexpr double z() const noexcept { return this->m_z; }

  /**
   * @brief Returns the vector part `(x, y, z)`.
   */
  [[nodiscard]] constexpr Vector3 vector() const noexcept
  {
    return {this->m_x, this->m_y, this->m_z};
  }

  [[nodiscard]] constexpr Quaternion conjugate() const noexcept
  {
    return {this->m_w, -this->m_x, -this->m_y, -this->m_z};
  }

  [[nodiscard]] constexpr double dot(Quaternion const& other) const noexcept
  {
    return this->m_w * other.m_w + this->m_x * other.m_x + this->m_y * other.m_y
         + this->m_z * other.m_z;
  }

  [[nodiscard]] constexpr double length_squared() const noexcept { return this->dot(*this); }

  [[nodiscard]] double length() const noexcept { return std::sqrt(this->length_squared()); }

  /**
   * @brief Checks whether every component is neither infinite nor NaN.
   */
  [[nodiscard]] constexpr bool is_finite() const noexcept
  {
    return common::is_finite(this->m_w) and common::is_finite(this->m_x)
       and common::is_finite(this->m_y) and common::is_finite(this->m_z);
  }

  /**
   * @brief Returns the unit quaternion of the same rotation, or `std::nullopt` when the length
   * is zero or not finite.
   * @see normalized
   */
  [[nodiscard]] std::optional<Quaternion> try_normalized() const noexcept
  {
    auto const len = this->length();
    if(not positive(len))
      return std::nullopt;
    return Quaternion(this->m_w / len, this->m_x / len, this->m_y / len, this->m_z / len);
  }

  /**
   * @brief Returns the unit quaternion of the same rotation.
   * @throws NumericError if the length is zero or not finite.
   * @see try_normalized
   */
  [[nodiscard]] Quaternion normalized() const;

  /**
   * @brief Rotates @p v by this unit quaternion: `q ⊗ (0, v) ⊗ q*`.
   * @see inverse_rotate
   */
  [[nodiscard]] constexpr Vector3 rotate(Vector3 const v) const noexcept
  {
    auto const u = this->vector();
    auto const t = 2.0 * u.cross(v);
    return v + this->m_w * t + u.cross(t);
  }

  /**
   * @brief Rotates @p v by the inverse of this unit quaternion: `q* ⊗ (0, v) ⊗ q`.
   * @see rotate
   */
  [[nodiscard]] constexpr Vector3 inverse_rotate(Vector3 const v) const noexcept
  {
    return this->conjugate().rotate(v);
  }

  /**
   * @brief Returns the rotation matrix `m` of this unit quaternion: `m * v == rotate(v)`.
   * @details The inverse rotation is the transpose of the matrix.
   */
  [[nodiscard]] constexpr Matrix3x3 to_rotation_matrix() const noexcept
  {
    auto const w = this->m_w;
    auto const x = this->m_x;
    auto const y = this->m_y;
    auto const z = this->m_z;
    return Matrix3x3::from_rows(
      {1.0 - 2.0 * (y * y + z * z), 2.0 * (x * y - w * z), 2.0 * (x * z + w * y)},
      {2.0 * (x * y + w * z), 1.0 - 2.0 * (x * x + z * z), 2.0 * (y * z - w * x)},
      {2.0 * (x * z - w * y), 2.0 * (y * z + w * x), 1.0 - 2.0 * (x * x + y * y)}
    );
  }

  /**
   * @brief Returns the ZYX Euler angles of this unit quaternion.
   * @details Roll and yaw are in `[-180°, 180°]`, pitch in `[-90°, 90°]`. At ±90° pitch only the
   * sum or difference of roll and yaw is defined; there the roll is zero and the yaw carries the
   * whole rotation about the vertical.
   * @see from_euler_angles
   */
  [[nodiscard]] EulerAngles to_euler_angles() const noexcept;

  /**
   * @brief Returns the time derivative `q̇ = ½ q ⊗ (0, ω)` of an attitude turning at the angular
   * velocity @p omega_body.
   * @details @p omega_body is the angular velocity of the body relative to the reference frame,
   * in body axes, in radians per second. The result is per second.
   */
  [[nodiscard]] constexpr Quaternion derivative(Vector3 omega_body) const noexcept;

#ifdef WHS31_COMMON_WITH_QT
  /**
   * @brief Converts to a Qt quaternion.
   * @details The components are narrowed to `float`.
   * @note Available only when wcommon is built with `with_qt=True`.
   */
  [[nodiscard]] QQuaternion to_qquaternion() const noexcept;
#endif

  /**
   * @brief Returns the components as `(w, x, y, z)`.
   */
  [[nodiscard]] std::string to_string() const;

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] static constexpr Quaternion identity() noexcept { return {}; }

  [[nodiscard]] static constexpr Quaternion from_wxyz(
    double const w,
    double const x,
    double const y,
    double const z
  ) noexcept
  {
    return {w, x, y, z};
  }

  /**
   * @brief Returns the right-handed rotation by @p angle about @p axis.
   * @details @p axis need not be a unit vector.
   * @throws NumericError if @p axis is zero or not finite.
   */
  [[nodiscard]] static Quaternion from_axis_angle(Vector3 axis, Angle angle);

  /**
   * @brief Returns the attitude of the aerospace ZYX Euler angles.
   * @details `q = q_z(yaw) ⊗ q_y(pitch) ⊗ q_x(roll)`: yaw about the reference z axis, then pitch
   * about the new y axis, then roll about the new x axis. With NED reference axes and FRD body
   * axes, positive yaw turns the nose right, positive pitch raises it, and positive roll lowers
   * the right side.
   * @see to_euler_angles
   */
  [[nodiscard]] static Quaternion from_euler_angles(EulerAngles const& angles) noexcept;

  /**
   * @brief Returns the unit quaternion of the rotation matrix @p matrix.
   * @details Uses Shepperd's method, which stays accurate for every rotation. The result is
   * normalized, so a matrix that is orthonormal up to rounding gives a unit quaternion.
   * @pre @p matrix is a rotation matrix: orthonormal with determinant `+1`.
   * @see to_rotation_matrix
   */
  [[nodiscard]] static Quaternion from_rotation_matrix(Matrix3x3 const& matrix) noexcept;

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] static Quaternion from_qquaternion(QQuaternion const& quaternion) noexcept;
#endif

  [[nodiscard]] constexpr bool operator==(Quaternion const&) const noexcept = default;

  [[nodiscard]] constexpr Quaternion operator+() const noexcept { return *this; }

  [[nodiscard]] constexpr Quaternion operator-() const noexcept
  {
    return {-this->m_w, -this->m_x, -this->m_y, -this->m_z};
  }

  constexpr Quaternion& operator+=(Quaternion const& rhs) noexcept
  {
    this->m_w += rhs.m_w;
    this->m_x += rhs.m_x;
    this->m_y += rhs.m_y;
    this->m_z += rhs.m_z;
    return *this;
  }

  constexpr Quaternion& operator-=(Quaternion const& rhs) noexcept
  {
    this->m_w -= rhs.m_w;
    this->m_x -= rhs.m_x;
    this->m_y -= rhs.m_y;
    this->m_z -= rhs.m_z;
    return *this;
  }

  constexpr Quaternion& operator*=(double const rhs) noexcept
  {
    this->m_w *= rhs;
    this->m_x *= rhs;
    this->m_y *= rhs;
    this->m_z *= rhs;
    return *this;
  }

  constexpr Quaternion& operator/=(double const rhs) noexcept
  {
    this->m_w /= rhs;
    this->m_x /= rhs;
    this->m_y /= rhs;
    this->m_z /= rhs;
    return *this;
  }

 private:
  double m_w = 1.0;
  double m_x = 0.0;
  double m_y = 0.0;
  double m_z = 0.0;
};

[[nodiscard]] constexpr Quaternion operator+(Quaternion lhs, Quaternion const& rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Quaternion operator-(Quaternion lhs, Quaternion const& rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Quaternion operator*(Quaternion lhs, double const rhs) noexcept
{
  lhs *= rhs;
  return lhs;
}

[[nodiscard]] constexpr Quaternion operator*(double const lhs, Quaternion rhs) noexcept
{
  rhs *= lhs;
  return rhs;
}

[[nodiscard]] constexpr Quaternion operator/(Quaternion lhs, double const rhs) noexcept
{
  lhs /= rhs;
  return lhs;
}

/**
 * @brief Returns the Hamilton product `a ⊗ b`: the rotation `b` followed by `a`.
 * @relates Quaternion
 */
[[nodiscard]] constexpr Quaternion operator*(Quaternion const& a, Quaternion const& b) noexcept
{
  return {
    a.w() * b.w() - a.x() * b.x() - a.y() * b.y() - a.z() * b.z(),
    a.w() * b.x() + a.x() * b.w() + a.y() * b.z() - a.z() * b.y(),
    a.w() * b.y() - a.x() * b.z() + a.y() * b.w() + a.z() * b.x(),
    a.w() * b.z() + a.x() * b.y() - a.y() * b.x() + a.z() * b.w()
  };
}

constexpr Quaternion Quaternion::derivative(Vector3 const omega_body) const noexcept
{
  return 0.5 * (*this * Quaternion(0.0, omega_body));
}

WHS31_COMMON_API std::ostream& operator<<(std::ostream& os, Quaternion const& quaternion);

static_assert(std::is_standard_layout_v<Quaternion>);
static_assert(std::is_trivially_copyable_v<Quaternion>);
static_assert(sizeof(Quaternion) == 4 * sizeof(double));
static_assert(
  std::bit_cast<std::array<double, 4>>(Quaternion(1.0, 2.0, 3.0, 4.0))
    == std::array {1.0, 2.0, 3.0, 4.0},
  "Quaternion must store w, x, y, z contiguously"
);

WHS31_COMMON_NAMESPACE_END

template <>
struct std::hash<whs31::common::Quaternion>
{
  [[nodiscard]] std::size_t operator()(whs31::common::Quaternion const& q) const noexcept
  {
    auto hash = std::hash<double> {}(q.w());
    for(auto const value : {q.x(), q.y(), q.z()})
      hash ^= std::hash<double> {}(value) + 0x9e3779b97f4a7c15ull + (hash << 6U) + (hash >> 2U);
    return hash;
  }
};

#ifndef __DOXYGEN__
/**
 * @brief Formats as `(w, x, y, z)`; the format spec, e.g. `{:.3f}`, applies to every component.
 */
template <>
struct fmt::formatter<whs31::common::Quaternion> : fmt::formatter<double>
{
  format_context::iterator format(whs31::common::Quaternion const& q, format_context& ctx) const
  {
    return whs31::common::_format_impl::format_components<
      4>(*this, {q.w(), q.x(), q.y(), q.z()}, ctx);
  }
};
#endif

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::Quaternion)
#endif
