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
 * @brief Power value stored in `W`.
 * @details Power is a small value type with watt and kilowatt conversions.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto hover = 1.8_kw + 150_w; // 1950 W
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Energy
 * @see Voltage
 * @see Current
 */
class RADAR_COMMON_API Power : public QuantityBase<Power>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double watts READ as_watts CONSTANT FINAL)
  Q_PROPERTY(double kilowatts READ as_kilowatts CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "W";

  /**
   * @brief Creates a zero power.
   */
  constexpr Power() noexcept = default;

  /**
   * @brief Creates a power from watts.
   */
  explicit constexpr Power(double const watts) noexcept
    : QuantityBase(watts)
  {}

  /**
   * @brief Creates a power from watts.
   */
  [[nodiscard]] static constexpr Power from_watts(double const watts) noexcept
  {
    return Power(watts);
  }

  /**
   * @brief Creates a power from kilowatts.
   */
  [[nodiscard]] static constexpr Power from_kilowatts(double const kilowatts) noexcept
  {
    return Power(kilowatts * 1'000.0);
  }

  /**
   * @brief Returns watts.
   */
  [[nodiscard]] constexpr double as_watts() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns kilowatts.
   */
  [[nodiscard]] constexpr double as_kilowatts() const noexcept
  {
    return this->magnitude() / 1'000.0;
  }
};

inline namespace literals
{
  /**
   * @brief Creates a power from a watt literal.
   * @ingroup core_measurements
   * @relates Power
   */
  [[nodiscard]] constexpr Power operator""_w(long double const value) noexcept
  {
    return Power::from_watts(static_cast<double>(value));
  }

  /**
   * @brief Creates a power from a watt literal.
   * @ingroup core_measurements
   * @relates Power
   */
  [[nodiscard]] constexpr Power operator""_w(unsigned long long const value) noexcept
  {
    return Power::from_watts(static_cast<double>(value));
  }

  /**
   * @brief Creates a power from a kilowatt literal.
   * @ingroup core_measurements
   * @relates Power
   */
  [[nodiscard]] constexpr Power operator""_kw(long double const value) noexcept
  {
    return Power::from_kilowatts(static_cast<double>(value));
  }

  /**
   * @brief Creates a power from a kilowatt literal.
   * @ingroup core_measurements
   * @relates Power
   */
  [[nodiscard]] constexpr Power operator""_kw(unsigned long long const value) noexcept
  {
    return Power::from_kilowatts(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Power)
#endif
