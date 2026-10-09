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
 * @brief Voltage value stored in `V`.
 * @details Voltage is a small value type with volt and millivolt conversions.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto cell = 3.7_v;
 * auto pack = cell * 6.0; // 22.2 V
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Current
 * @see Resistance
 * @see Power
 */
class RADAR_COMMON_API Voltage : public QuantityBase<Voltage>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double volts READ as_volts CONSTANT FINAL)
  Q_PROPERTY(double millivolts READ as_millivolts CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "V";

  /**
   * @brief Creates a zero voltage.
   */
  constexpr Voltage() noexcept = default;

  /**
   * @brief Creates a voltage from volts.
   */
  explicit constexpr Voltage(double const volts) noexcept
    : QuantityBase(volts)
  {}

  /**
   * @brief Creates a voltage from volts.
   */
  [[nodiscard]] static constexpr Voltage from_volts(double const volts) noexcept
  {
    return Voltage(volts);
  }

  /**
   * @brief Creates a voltage from millivolts.
   */
  [[nodiscard]] static constexpr Voltage from_millivolts(double const millivolts) noexcept
  {
    return Voltage(millivolts / 1'000.0);
  }

  /**
   * @brief Returns volts.
   */
  [[nodiscard]] constexpr double as_volts() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns millivolts.
   */
  [[nodiscard]] constexpr double as_millivolts() const noexcept
  {
    return this->magnitude() * 1'000.0;
  }
};

inline namespace literals
{
  /**
   * @brief Creates a voltage from a volt literal.
   * @ingroup core_measurements
   * @relates Voltage
   */
  [[nodiscard]] constexpr Voltage operator""_v(long double const value) noexcept
  {
    return Voltage::from_volts(static_cast<double>(value));
  }

  /**
   * @brief Creates a voltage from a volt literal.
   * @ingroup core_measurements
   * @relates Voltage
   */
  [[nodiscard]] constexpr Voltage operator""_v(unsigned long long const value) noexcept
  {
    return Voltage::from_volts(static_cast<double>(value));
  }

  /**
   * @brief Creates a voltage from a millivolt literal.
   * @ingroup core_measurements
   * @relates Voltage
   */
  [[nodiscard]] constexpr Voltage operator""_mv(long double const value) noexcept
  {
    return Voltage::from_millivolts(static_cast<double>(value));
  }

  /**
   * @brief Creates a voltage from a millivolt literal.
   * @ingroup core_measurements
   * @relates Voltage
   */
  [[nodiscard]] constexpr Voltage operator""_mv(unsigned long long const value) noexcept
  {
    return Voltage::from_millivolts(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Voltage)
#endif
