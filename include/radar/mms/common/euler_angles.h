#pragma once

#include <compare>
#include <functional>
#include <iosfwd>
#include <ostream>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <radar/mms/common/angle.h>
#include <radar/mms/common/config.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Roll, pitch, and yaw rotation angles.
 * @details Values are stored as three Angle objects. The `*Radians` and
 * `*Degrees` properties are provided for convenient QML use.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @ingroup core_measurements
 * @see Angle
 */
class RADAR_COMMON_API EulerAngles
{
#ifdef RCOMMON_WITH_QT
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

  [[nodiscard]] std::string to_string() const;

#ifdef RCOMMON_WITH_QT
  /// @note Available only when rcommon is built with `with_qt=True`.
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

RADAR_COMMON_API std::ostream& operator<<(std::ostream& os, EulerAngles const& angles);

RADAR_COMMON_NAMESPACE_END

template <>
struct std::hash<radar::mms::common::EulerAngles>
{
  [[nodiscard]] std::size_t operator()(radar::mms::common::EulerAngles const& angles) const noexcept
  {
    auto hash = std::hash<radar::mms::common::Angle> {}(angles.roll());
    hash ^= std::hash<radar::mms::common::Angle> {}(angles.pitch()) + 0x9e3779b97f4a7c15ull
          + (hash << 6) + (hash >> 2);
    hash ^= std::hash<radar::mms::common::Angle> {}(angles.yaw()) + 0x9e3779b97f4a7c15ull
          + (hash << 6) + (hash >> 2);
    return hash;
  }
};

#ifndef __DOXYGEN__
template <>
struct fmt::formatter<radar::mms::common::EulerAngles> : fmt::ostream_formatter
{};
#endif

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::EulerAngles)
#endif
