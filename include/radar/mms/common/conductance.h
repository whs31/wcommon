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
 * @brief Conductance value stored in `S`.
 * @details Conductance is a small value type stored in siemens. Its literal is `_siemens`
 * rather than `_s`, which would read as seconds next to `std::chrono` literals.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto load = 0.25_siemens;
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Resistance
 * @see Current
 */
class RADAR_COMMON_API Conductance : public QuantityBase<Conductance>
{
#ifdef RCOMMON_WITH_QT
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

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Conductance)
#endif
