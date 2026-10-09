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
 * @brief Magnetic flux density value stored in `T`.
 * @details MagneticFluxDensity is a scalar magnetic field strength stored in teslas, with
 * gauss, microtesla and nanotesla conversions.
 * @note Qt properties, QString conversion, and metatype registration are
 * available only when rcommon is built with `with_qt=True`.
 *
 * @code {.cpp}
 * using namespace radar::mms::literals;
 *
 * auto earth_field = 0.5_gauss;
 * auto micro = earth_field.as_microteslas(); // 50
 * @endcode
 *
 * @ingroup core_measurements
 * @see QuantityBase
 * @see Current
 */
class RADAR_COMMON_API MagneticFluxDensity : public QuantityBase<MagneticFluxDensity>
{
#ifdef RCOMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double teslas READ as_teslas CONSTANT FINAL)
  Q_PROPERTY(double gauss READ as_gauss CONSTANT FINAL)
  Q_PROPERTY(double microteslas READ as_microteslas CONSTANT FINAL)
  Q_PROPERTY(double nanoteslas READ as_nanoteslas CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Symbol of the SI unit the magnitude is stored in.
   */
  static constexpr std::string_view si_symbol = "T";

  /**
   * @brief Creates a zero magnetic flux density.
   */
  constexpr MagneticFluxDensity() noexcept = default;

  /**
   * @brief Creates a magnetic flux density from teslas.
   */
  explicit constexpr MagneticFluxDensity(double const teslas) noexcept
    : QuantityBase(teslas)
  {}

  /**
   * @brief Creates a magnetic flux density from teslas.
   */
  [[nodiscard]] static constexpr MagneticFluxDensity from_teslas(double const teslas) noexcept
  {
    return MagneticFluxDensity(teslas);
  }

  /**
   * @brief Creates a magnetic flux density from gauss.
   */
  [[nodiscard]] static constexpr MagneticFluxDensity from_gauss(double const gauss) noexcept
  {
    return MagneticFluxDensity(gauss / TESLA_GAUSS_FACTOR);
  }

  /**
   * @brief Creates a magnetic flux density from microteslas.
   */
  [[nodiscard]] static constexpr MagneticFluxDensity from_microteslas(
    double const microteslas
  ) noexcept
  {
    return MagneticFluxDensity(microteslas / 1'000'000.0);
  }

  /**
   * @brief Creates a magnetic flux density from nanoteslas.
   */
  [[nodiscard]] static constexpr MagneticFluxDensity from_nanoteslas(
    double const nanoteslas
  ) noexcept
  {
    return MagneticFluxDensity(nanoteslas / 1'000'000'000.0);
  }

  /**
   * @brief Returns teslas.
   */
  [[nodiscard]] constexpr double as_teslas() const noexcept { return this->magnitude(); }

  /**
   * @brief Returns gauss.
   */
  [[nodiscard]] constexpr double as_gauss() const noexcept
  {
    return this->magnitude() * TESLA_GAUSS_FACTOR;
  }

  /**
   * @brief Returns microteslas.
   */
  [[nodiscard]] constexpr double as_microteslas() const noexcept
  {
    return this->magnitude() * 1'000'000.0;
  }

  /**
   * @brief Returns nanoteslas.
   */
  [[nodiscard]] constexpr double as_nanoteslas() const noexcept
  {
    return this->magnitude() * 1'000'000'000.0;
  }
};

inline namespace literals
{
  /**
   * @brief Creates a magnetic flux density from a gauss literal.
   * @ingroup core_measurements
   * @relates MagneticFluxDensity
   */
  [[nodiscard]] constexpr MagneticFluxDensity operator""_gauss(long double const value) noexcept
  {
    return MagneticFluxDensity::from_gauss(static_cast<double>(value));
  }

  /**
   * @brief Creates a magnetic flux density from a gauss literal.
   * @ingroup core_measurements
   * @relates MagneticFluxDensity
   */
  [[nodiscard]] constexpr MagneticFluxDensity operator""_gauss(
    unsigned long long const value
  ) noexcept
  {
    return MagneticFluxDensity::from_gauss(static_cast<double>(value));
  }

  /**
   * @brief Creates a magnetic flux density from a microtesla literal.
   * @ingroup core_measurements
   * @relates MagneticFluxDensity
   */
  [[nodiscard]] constexpr MagneticFluxDensity operator""_ut(long double const value) noexcept
  {
    return MagneticFluxDensity::from_microteslas(static_cast<double>(value));
  }

  /**
   * @brief Creates a magnetic flux density from a microtesla literal.
   * @ingroup core_measurements
   * @relates MagneticFluxDensity
   */
  [[nodiscard]] constexpr MagneticFluxDensity operator""_ut(unsigned long long const value) noexcept
  {
    return MagneticFluxDensity::from_microteslas(static_cast<double>(value));
  }

  /**
   * @brief Creates a magnetic flux density from a nanotesla literal.
   * @ingroup core_measurements
   * @relates MagneticFluxDensity
   */
  [[nodiscard]] constexpr MagneticFluxDensity operator""_nt(long double const value) noexcept
  {
    return MagneticFluxDensity::from_nanoteslas(static_cast<double>(value));
  }

  /**
   * @brief Creates a magnetic flux density from a nanotesla literal.
   * @ingroup core_measurements
   * @relates MagneticFluxDensity
   */
  [[nodiscard]] constexpr MagneticFluxDensity operator""_nt(unsigned long long const value) noexcept
  {
    return MagneticFluxDensity::from_nanoteslas(static_cast<double>(value));
  }
}  // namespace literals

RADAR_COMMON_NAMESPACE_END

#ifdef RCOMMON_WITH_QT
Q_DECLARE_METATYPE(radar::mms::common::MagneticFluxDensity)
#endif
