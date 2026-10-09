#pragma once

#include <string_view>

#include <radar/mms/common/config.h>
#include <radar/mms/common/detail/export.h>
#include <radar/mms/common/detail/namespace.h>
#include <radar/mms/common/quantity.h>

#ifdef RCOMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#endif

RADAR_COMMON_NAMESPACE_BEGIN

/**
 * @brief Torque value stored in `N*m`.
 * @details Torque is a small value type stored in newton-meters.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto load = 1.5_nm * 2.0; // 3 N*m
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Force
 * @see AngularAcceleration
 * @see MomentOfInertia
 */
class RADAR_COMMON_API Torque : public QuantityBase<Torque>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double newtonMeters READ as_newton_meters CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "N*m";

  /**
   * @brief Creates a zero torque.
   */
  constexpr Torque() noexcept = default;

  /**
   * @brief Creates a torque from newton-meters.
   */
  explicit constexpr Torque(double const newton_meters) noexcept
    : QuantityBase(newton_meters)
  {}

  /**
   * @brief Creates a torque from newton-meters.
   */
  [[nodiscard]] static constexpr Torque from_newton_meters(double const newton_meters) noexcept
  {
    return Torque(newton_meters);
  }

  /**
   * @brief Returns newton-meters.
   */
  [[nodiscard]] constexpr double as_newton_meters() const noexcept { return this->magnitude(); }
};

inline namespace literals
{
  /**
   * @brief Creates a torque from a newton-meter literal.
   * @ingroup core_measurements
   * @relates Torque
   */
  [[nodiscard]] constexpr Torque operator""_nm(long double const value) noexcept
  {
    return Torque::from_newton_meters(static_cast<double>(value));
  }

  /**
   * @brief Creates a torque from a newton-meter literal.
   * @ingroup core_measurements
   * @relates Torque
   */
  [[nodiscard]] constexpr Torque operator""_nm(unsigned long long const value) noexcept
  {
    return Torque::from_newton_meters(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Torque)
#endif
