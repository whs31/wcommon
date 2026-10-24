#pragma once

#include <array>
#include <bit>
#include <compare>
#include <functional>
#include <iosfwd>
#include <ostream>
#include <string>
#include <type_traits>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <whs31/common/angle.h>
#include <whs31/common/config.h>
#include <whs31/common/detail/export.h>
#include <whs31/common/detail/namespace.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#endif

WHS31_COMMON_NAMESPACE_BEGIN

class Quaternion;

/**
 * @brief Roll, pitch, and yaw rotation angles.
 * @details Values are stored as three Angle objects. The `*Radians` and
 * `*Degrees` properties are provided for convenient QML use.
 *
 * @par Layout
 * The storage is exactly three contiguous Angle values in `roll, pitch, yaw` order, each a single
 * double in radians, with no padding. The class is standard layout and trivially copyable, so it
 * can be reflected by offset or bit-cast to `std::array<double, 3>` of radians. `Q_GADGET` adds no
 * data members and does not affect the layout. The layout is part of the API and is checked by
 * `static_assert`s in this header.
 *
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when wcommon is built with `with_qt=True`.
 *
 * @ingroup core_measurements
 * @see Angle
 * @see Quaternion
 */
class WHS31_COMMON_API EulerAngles
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(Angle roll READ roll CONSTANT FINAL)
  Q_PROPERTY(Angle pitch READ pitch CONSTANT FINAL)
  Q_PROPERTY(Angle yaw READ yaw CONSTANT FINAL)
  Q_PROPERTY(double rollRadians READ roll_radians CONSTANT FINAL)
  Q_PROPERTY(double pitchRadians READ pitch_radians CONSTANT FINAL)
  Q_PROPERTY(double yawRadians READ yaw_radians CONSTANT FINAL)
  Q_PROPERTY(double rollDegrees READ roll_degrees CONSTANT FINAL)
  Q_PROPERTY(double pitchDegrees READ pitch_degrees CONSTANT FINAL)
  Q_PROPERTY(double yawDegrees READ yaw_degrees CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  constexpr EulerAngles() noexcept = default;

  constexpr EulerAngles(Angle const roll, Angle const pitch, Angle const yaw) noexcept
    : m_roll(roll)
    , m_pitch(pitch)
    , m_yaw(yaw)
  {}

  [[nodiscard]] static constexpr EulerAngles from_angles(
    Angle const roll,
    Angle const pitch,
    Angle const yaw
  ) noexcept
  {
    return {roll, pitch, yaw};
  }

  [[nodiscard]] static constexpr EulerAngles from_radians(
    double const roll,
    double const pitch,
    double const yaw
  ) noexcept
  {
    return {Angle::from_radians(roll), Angle::from_radians(pitch), Angle::from_radians(yaw)};
  }

  [[nodiscard]] static constexpr EulerAngles from_degrees(
    double const roll,
    double const pitch,
    double const yaw
  ) noexcept
  {
    return {Angle::from_degrees(roll), Angle::from_degrees(pitch), Angle::from_degrees(yaw)};
  }

  [[nodiscard]] static constexpr EulerAngles zero() noexcept { return {}; }

  /**
   * @brief Returns the aerospace ZYX Euler angles of the unit quaternion @p attitude.
   * @details Same as Quaternion::to_euler_angles(): roll and yaw are in `[-180°, 180°]`, pitch in
   * `[-90°, 90°]`, and the roll is zero at ±90° pitch.
   * @note Include `whs31/common/quaternion.h` to call it; this header only declares
   * Quaternion.
   * @see to_quaternion
   */
  [[nodiscard]] static EulerAngles from_quaternion(Quaternion const& attitude) noexcept;

  [[nodiscard]] constexpr Angle roll() const noexcept { return this->m_roll; }

  [[nodiscard]] constexpr Angle pitch() const noexcept { return this->m_pitch; }

  [[nodiscard]] constexpr Angle yaw() const noexcept { return this->m_yaw; }

  [[nodiscard]] constexpr double roll_radians() const noexcept { return this->m_roll.radians(); }

  [[nodiscard]] constexpr double pitch_radians() const noexcept { return this->m_pitch.radians(); }

  [[nodiscard]] constexpr double yaw_radians() const noexcept { return this->m_yaw.radians(); }

  [[nodiscard]] constexpr double roll_degrees() const noexcept { return this->m_roll.degrees(); }

  [[nodiscard]] constexpr double pitch_degrees() const noexcept { return this->m_pitch.degrees(); }

  [[nodiscard]] constexpr double yaw_degrees() const noexcept { return this->m_yaw.degrees(); }

  [[nodiscard]] bool is_finite() const noexcept
  {
    return this->m_roll.is_finite() and this->m_pitch.is_finite() and this->m_yaw.is_finite();
  }

  /**
   * @brief Returns the attitude of these aerospace ZYX Euler angles.
   * @details Same as Quaternion::from_euler_angles(): `q = q_z(yaw) ⊗ q_y(pitch) ⊗ q_x(roll)`.
   * @note Include `whs31/common/quaternion.h` to call it; this header only declares
   * Quaternion.
   * @see from_quaternion
   */
  [[nodiscard]] Quaternion to_quaternion() const noexcept;

  [[nodiscard]] std::string to_string() const;

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] constexpr bool operator==(EulerAngles const&) const noexcept = default;

  [[nodiscard]] constexpr std::partial_ordering operator<=>(EulerAngles const&) const noexcept =
    default;

 private:
  Angle m_roll;
  Angle m_pitch;
  Angle m_yaw;
};

WHS31_COMMON_API std::ostream& operator<<(std::ostream& os, EulerAngles const& angles);

static_assert(std::is_standard_layout_v<EulerAngles>);
static_assert(std::is_trivially_copyable_v<EulerAngles>);
static_assert(sizeof(EulerAngles) == 3 * sizeof(double));
static_assert(
  std::bit_cast<std::array<double, 3>>(EulerAngles::from_radians(1.0, 2.0, 3.0))
    == std::array {1.0, 2.0, 3.0},
  "EulerAngles must store roll, pitch, yaw radians contiguously"
);

WHS31_COMMON_NAMESPACE_END

template <>
struct std::hash<whs31::common::EulerAngles>
{
  [[nodiscard]] std::size_t operator()(whs31::common::EulerAngles const& angles) const noexcept
  {
    auto hash = std::hash<whs31::common::Angle> {}(angles.roll());
    hash ^= std::hash<whs31::common::Angle> {}(angles.pitch()) + 0x9e3779b97f4a7c15ull + (hash << 6)
          + (hash >> 2);
    hash ^= std::hash<whs31::common::Angle> {}(angles.yaw()) + 0x9e3779b97f4a7c15ull + (hash << 6)
          + (hash >> 2);
    return hash;
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<whs31::common::EulerAngles> : fmt::ostream_formatter
{};
#endif

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::EulerAngles)
#endif
