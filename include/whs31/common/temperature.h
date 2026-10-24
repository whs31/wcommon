#pragma once

#include <string_view>

#include <whs31/common/config.h>
#include <whs31/common/constants.h>
#include <whs31/common/detail/export.h>
#include <whs31/common/detail/namespace.h>
#include <whs31/common/quantity.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#endif

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief Temperature value stored in `K`.
 * @details Temperature is an absolute thermodynamic temperature stored in kelvin, with
 * Celsius and Fahrenheit conversions.
 *
 * The difference of two temperatures is also a Temperature, in kelvin. It is correct as a
 * kelvin or Celsius *difference*, but its as_celsius() and as_fahrenheit() are meaningless, since
 * those scales are offset from absolute zero.
 *
 * Negation is deleted, so a negative Celsius or Fahrenheit literal such as `-5_degc`, which C++
 * parses as `-(5_degc)`, does not compile. Write `Temperature::from_celsius(-5.0)` instead.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when wcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace whs31::literals;
 *
 * auto ambient = 15_degc;
 * auto kelvin = ambient.as_kelvin(); // 288.15
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Pressure
 * @see Density
 */
class WHS31_COMMON_API Temperature : public QuantityBase<Temperature>
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double kelvin READ as_kelvin CONSTANT FINAL)
  Q_PROPERTY(double celsius READ as_celsius CONSTANT FINAL)
  Q_PROPERTY(double fahrenheit READ as_fahrenheit CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "K";

  /**
   * @brief Creates a zero temperature.
   */
  constexpr Temperature() noexcept = default;

  /**
   * @brief Creates a temperature from kelvin.
   */
  explicit constexpr Temperature(double const kelvin) noexcept
    : QuantityBase(kelvin)
  {}

  /**
   * @brief Creates a temperature from kelvin.
   */
  [[nodiscard]] static constexpr Temperature from_kelvin(double const kelvin) noexcept
  {
    return Temperature(kelvin);
  }

  /**
   * @brief Creates a temperature from degrees Celsius.
   */
  [[nodiscard]] static constexpr Temperature from_celsius(double const celsius) noexcept
  {
    return Temperature(celsius + CELSIUS_ZERO_KELVIN);
  }

  /**
   * @brief Creates a temperature from degrees Fahrenheit.
   */
  [[nodiscard]] static constexpr Temperature from_fahrenheit(double const fahrenheit) noexcept
  {
    return Temperature(((fahrenheit - 32.0) * 5.0 / 9.0) + CELSIUS_ZERO_KELVIN);
  }

  /**
   * @brief Returns kelvin.
   */
  [[nodiscard]] constexpr double as_kelvin() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns degrees Celsius.
   */
  [[nodiscard]] constexpr double as_celsius() const noexcept
  {
    return this->magnitude() - CELSIUS_ZERO_KELVIN;
  }

  /**
   * @brief Returns degrees Fahrenheit.
   */
  [[nodiscard]] constexpr double as_fahrenheit() const noexcept
  {
    return ((this->magnitude() - CELSIUS_ZERO_KELVIN) * 9.0 / 5.0) + 32.0;
  }

  /**
   * @brief Deleted: the negation of an absolute temperature has no meaning.
   * @details It also rejects literals such as `-5_degc`, which C++ parses as `-(5_degc)`.
   */
  // NOLINTNEXTLINE(bugprone-derived-method-shadowing-base-method)
  Temperature operator-() const = delete;
};

inline namespace literals
{
  /**
   * @brief Creates a temperature from a kelvin literal.
   * @ingroup core_measurements
   * @relates Temperature
   */
  [[nodiscard]] constexpr Temperature operator""_k(long double const value) noexcept
  {
    return Temperature::from_kelvin(static_cast<double>(value));
  }

  /**
   * @brief Creates a temperature from a kelvin literal.
   * @ingroup core_measurements
   * @relates Temperature
   */
  [[nodiscard]] constexpr Temperature operator""_k(unsigned long long const value) noexcept
  {
    return Temperature::from_kelvin(static_cast<double>(value));
  }

  /**
   * @brief Creates a temperature from a degree-Celsius literal.
   * @ingroup core_measurements
   * @relates Temperature
   */
  [[nodiscard]] constexpr Temperature operator""_degc(long double const value) noexcept
  {
    return Temperature::from_celsius(static_cast<double>(value));
  }

  /**
   * @brief Creates a temperature from a degree-Celsius literal.
   * @ingroup core_measurements
   * @relates Temperature
   */
  [[nodiscard]] constexpr Temperature operator""_degc(unsigned long long const value) noexcept
  {
    return Temperature::from_celsius(static_cast<double>(value));
  }

  /**
   * @brief Creates a temperature from a degree-Fahrenheit literal.
   * @ingroup core_measurements
   * @relates Temperature
   */
  [[nodiscard]] constexpr Temperature operator""_degf(long double const value) noexcept
  {
    return Temperature::from_fahrenheit(static_cast<double>(value));
  }

  /**
   * @brief Creates a temperature from a degree-Fahrenheit literal.
   * @ingroup core_measurements
   * @relates Temperature
   */
  [[nodiscard]] constexpr Temperature operator""_degf(unsigned long long const value) noexcept
  {
    return Temperature::from_fahrenheit(static_cast<double>(value));
  }
}  // namespace literals

WHS31_COMMON_NAMESPACE_END

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::Temperature)
#endif
