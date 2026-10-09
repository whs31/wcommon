#pragma once

#include <string_view>

#include <radar/mms/common/config.h>
#include <radar/mms/common/constants.h>
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
 * @brief Energy value stored in `J`.
 * @details Energy is a small value type with joule, watt-hour and kilowatt-hour conversions.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto battery = 222_wh;
 * auto joules = battery.as_joules(); // 799200
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Power
 * @see Charge
 */
class RADAR_COMMON_API Energy : public QuantityBase<Energy>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double joules READ as_joules CONSTANT FINAL)
  Q_PROPERTY(double wattHours READ as_watt_hours CONSTANT FINAL)
  Q_PROPERTY(double kilowattHours READ as_kilowatt_hours CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "J";

  /**
   * @brief Creates a zero energy.
   */
  constexpr Energy() noexcept = default;

  /**
   * @brief Creates an energy from joules.
   */
  explicit constexpr Energy(double const joules) noexcept
    : QuantityBase(joules)
  {}

  /**
   * @brief Creates an energy from joules.
   */
  [[nodiscard]] static constexpr Energy from_joules(double const joules) noexcept
  {
    return Energy(joules);
  }

  /**
   * @brief Creates an energy from watt-hours.
   */
  [[nodiscard]] static constexpr Energy from_watt_hours(double const watt_hours) noexcept
  {
    return Energy(watt_hours * SECONDS_HOURS_FACTOR);
  }

  /**
   * @brief Creates an energy from kilowatt-hours.
   */
  [[nodiscard]] static constexpr Energy from_kilowatt_hours(double const kilowatt_hours) noexcept
  {
    return Energy(kilowatt_hours * (SECONDS_HOURS_FACTOR * 1'000.0));
  }

  /**
   * @brief Returns joules.
   */
  [[nodiscard]] constexpr double as_joules() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns watt-hours.
   */
  [[nodiscard]] constexpr double as_watt_hours() const noexcept
  {
    return this->magnitude() / SECONDS_HOURS_FACTOR;
  }

  /**
   * @brief Returns kilowatt-hours.
   */
  [[nodiscard]] constexpr double as_kilowatt_hours() const noexcept
  {
    return this->magnitude() / (SECONDS_HOURS_FACTOR * 1'000.0);
  }
};

inline namespace literals
{
  /**
   * @brief Creates an energy from a joule literal.
   * @ingroup core_measurements
   * @relates Energy
   */
  [[nodiscard]] constexpr Energy operator""_j(long double const value) noexcept
  {
    return Energy::from_joules(static_cast<double>(value));
  }

  /**
   * @brief Creates an energy from a joule literal.
   * @ingroup core_measurements
   * @relates Energy
   */
  [[nodiscard]] constexpr Energy operator""_j(unsigned long long const value) noexcept
  {
    return Energy::from_joules(static_cast<double>(value));
  }

  /**
   * @brief Creates an energy from a watt-hour literal.
   * @ingroup core_measurements
   * @relates Energy
   */
  [[nodiscard]] constexpr Energy operator""_wh(long double const value) noexcept
  {
    return Energy::from_watt_hours(static_cast<double>(value));
  }

  /**
   * @brief Creates an energy from a watt-hour literal.
   * @ingroup core_measurements
   * @relates Energy
   */
  [[nodiscard]] constexpr Energy operator""_wh(unsigned long long const value) noexcept
  {
    return Energy::from_watt_hours(static_cast<double>(value));
  }

  /**
   * @brief Creates an energy from a kilowatt-hour literal.
   * @ingroup core_measurements
   * @relates Energy
   */
  [[nodiscard]] constexpr Energy operator""_kwh(long double const value) noexcept
  {
    return Energy::from_kilowatt_hours(static_cast<double>(value));
  }

  /**
   * @brief Creates an energy from a kilowatt-hour literal.
   * @ingroup core_measurements
   * @relates Energy
   */
  [[nodiscard]] constexpr Energy operator""_kwh(unsigned long long const value) noexcept
  {
    return Energy::from_kilowatt_hours(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Energy)
#endif
