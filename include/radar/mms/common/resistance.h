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
 * @brief Resistance value stored in `ohm`.
 * @details Resistance is a small value type with ohm and milliohm conversions. The
 * `_mohm` literal is a milliohm, not a megaohm.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto winding = 45_mohm; // 0.045 ohm
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Conductance
 * @see Voltage
 * @see Current
 */
class RADAR_COMMON_API Resistance : public QuantityBase<Resistance>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double ohms READ as_ohms CONSTANT FINAL)
  Q_PROPERTY(double milliohms READ as_milliohms CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "ohm";

  /**
   * @brief Creates a zero resistance.
   */
  constexpr Resistance() noexcept = default;

  /**
   * @brief Creates a resistance from ohms.
   */
  explicit constexpr Resistance(double const ohms) noexcept
    : QuantityBase(ohms)
  {}

  /**
   * @brief Creates a resistance from ohms.
   */
  [[nodiscard]] static constexpr Resistance from_ohms(double const ohms) noexcept
  {
    return Resistance(ohms);
  }

  /**
   * @brief Creates a resistance from milliohms.
   */
  [[nodiscard]] static constexpr Resistance from_milliohms(double const milliohms) noexcept
  {
    return Resistance(milliohms / 1'000.0);
  }

  /**
   * @brief Returns ohms.
   */
  [[nodiscard]] constexpr double as_ohms() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns milliohms.
   */
  [[nodiscard]] constexpr double as_milliohms() const noexcept
  {
    return this->magnitude() * 1'000.0;
  }
};

inline namespace literals
{
  /**
   * @brief Creates a resistance from an ohm literal.
   * @ingroup core_measurements
   * @relates Resistance
   */
  [[nodiscard]] constexpr Resistance operator""_ohm(long double const value) noexcept
  {
    return Resistance::from_ohms(static_cast<double>(value));
  }

  /**
   * @brief Creates a resistance from an ohm literal.
   * @ingroup core_measurements
   * @relates Resistance
   */
  [[nodiscard]] constexpr Resistance operator""_ohm(unsigned long long const value) noexcept
  {
    return Resistance::from_ohms(static_cast<double>(value));
  }

  /**
   * @brief Creates a resistance from a milliohm literal.
   * @ingroup core_measurements
   * @relates Resistance
   */
  [[nodiscard]] constexpr Resistance operator""_mohm(long double const value) noexcept
  {
    return Resistance::from_milliohms(static_cast<double>(value));
  }

  /**
   * @brief Creates a resistance from a milliohm literal.
   * @ingroup core_measurements
   * @relates Resistance
   */
  [[nodiscard]] constexpr Resistance operator""_mohm(unsigned long long const value) noexcept
  {
    return Resistance::from_milliohms(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Resistance)
#endif
