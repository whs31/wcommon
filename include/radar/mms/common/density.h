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
 * @brief Density value stored in `kg/m^3`.
 * @details Density is a small value type stored in kilograms per cubic meter.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto sea_level_air = 1.225_kg_per_m3;
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Mass
 * @see Pressure
 * @see Temperature
 */
class RADAR_COMMON_API Density : public QuantityBase<Density>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double kilogramsPerCubicMeter READ as_kilograms_per_cubic_meter CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "kg/m^3";

  /**
   * @brief Creates a zero density.
   */
  constexpr Density() noexcept = default;

  /**
   * @brief Creates a density from kilograms per cubic meter.
   */
  explicit constexpr Density(double const kilograms_per_cubic_meter) noexcept
    : QuantityBase(kilograms_per_cubic_meter)
  {}

  /**
   * @brief Creates a density from kilograms per cubic meter.
   */
  [[nodiscard]] static constexpr Density from_kilograms_per_cubic_meter(
    double const kilograms_per_cubic_meter
  ) noexcept
  {
    return Density(kilograms_per_cubic_meter);
  }

  /**
   * @brief Returns kilograms per cubic meter.
   */
  [[nodiscard]] constexpr double as_kilograms_per_cubic_meter() const noexcept
  {
    return this->magnitude();
  }
};

inline namespace literals
{
  /**
   * @brief Creates a density from a kilogram-per-cubic-meter literal.
   * @ingroup core_measurements
   * @relates Density
   */
  [[nodiscard]] constexpr Density operator""_kg_per_m3(long double const value) noexcept
  {
    return Density::from_kilograms_per_cubic_meter(static_cast<double>(value));
  }

  /**
   * @brief Creates a density from a kilogram-per-cubic-meter literal.
   * @ingroup core_measurements
   * @relates Density
   */
  [[nodiscard]] constexpr Density operator""_kg_per_m3(unsigned long long const value) noexcept
  {
    return Density::from_kilograms_per_cubic_meter(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Density)
#endif
