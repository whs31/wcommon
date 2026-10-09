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
 * @brief Charge value stored in `C`.
 * @details Charge is an electric charge stored in coulombs, with ampere-hour and
 * milliampere-hour conversions for battery capacities.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto capacity = 5000_mah;
 * auto amp_hours = capacity.as_ampere_hours(); // 5
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Current
 * @see Energy
 * @see Capacitance
 */
class RADAR_COMMON_API Charge : public QuantityBase<Charge>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double coulombs READ as_coulombs CONSTANT FINAL)
  Q_PROPERTY(double ampereHours READ as_ampere_hours CONSTANT FINAL)
  Q_PROPERTY(double milliampereHours READ as_milliampere_hours CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "C";

  /**
   * @brief Creates a zero charge.
   */
  constexpr Charge() noexcept = default;

  /**
   * @brief Creates a charge from coulombs.
   */
  explicit constexpr Charge(double const coulombs) noexcept
    : QuantityBase(coulombs)
  {}

  /**
   * @brief Creates a charge from coulombs.
   */
  [[nodiscard]] static constexpr Charge from_coulombs(double const coulombs) noexcept
  {
    return Charge(coulombs);
  }

  /**
   * @brief Creates a charge from ampere-hours.
   */
  [[nodiscard]] static constexpr Charge from_ampere_hours(double const ampere_hours) noexcept
  {
    return Charge(ampere_hours * SECONDS_HOURS_FACTOR);
  }

  /**
   * @brief Creates a charge from milliampere-hours.
   */
  [[nodiscard]] static constexpr Charge from_milliampere_hours(
    double const milliampere_hours
  ) noexcept
  {
    return Charge(milliampere_hours * (SECONDS_HOURS_FACTOR / 1'000.0));
  }

  /**
   * @brief Returns coulombs.
   */
  [[nodiscard]] constexpr double as_coulombs() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns ampere-hours.
   */
  [[nodiscard]] constexpr double as_ampere_hours() const noexcept
  {
    return this->magnitude() / SECONDS_HOURS_FACTOR;
  }

  /**
   * @brief Returns milliampere-hours.
   */
  [[nodiscard]] constexpr double as_milliampere_hours() const noexcept
  {
    return this->magnitude() / (SECONDS_HOURS_FACTOR / 1'000.0);
  }
};

inline namespace literals
{
  /**
   * @brief Creates a charge from an ampere-hour literal.
   * @ingroup core_measurements
   * @relates Charge
   */
  [[nodiscard]] constexpr Charge operator""_ah(long double const value) noexcept
  {
    return Charge::from_ampere_hours(static_cast<double>(value));
  }

  /**
   * @brief Creates a charge from an ampere-hour literal.
   * @ingroup core_measurements
   * @relates Charge
   */
  [[nodiscard]] constexpr Charge operator""_ah(unsigned long long const value) noexcept
  {
    return Charge::from_ampere_hours(static_cast<double>(value));
  }

  /**
   * @brief Creates a charge from a milliampere-hour literal.
   * @ingroup core_measurements
   * @relates Charge
   */
  [[nodiscard]] constexpr Charge operator""_mah(long double const value) noexcept
  {
    return Charge::from_milliampere_hours(static_cast<double>(value));
  }

  /**
   * @brief Creates a charge from a milliampere-hour literal.
   * @ingroup core_measurements
   * @relates Charge
   */
  [[nodiscard]] constexpr Charge operator""_mah(unsigned long long const value) noexcept
  {
    return Charge::from_milliampere_hours(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Charge)
#endif
