#include <radar/mms/common/quaternion.h>

#include <cmath>
#include <numbers>
#include <ostream>

#include <fmt/format.h>

RADAR_COMMON_NAMESPACE_BEGIN

Quaternion Quaternion::normalized() const
{
  if(auto const result = this->try_normalized())
    return *result;
  throw NumericError("cannot normalize the quaternion {}", *this);
}

EulerAngles Quaternion::to_euler_angles() const noexcept
{
  auto const w = this->m_w;
  auto const x = this->m_x;
  auto const y = this->m_y;
  auto const z = this->m_z;

  // the last row of the rotation matrix: (-sin pitch, sin roll cos pitch, cos roll cos pitch)
  auto const sin_pitch = 2.0 * (w * y - x * z);
  auto const sin_roll_cos_pitch = 2.0 * (w * x + y * z);
  auto const cos_roll_cos_pitch = 1.0 - 2.0 * (x * x + y * y);
  auto const cos_pitch = std::hypot(sin_roll_cos_pitch, cos_roll_cos_pitch);

  // below this cosine of the pitch, rounding noise of the quaternion outweighs the roll and yaw
  // terms, which scale with it; about the square root of the double epsilon
  if(constexpr auto singular_cos_pitch = 1e-8; cos_pitch < singular_cos_pitch)
    return EulerAngles::from_radians(
      0.0,
      std::copysign(std::numbers::pi / 2.0, sin_pitch),
      std::atan2(2.0 * (w * z - x * y), 1.0 - 2.0 * (x * x + z * z))
    );
  return EulerAngles::from_radians(
    std::atan2(sin_roll_cos_pitch, cos_roll_cos_pitch),
    std::atan2(sin_pitch, cos_pitch),
    std::atan2(2.0 * (w * z + x * y), 1.0 - 2.0 * (y * y + z * z))
  );
}

#ifdef RCOMMON_WITH_QT
Quaternion::Quaternion(QQuaternion const& quaternion) noexcept
  : m_w(quaternion.scalar())
  , m_x(quaternion.x())
  , m_y(quaternion.y())
  , m_z(quaternion.z())
{}

QQuaternion Quaternion::to_qquaternion() const noexcept
{
  return {
    static_cast<float>(this->m_w),
    static_cast<float>(this->m_x),
    static_cast<float>(this->m_y),
    static_cast<float>(this->m_z)
  };
}
#endif

std::string Quaternion::to_string() const { return fmt::format("{}", *this); }

#ifdef RCOMMON_WITH_QT
QString Quaternion::to_qstring() const { return QString::fromStdString(this->to_string()); }
#endif

Quaternion Quaternion::from_axis_angle(Vector3 const axis, Angle const angle)
{
  auto const unit = axis.try_normalized();
  if(not unit.has_value())
    throw NumericError("cannot rotate about the axis {}", axis);

  auto const half = angle / 2.0;
  return {half.cos(), *unit * half.sin()};
}

Quaternion Quaternion::from_euler_angles(EulerAngles const& angles) noexcept
{
  auto const half_roll = angles.roll_radians() / 2.0;
  auto const half_pitch = angles.pitch_radians() / 2.0;
  auto const half_yaw = angles.yaw_radians() / 2.0;
  auto const cr = std::cos(half_roll);
  auto const sr = std::sin(half_roll);
  auto const cp = std::cos(half_pitch);
  auto const sp = std::sin(half_pitch);
  auto const cy = std::cos(half_yaw);
  auto const sy = std::sin(half_yaw);
  return {
    cr * cp * cy + sr * sp * sy,
    sr * cp * cy - cr * sp * sy,
    cr * sp * cy + sr * cp * sy,
    cr * cp * sy - sr * sp * cy
  };
}

Quaternion Quaternion::from_rotation_matrix(Matrix3x3 const& matrix) noexcept
{
  auto const [m00, m01, m02] = std::array {matrix.row(0).x(), matrix.row(0).y(), matrix.row(0).z()};
  auto const [m10, m11, m12] = std::array {matrix.row(1).x(), matrix.row(1).y(), matrix.row(1).z()};
  auto const [m20, m21, m22] = std::array {matrix.row(2).x(), matrix.row(2).y(), matrix.row(2).z()};

  // Shepperd's method: derive the largest of |w|, |x|, |y|, |z| from the diagonal, so the
  // divisor of the other three components is never small
  auto const trace = m00 + m11 + m22;
  auto result = Quaternion();
  if(trace > 0.0) {
    auto const s = 2.0 * std::sqrt(1.0 + trace);
    result = {s / 4.0, (m21 - m12) / s, (m02 - m20) / s, (m10 - m01) / s};
  }
  else if(m00 > m11 and m00 > m22) {
    auto const s = 2.0 * std::sqrt(1.0 + m00 - m11 - m22);
    result = {(m21 - m12) / s, s / 4.0, (m01 + m10) / s, (m02 + m20) / s};
  }
  else if(m11 > m22) {
    auto const s = 2.0 * std::sqrt(1.0 + m11 - m00 - m22);
    result = {(m02 - m20) / s, (m01 + m10) / s, s / 4.0, (m12 + m21) / s};
  }
  else {
    auto const s = 2.0 * std::sqrt(1.0 + m22 - m00 - m11);
    result = {(m10 - m01) / s, (m02 + m20) / s, (m12 + m21) / s, s / 4.0};
  }
  return result / result.length();
}

#ifdef RCOMMON_WITH_QT
Quaternion Quaternion::from_qquaternion(QQuaternion const& quaternion) noexcept
{
  return Quaternion(quaternion);
}
#endif

std::ostream& operator<<(std::ostream& os, Quaternion const& quaternion)
{
  return os << quaternion.to_string();
}

RADAR_COMMON_NAMESPACE_END
