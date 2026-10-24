#pragma once

#include <string_view>

#include <whs31/common/config.h>
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
 * @brief Capacitance value stored in `F`.
 * @details Capacitance is a small value type with farad and microfarad conversions.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when wcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace whs31::literals;
 *
 * auto bus = 470_uf; // 0.00047 F
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Charge
 * @see Voltage
 */
class WHS31_COMMON_API Capacitance : public QuantityBase<Capacitance>
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double farads READ as_farads CONSTANT FINAL)
  Q_PROPERTY(double microfarads READ as_microfarads CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "F";

  /**
   * @brief Creates a zero capacitance.
   */
  constexpr Capacitance() noexcept = default;

  /**
   * @brief Creates a capacitance from farads.
   */
  explicit constexpr Capacitance(double const farads) noexcept
    : QuantityBase(farads)
  {}

  /**
   * @brief Creates a capacitance from farads.
   */
  [[nodiscard]] static constexpr Capacitance from_farads(double const farads) noexcept
  {
    return Capacitance(farads);
  }

  /**
   * @brief Creates a capacitance from microfarads.
   */
  [[nodiscard]] static constexpr Capacitance from_microfarads(double const microfarads) noexcept
  {
    return Capacitance(microfarads / 1'000'000.0);
  }

  /**
   * @brief Returns farads.
   */
  [[nodiscard]] constexpr double as_farads() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns microfarads.
   */
  [[nodiscard]] constexpr double as_microfarads() const noexcept
  {
    return this->magnitude() * 1'000'000.0;
  }
};

inline namespace literals
{
  /**
   * @brief Creates a capacitance from a farad literal.
   * @ingroup core_measurements
   * @relates Capacitance
   */
  [[nodiscard]] constexpr Capacitance operator""_f(long double const value) noexcept
  {
    return Capacitance::from_farads(static_cast<double>(value));
  }

  /**
   * @brief Creates a capacitance from a farad literal.
   * @ingroup core_measurements
   * @relates Capacitance
   */
  [[nodiscard]] constexpr Capacitance operator""_f(unsigned long long const value) noexcept
  {
    return Capacitance::from_farads(static_cast<double>(value));
  }

  /**
   * @brief Creates a capacitance from a microfarad literal.
   * @ingroup core_measurements
   * @relates Capacitance
   */
  [[nodiscard]] constexpr Capacitance operator""_uf(long double const value) noexcept
  {
    return Capacitance::from_microfarads(static_cast<double>(value));
  }

  /**
   * @brief Creates a capacitance from a microfarad literal.
   * @ingroup core_measurements
   * @relates Capacitance
   */
  [[nodiscard]] constexpr Capacitance operator""_uf(unsigned long long const value) noexcept
  {
    return Capacitance::from_microfarads(static_cast<double>(value));
  }
}  // namespace literals

WHS31_COMMON_NAMESPACE_END

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::Capacitance)
#endif
