#pragma once

#include <string_view>

#include <radar/mms/common/config.h>
#include <radar/mms/common/constants.h>
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
 * @brief Pressure value stored in `Pa`.
 * @details Pressure is a small value type with SI, meteorological and aviation pressure units.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto qnh = 1013.25_hpa;
 * auto altimeter = qnh.as_inches_of_mercury(); // 29.92
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Force
 * @see Density
 * @see Temperature
 */
class RADAR_COMMON_API Pressure : public QuantityBase<Pressure>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double pascals READ as_pascals CONSTANT FINAL)
  Q_PROPERTY(double hectopascals READ as_hectopascals CONSTANT FINAL)
  Q_PROPERTY(double kilopascals READ as_kilopascals CONSTANT FINAL)
  Q_PROPERTY(double millibars READ as_millibars CONSTANT FINAL)
  Q_PROPERTY(double inchesOfMercury READ as_inches_of_mercury CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "Pa";

  /**
   * @brief Creates a zero pressure.
   */
  constexpr Pressure() noexcept = default;

  /**
   * @brief Creates a pressure from pascals.
   */
  explicit constexpr Pressure(double const pascals) noexcept
    : QuantityBase(pascals)
  {}

  /**
   * @brief Creates a pressure from pascals.
   */
  [[nodiscard]] static constexpr Pressure from_pascals(double const pascals) noexcept
  {
    return Pressure(pascals);
  }

  /**
   * @brief Creates a pressure from hectopascals.
   */
  [[nodiscard]] static constexpr Pressure from_hectopascals(double const hectopascals) noexcept
  {
    return Pressure(hectopascals * 100.0);
  }

  /**
   * @brief Creates a pressure from kilopascals.
   */
  [[nodiscard]] static constexpr Pressure from_kilopascals(double const kilopascals) noexcept
  {
    return Pressure(kilopascals * 1'000.0);
  }

  /**
   * @brief Creates a pressure from millibars.
   */
  [[nodiscard]] static constexpr Pressure from_millibars(double const millibars) noexcept
  {
    return Pressure(millibars * 100.0);
  }

  /**
   * @brief Creates a pressure from conventional inches of mercury.
   */
  [[nodiscard]] static constexpr Pressure from_inches_of_mercury(
    double const inches_of_mercury
  ) noexcept
  {
    return Pressure(inches_of_mercury * INHG_PASCAL_FACTOR);
  }

  /**
   * @brief Returns pascals.
   */
  [[nodiscard]] constexpr double as_pascals() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns hectopascals.
   */
  [[nodiscard]] constexpr double as_hectopascals() const noexcept
  {
    return this->magnitude() / 100.0;
  }

  /**
   * @brief Returns kilopascals.
   */
  [[nodiscard]] constexpr double as_kilopascals() const noexcept
  {
    return this->magnitude() / 1'000.0;
  }

  /**
   * @brief Returns millibars.
   */
  [[nodiscard]] constexpr double as_millibars() const noexcept { return this->magnitude() / 100.0; }

  /**
   * @brief Returns conventional inches of mercury.
   */
  [[nodiscard]] constexpr double as_inches_of_mercury() const noexcept
  {
    return this->magnitude() / INHG_PASCAL_FACTOR;
  }
};

inline namespace literals
{
  /**
   * @brief Creates a pressure from a pascal literal.
   * @ingroup core_measurements
   * @relates Pressure
   */
  [[nodiscard]] constexpr Pressure operator""_pa(long double const value) noexcept
  {
    return Pressure::from_pascals(static_cast<double>(value));
  }

  /**
   * @brief Creates a pressure from a pascal literal.
   * @ingroup core_measurements
   * @relates Pressure
   */
  [[nodiscard]] constexpr Pressure operator""_pa(unsigned long long const value) noexcept
  {
    return Pressure::from_pascals(static_cast<double>(value));
  }

  /**
   * @brief Creates a pressure from a hectopascal literal.
   * @ingroup core_measurements
   * @relates Pressure
   */
  [[nodiscard]] constexpr Pressure operator""_hpa(long double const value) noexcept
  {
    return Pressure::from_hectopascals(static_cast<double>(value));
  }

  /**
   * @brief Creates a pressure from a hectopascal literal.
   * @ingroup core_measurements
   * @relates Pressure
   */
  [[nodiscard]] constexpr Pressure operator""_hpa(unsigned long long const value) noexcept
  {
    return Pressure::from_hectopascals(static_cast<double>(value));
  }

  /**
   * @brief Creates a pressure from a kilopascal literal.
   * @ingroup core_measurements
   * @relates Pressure
   */
  [[nodiscard]] constexpr Pressure operator""_kpa(long double const value) noexcept
  {
    return Pressure::from_kilopascals(static_cast<double>(value));
  }

  /**
   * @brief Creates a pressure from a kilopascal literal.
   * @ingroup core_measurements
   * @relates Pressure
   */
  [[nodiscard]] constexpr Pressure operator""_kpa(unsigned long long const value) noexcept
  {
    return Pressure::from_kilopascals(static_cast<double>(value));
  }

  /**
   * @brief Creates a pressure from a millibar literal.
   * @ingroup core_measurements
   * @relates Pressure
   */
  [[nodiscard]] constexpr Pressure operator""_mbar(long double const value) noexcept
  {
    return Pressure::from_millibars(static_cast<double>(value));
  }

  /**
   * @brief Creates a pressure from a millibar literal.
   * @ingroup core_measurements
   * @relates Pressure
   */
  [[nodiscard]] constexpr Pressure operator""_mbar(unsigned long long const value) noexcept
  {
    return Pressure::from_millibars(static_cast<double>(value));
  }

  /**
   * @brief Creates a pressure from an inch-of-mercury literal.
   * @ingroup core_measurements
   * @relates Pressure
   */
  [[nodiscard]] constexpr Pressure operator""_inhg(long double const value) noexcept
  {
    return Pressure::from_inches_of_mercury(static_cast<double>(value));
  }

  /**
   * @brief Creates a pressure from an inch-of-mercury literal.
   * @ingroup core_measurements
   * @relates Pressure
   */
  [[nodiscard]] constexpr Pressure operator""_inhg(unsigned long long const value) noexcept
  {
    return Pressure::from_inches_of_mercury(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Pressure)
#endif
