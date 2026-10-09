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
 * @brief Force value stored in `N`.
 * @details Force is a small value type with newton, kilonewton and kilogram-force conversions.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto thrust = 1.2_kn - 20_kgf; // 1003.87 N
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Mass
 * @see Torque
 * @see Pressure
 */
class RADAR_COMMON_API Force : public QuantityBase<Force>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double newtons READ as_newtons CONSTANT FINAL)
  Q_PROPERTY(double kilonewtons READ as_kilonewtons CONSTANT FINAL)
  Q_PROPERTY(double kilogramsForce READ as_kilograms_force CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "N";

  /**
   * @brief Creates a zero force.
   */
  constexpr Force() noexcept = default;

  /**
   * @brief Creates a force from newtons.
   */
  explicit constexpr Force(double const newtons) noexcept
    : QuantityBase(newtons)
  {}

  /**
   * @brief Creates a force from newtons.
   */
  [[nodiscard]] static constexpr Force from_newtons(double const newtons) noexcept
  {
    return Force(newtons);
  }

  /**
   * @brief Creates a force from kilonewtons.
   */
  [[nodiscard]] static constexpr Force from_kilonewtons(double const kilonewtons) noexcept
  {
    return Force(kilonewtons * 1'000.0);
  }

  /**
   * @brief Creates a force from kilograms-force.
   */
  [[nodiscard]] static constexpr Force from_kilograms_force(double const kilograms_force) noexcept
  {
    return Force(kilograms_force * STANDARD_GRAVITY);
  }

  /**
   * @brief Returns newtons.
   */
  [[nodiscard]] constexpr double as_newtons() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns kilonewtons.
   */
  [[nodiscard]] constexpr double as_kilonewtons() const noexcept
  {
    return this->magnitude() / 1'000.0;
  }

  /**
   * @brief Returns kilograms-force.
   */
  [[nodiscard]] constexpr double as_kilograms_force() const noexcept
  {
    return this->magnitude() / STANDARD_GRAVITY;
  }
};

inline namespace literals
{
  /**
   * @brief Creates a force from a newton literal.
   * @ingroup core_measurements
   * @relates Force
   */
  [[nodiscard]] constexpr Force operator""_n(long double const value) noexcept
  {
    return Force::from_newtons(static_cast<double>(value));
  }

  /**
   * @brief Creates a force from a newton literal.
   * @ingroup core_measurements
   * @relates Force
   */
  [[nodiscard]] constexpr Force operator""_n(unsigned long long const value) noexcept
  {
    return Force::from_newtons(static_cast<double>(value));
  }

  /**
   * @brief Creates a force from a kilonewton literal.
   * @ingroup core_measurements
   * @relates Force
   */
  [[nodiscard]] constexpr Force operator""_kn(long double const value) noexcept
  {
    return Force::from_kilonewtons(static_cast<double>(value));
  }

  /**
   * @brief Creates a force from a kilonewton literal.
   * @ingroup core_measurements
   * @relates Force
   */
  [[nodiscard]] constexpr Force operator""_kn(unsigned long long const value) noexcept
  {
    return Force::from_kilonewtons(static_cast<double>(value));
  }

  /**
   * @brief Creates a force from a kilogram-force literal.
   * @ingroup core_measurements
   * @relates Force
   */
  [[nodiscard]] constexpr Force operator""_kgf(long double const value) noexcept
  {
    return Force::from_kilograms_force(static_cast<double>(value));
  }

  /**
   * @brief Creates a force from a kilogram-force literal.
   * @ingroup core_measurements
   * @relates Force
   */
  [[nodiscard]] constexpr Force operator""_kgf(unsigned long long const value) noexcept
  {
    return Force::from_kilograms_force(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::Force)
#endif
