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
 * @brief Conductance value stored in `S`.
 * @details Conductance is a small value type stored in siemens. Its literal is `_siemens`
 * rather than `_s`, which would read as seconds next to `std::chrono` literals.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when wcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace whs31::literals;
 *
 * auto load = 0.25_siemens;
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Resistance
 * @see Current
 */
class WHS31_COMMON_API Conductance : public QuantityBase<Conductance>
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double siemens READ as_siemens CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "S";

  /**
   * @brief Creates a zero conductance.
   */
  constexpr Conductance() noexcept = default;

  /**
   * @brief Creates a conductance from siemens.
   */
  explicit constexpr Conductance(double const siemens) noexcept
    : QuantityBase(siemens)
  {}

  /**
   * @brief Creates a conductance from siemens.
   */
  [[nodiscard]] static constexpr Conductance from_siemens(double const siemens) noexcept
  {
    return Conductance(siemens);
  }

  /**
   * @brief Returns siemens.
   */
  [[nodiscard]] constexpr double as_siemens() const noexcept { return this->magnitude(); }
};

inline namespace literals
{
  /**
   * @brief Creates a conductance from a siemens literal.
   * @ingroup core_measurements
   * @relates Conductance
   */
  [[nodiscard]] constexpr Conductance operator""_siemens(long double const value) noexcept
  {
    return Conductance::from_siemens(static_cast<double>(value));
  }

  /**
   * @brief Creates a conductance from a siemens literal.
   * @ingroup core_measurements
   * @relates Conductance
   */
  [[nodiscard]] constexpr Conductance operator""_siemens(unsigned long long const value) noexcept
  {
    return Conductance::from_siemens(static_cast<double>(value));
  }
}  // namespace literals

WHS31_COMMON_NAMESPACE_END

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::Conductance)
#endif
