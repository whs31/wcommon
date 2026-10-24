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
 * @brief Current value stored in `A`.
 * @details Current is a small value type with ampere and milliampere conversions.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when wcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace whs31::literals;
 *
 * auto load = 12_a + 350_ma; // 12.35 A
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Voltage
 * @see Charge
 * @see Power
 */
class WHS31_COMMON_API Current : public QuantityBase<Current>
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double amperes READ as_amperes CONSTANT FINAL)
  Q_PROPERTY(double milliamperes READ as_milliamperes CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "A";

  /**
   * @brief Creates a zero current.
   */
  constexpr Current() noexcept = default;

  /**
   * @brief Creates a current from amperes.
   */
  explicit constexpr Current(double const amperes) noexcept
    : QuantityBase(amperes)
  {}

  /**
   * @brief Creates a current from amperes.
   */
  [[nodiscard]] static constexpr Current from_amperes(double const amperes) noexcept
  {
    return Current(amperes);
  }

  /**
   * @brief Creates a current from milliamperes.
   */
  [[nodiscard]] static constexpr Current from_milliamperes(double const milliamperes) noexcept
  {
    return Current(milliamperes / 1'000.0);
  }

  /**
   * @brief Returns amperes.
   */
  [[nodiscard]] constexpr double as_amperes() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns milliamperes.
   */
  [[nodiscard]] constexpr double as_milliamperes() const noexcept
  {
    return this->magnitude() * 1'000.0;
  }
};

inline namespace literals
{
  /**
   * @brief Creates a current from an ampere literal.
   * @ingroup core_measurements
   * @relates Current
   */
  [[nodiscard]] constexpr Current operator""_a(long double const value) noexcept
  {
    return Current::from_amperes(static_cast<double>(value));
  }

  /**
   * @brief Creates a current from an ampere literal.
   * @ingroup core_measurements
   * @relates Current
   */
  [[nodiscard]] constexpr Current operator""_a(unsigned long long const value) noexcept
  {
    return Current::from_amperes(static_cast<double>(value));
  }

  /**
   * @brief Creates a current from a milliampere literal.
   * @ingroup core_measurements
   * @relates Current
   */
  [[nodiscard]] constexpr Current operator""_ma(long double const value) noexcept
  {
    return Current::from_milliamperes(static_cast<double>(value));
  }

  /**
   * @brief Creates a current from a milliampere literal.
   * @ingroup core_measurements
   * @relates Current
   */
  [[nodiscard]] constexpr Current operator""_ma(unsigned long long const value) noexcept
  {
    return Current::from_milliamperes(static_cast<double>(value));
  }
}  // namespace literals

WHS31_COMMON_NAMESPACE_END

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::Current)
#endif
