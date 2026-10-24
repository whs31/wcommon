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
 * @brief Mass value stored in `kg`.
 * @details Mass is a small value type with kilogram and gram conversions.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when wcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace whs31::literals;
 *
 * auto payload = 2.5_kg + 300_g; // 2.8 kg
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Force
 * @see Density
 * @see MomentOfInertia
 */
class WHS31_COMMON_API Mass : public QuantityBase<Mass>
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double kilograms READ as_kilograms CONSTANT FINAL)
  Q_PROPERTY(double grams READ as_grams CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "kg";

  /**
   * @brief Creates a zero mass.
   */
  constexpr Mass() noexcept = default;

  /**
   * @brief Creates a mass from kilograms.
   */
  explicit constexpr Mass(double const kilograms) noexcept
    : QuantityBase(kilograms)
  {}

  /**
   * @brief Creates a mass from kilograms.
   */
  [[nodiscard]] static constexpr Mass from_kilograms(double const kilograms) noexcept
  {
    return Mass(kilograms);
  }

  /**
   * @brief Creates a mass from grams.
   */
  [[nodiscard]] static constexpr Mass from_grams(double const grams) noexcept
  {
    return Mass(grams / 1'000.0);
  }

  /**
   * @brief Returns kilograms.
   */
  [[nodiscard]] constexpr double as_kilograms() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns grams.
   */
  [[nodiscard]] constexpr double as_grams() const noexcept { return this->magnitude() * 1'000.0; }
};

inline namespace literals
{
  /**
   * @brief Creates a mass from a kilogram literal.
   * @ingroup core_measurements
   * @relates Mass
   */
  [[nodiscard]] constexpr Mass operator""_kg(long double const value) noexcept
  {
    return Mass::from_kilograms(static_cast<double>(value));
  }

  /**
   * @brief Creates a mass from a kilogram literal.
   * @ingroup core_measurements
   * @relates Mass
   */
  [[nodiscard]] constexpr Mass operator""_kg(unsigned long long const value) noexcept
  {
    return Mass::from_kilograms(static_cast<double>(value));
  }

  /**
   * @brief Creates a mass from a gram literal.
   * @ingroup core_measurements
   * @relates Mass
   */
  [[nodiscard]] constexpr Mass operator""_g(long double const value) noexcept
  {
    return Mass::from_grams(static_cast<double>(value));
  }

  /**
   * @brief Creates a mass from a gram literal.
   * @ingroup core_measurements
   * @relates Mass
   */
  [[nodiscard]] constexpr Mass operator""_g(unsigned long long const value) noexcept
  {
    return Mass::from_grams(static_cast<double>(value));
  }
}  // namespace literals

WHS31_COMMON_NAMESPACE_END

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::Mass)
#endif
