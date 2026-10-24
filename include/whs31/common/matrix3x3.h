#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <bit>
#include <functional>
#include <iosfwd>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <fmt/format.h>

#include <whs31/common/config.h>
#include <whs31/common/detail/export.h>
#include <whs31/common/detail/format.h>
#include <whs31/common/detail/namespace.h>
#include <whs31/common/error.h>
#include <whs31/common/math.h>
#include <whs31/common/vector3.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <qgenericmatrix.h>
#  include <qmetaobject.h>
#  include <qmetatype.h>
#  include <qstring.h>
#endif

WHS31_COMMON_NAMESPACE_BEGIN

/**
 * @brief 3×3 matrix of doubles, stored as three row vectors.
 * @details Row-major: `(m * v).x() == m.row(0).dot(v)`. The default value is the zero matrix.
 *
 * Typical uses are rotation matrices, inertia tensors and small linear systems. It is not a
 * general linear-algebra library; use Eigen when more is needed.
 *
 * @par Layout
 * The storage is exactly three contiguous Vector3 rows, i.e. nine contiguous doubles in row-major
 * order, with no padding. The class is standard layout and trivially copyable, so it can be
 * reflected as `Vector3[3]` by offset or bit-cast to `std::array<Vector3, 3>`. `Q_GADGET` adds no
 * data members and does not affect the layout. The layout is part of the API and is checked by
 * `static_assert`s in this header.
 *
 * @code {.cpp}
 * using namespace whs31;
 *
 * // Euler's rotation equation: I ω̇ = τ − ω × (I ω)
 * auto const inertia_body = Matrix3x3::diagonal({0.5, 3.2, 3.0});
 * auto const omega_body = Vector3(0.1, -0.2, 0.3);
 * auto const torque_body = Vector3(0.0, 0.0, 1.5);
 * auto const omega_dot_body =
 *   inertia_body.inverse() * (torque_body - omega_body.cross(inertia_body * omega_body));
 * @endcode
 *
 * @note Qt properties, conversions, and metatype registration are available only when wcommon is
 * built with `with_qt=True`.
 * @ingroup core_math
 * @see Vector3
 * @see Quaternion
 */
class WHS31_COMMON_API Matrix3x3
{
#ifdef WHS31_COMMON_WITH_QT
  Q_GADGET

  Q_PROPERTY(double determinant READ determinant CONSTANT FINAL)
  Q_PROPERTY(double trace READ trace CONSTANT FINAL)
  Q_PROPERTY(bool isFinite READ is_finite CONSTANT FINAL)
  Q_PROPERTY(QString string READ to_qstring CONSTANT FINAL)
#endif

 public:
  /**
   * @brief Default tolerance of is_invertible(), relative to the product of the row magnitudes.
   */
  static constexpr double singular_tolerance = 1e-12;

  /**
   * @brief Default tolerance of is_symmetric(), relative to the largest element.
   */
  static constexpr double symmetric_tolerance = 1e-12;

  /**
   * @brief Creates a zero matrix.
   */
  constexpr Matrix3x3() noexcept = default;

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  explicit Matrix3x3(QMatrix3x3 const& matrix) noexcept;
#endif

  /**
   * @brief Returns the row at @p index.
   * @throws std::out_of_range if @p index is not in `[0, 3)`.
   */
  [[nodiscard]] constexpr Vector3 row(std::size_t const index) const
  {
    return this->m_rows.at(index);
  }

  /**
   * @brief Returns the column at @p index.
   * @throws std::out_of_range if @p index is not in `[0, 3)`.
   */
  [[nodiscard]] constexpr Vector3 column(std::size_t const index) const
  {
    auto const& [a, b, c] = this->m_rows;
    return {component(a, index), component(b, index), component(c, index)};
  }

  /**
   * @brief Returns the element in @p row and @p column.
   * @throws std::out_of_range if either index is not in `[0, 3)`.
   */
  [[nodiscard]] constexpr double at(std::size_t const row, std::size_t const column) const
  {
    return component(this->m_rows.at(row), column);
  }

  /**
   * @brief Returns the main diagonal `(m00, m11, m22)`.
   */
  [[nodiscard]] constexpr Vector3 diagonal() const noexcept
  {
    auto const& [a, b, c] = this->m_rows;
    return {a.x(), b.y(), c.z()};
  }

  [[nodiscard]] constexpr Matrix3x3 transposed() const noexcept
  {
    auto const& [a, b, c] = this->m_rows;
    return from_columns(a, b, c);
  }

  [[nodiscard]] constexpr double determinant() const noexcept
  {
    auto const& [a, b, c] = this->m_rows;
    return a.dot(b.cross(c));
  }

  [[nodiscard]] constexpr double trace() const noexcept
  {
    auto const& [a, b, c] = this->m_rows;
    return a.x() + b.y() + c.z();
  }

  /**
   * @brief Checks whether every element is neither infinite nor NaN.
   */
  [[nodiscard]] constexpr bool is_finite() const noexcept
  {
    auto const& [a, b, c] = this->m_rows;
    return a.is_finite() and b.is_finite() and c.is_finite();
  }

  /**
   * @brief Checks whether the matrix equals its transpose within @p tolerance relative to its
   * largest element, e.g. an inertia tensor after a change of frame.
   * @details A non-finite matrix is never symmetric.
   */
  [[nodiscard]] constexpr bool is_symmetric(
    double const tolerance = symmetric_tolerance
  ) const noexcept
  {
    auto const& [a, b, c] = this->m_rows;
    auto const limit = tolerance * this->max_abs_element();
    return this->is_finite() and abs(a.y() - b.x()) <= limit and abs(a.z() - c.x()) <= limit
       and abs(b.z() - c.y()) <= limit;
  }

  /**
   * @brief Checks whether inverse() succeeds.
   * @details The matrix counts as singular when it is not finite or when its determinant is not
   * greater than @p tolerance times the product of the largest absolute elements of its rows.
   * That product bounds the determinant up to a constant factor, so the test does not depend on
   * the scale of the rows: a tiny but well-conditioned matrix is invertible.
   */
  [[nodiscard]] constexpr bool is_invertible(
    double const tolerance = singular_tolerance
  ) const noexcept
  {
    auto const& [a, b, c] = this->m_rows;
    auto const det = this->determinant();
    return this->is_finite() and common::is_finite(det)
       and abs(det)
             > tolerance * max_abs_component(a) * max_abs_component(b) * max_abs_component(c);
  }

  /**
   * @brief Returns the inverse matrix, or `std::nullopt` unless is_invertible().
   * @see inverse
   */
  [[nodiscard]] constexpr std::optional<Matrix3x3> try_inverse() const noexcept
  {
    if(not this->is_invertible())
      return std::nullopt;
    auto result = this->adjugate();
    result /= this->determinant();
    return result;
  }

  /**
   * @brief Returns the inverse matrix.
   * @throws NumericError unless is_invertible().
   * @see try_inverse
   */
  [[nodiscard]] constexpr Matrix3x3 inverse() const
  {
    if(auto const result = this->try_inverse())
      return *result;
    throw NumericError("cannot invert a singular matrix, determinant {}", this->determinant());
  }

#ifdef WHS31_COMMON_WITH_QT
  /**
   * @brief Converts to a Qt matrix.
   * @details The elements are narrowed to `float`.
   * @note Available only when wcommon is built with `with_qt=True`.
   */
  [[nodiscard]] QMatrix3x3 to_qmatrix3x3() const noexcept;
#endif

  /**
   * @brief Returns the rows as `[(a, b, c), (d, e, f), (g, h, i)]`.
   */
  [[nodiscard]] std::string to_string() const;

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] QString to_qstring() const;
#endif

  [[nodiscard]] static constexpr Matrix3x3 zero() noexcept { return {}; }

  [[nodiscard]] static constexpr Matrix3x3 identity() noexcept
  {
    return diagonal(Vector3::splat(1.0));
  }

  /**
   * @brief Returns the diagonal matrix with @p diagonal on its main diagonal.
   */
  [[nodiscard]] static constexpr Matrix3x3 diagonal(Vector3 const diagonal) noexcept
  {
    return from_rows({diagonal.x(), 0.0, 0.0}, {0.0, diagonal.y(), 0.0}, {0.0, 0.0, diagonal.z()});
  }

  [[nodiscard]] static constexpr Matrix3x3 from_rows(
    Vector3 const a,
    Vector3 const b,
    Vector3 const c
  ) noexcept
  {
    return {a, b, c};
  }

  [[nodiscard]] static constexpr Matrix3x3 from_columns(
    Vector3 const a,
    Vector3 const b,
    Vector3 const c
  ) noexcept
  {
    return from_rows({a.x(), b.x(), c.x()}, {a.y(), b.y(), c.y()}, {a.z(), b.z(), c.z()});
  }

  /**
   * @brief Returns the skew-symmetric cross-product matrix of @p v: `m * u == v.cross(u)`.
   */
  [[nodiscard]] static constexpr Matrix3x3 cross_product(Vector3 const v) noexcept
  {
    return from_rows({0.0, -v.z(), v.y()}, {v.z(), 0.0, -v.x()}, {-v.y(), v.x(), 0.0});
  }

#ifdef WHS31_COMMON_WITH_QT
  /// @note Available only when wcommon is built with `with_qt=True`.
  [[nodiscard]] static Matrix3x3 from_qmatrix3x3(QMatrix3x3 const& matrix) noexcept;
#endif

  [[nodiscard]] constexpr bool operator==(Matrix3x3 const&) const noexcept = default;

  [[nodiscard]] constexpr Matrix3x3 operator+() const noexcept { return *this; }

  [[nodiscard]] constexpr Matrix3x3 operator-() const noexcept
  {
    auto const& [a, b, c] = this->m_rows;
    return from_rows(-a, -b, -c);
  }

  constexpr Matrix3x3& operator+=(Matrix3x3 const& rhs) noexcept
  {
    auto& [a, b, c] = this->m_rows;
    auto const& [rhs_a, rhs_b, rhs_c] = rhs.m_rows;
    a += rhs_a;
    b += rhs_b;
    c += rhs_c;
    return *this;
  }

  constexpr Matrix3x3& operator-=(Matrix3x3 const& rhs) noexcept
  {
    auto& [a, b, c] = this->m_rows;
    auto const& [rhs_a, rhs_b, rhs_c] = rhs.m_rows;
    a -= rhs_a;
    b -= rhs_b;
    c -= rhs_c;
    return *this;
  }

  constexpr Matrix3x3& operator*=(double const rhs) noexcept
  {
    for(auto& row : this->m_rows)
      row *= rhs;
    return *this;
  }

  constexpr Matrix3x3& operator/=(double const rhs) noexcept
  {
    for(auto& row : this->m_rows)
      row /= rhs;
    return *this;
  }

 private:
  constexpr Matrix3x3(Vector3 const a, Vector3 const b, Vector3 const c) noexcept
    : m_rows {a, b, c}
  {}

  [[nodiscard]] static constexpr double component(Vector3 const v, std::size_t const index)
  {
    switch(index) {
      case 0: return v.x();
      case 1: return v.y();
      case 2: return v.z();
      default: throw std::out_of_range("Matrix3x3 index out of range");
    }
  }

  [[nodiscard]] static constexpr double max_abs_component(Vector3 const v) noexcept
  {
    auto const x = abs(v.x());
    auto const y = abs(v.y());
    auto const z = abs(v.z());
    return std::max({x, y, z});
  }

  [[nodiscard]] constexpr double max_abs_element() const noexcept
  {
    auto result = 0.0;
    for(auto const& row : this->m_rows)
      result = std::max(result, max_abs_component(row));
    return result;
  }

  // the columns of the adjugate are the cross products of the other two rows
  [[nodiscard]] constexpr Matrix3x3 adjugate() const noexcept
  {
    auto const& [a, b, c] = this->m_rows;
    return from_columns(b.cross(c), c.cross(a), a.cross(b));
  }

  std::array<Vector3, 3> m_rows {};
};

[[nodiscard]] constexpr Matrix3x3 operator+(Matrix3x3 lhs, Matrix3x3 const& rhs) noexcept
{
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr Matrix3x3 operator-(Matrix3x3 lhs, Matrix3x3 const& rhs) noexcept
{
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr Matrix3x3 operator*(Matrix3x3 lhs, double const rhs) noexcept
{
  lhs *= rhs;
  return lhs;
}

[[nodiscard]] constexpr Matrix3x3 operator*(double const lhs, Matrix3x3 rhs) noexcept
{
  rhs *= lhs;
  return rhs;
}

[[nodiscard]] constexpr Matrix3x3 operator/(Matrix3x3 lhs, double const rhs) noexcept
{
  lhs /= rhs;
  return lhs;
}

/**
 * @brief Returns the matrix-vector product `m v`.
 * @relates Matrix3x3
 */
[[nodiscard]] constexpr Vector3 operator*(Matrix3x3 const& m, Vector3 const v) noexcept
{
  return {m.row(0).dot(v), m.row(1).dot(v), m.row(2).dot(v)};
}

/**
 * @brief Returns the matrix product `lhs rhs`: `(lhs * rhs) * v == lhs * (rhs * v)`.
 * @relates Matrix3x3
 */
[[nodiscard]] constexpr Matrix3x3 operator*(Matrix3x3 const& lhs, Matrix3x3 const& rhs) noexcept
{
  auto const columns = rhs.transposed();
  return Matrix3x3::from_rows(columns * lhs.row(0), columns * lhs.row(1), columns * lhs.row(2));
}

WHS31_COMMON_API std::ostream& operator<<(std::ostream& os, Matrix3x3 const& matrix);

static_assert(std::is_standard_layout_v<Matrix3x3>);
static_assert(std::is_trivially_copyable_v<Matrix3x3>);
static_assert(sizeof(Matrix3x3) == 9 * sizeof(double));
static_assert(
  std::bit_cast<std::array<double, 9>>(
    Matrix3x3::from_rows({1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0})
  ) == std::array {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0},
  "Matrix3x3 must store three contiguous rows"
);

WHS31_COMMON_NAMESPACE_END

template <>
struct std::hash<whs31::common::Matrix3x3>
{
  [[nodiscard]] std::size_t operator()(whs31::common::Matrix3x3 const& matrix) const noexcept
  {
    auto hash = std::hash<whs31::common::Vector3> {}(matrix.row(0));
    hash ^= std::hash<whs31::common::Vector3> {}(matrix.row(1)) + 0x9e3779b97f4a7c15ull
          + (hash << 6U) + (hash >> 2U);
    hash ^= std::hash<whs31::common::Vector3> {}(matrix.row(2)) + 0x9e3779b97f4a7c15ull
          + (hash << 6U) + (hash >> 2U);
    return hash;
  }
};

#ifndef __DOXYGEN__
/**
 * @brief Formats as `[(a, b, c), (d, e, f), (g, h, i)]` row by row; the format spec, e.g.
 * `{:.3f}`, applies to every element.
 */
template <>
struct fmt::formatter<whs31::common::Matrix3x3> : fmt::formatter<double>
{
  format_context::iterator format(whs31::common::Matrix3x3 const& matrix, format_context& ctx) const
  {
    auto out = ctx.out();
    *out++ = '[';
    for(auto index = std::size_t {0}; index < 3; ++index) {
      if(index != 0) {
        *out++ = ',';
        *out++ = ' ';
      }
      ctx.advance_to(out);
      auto const row = matrix.row(index);
      out = whs31::common::_format_impl::format_components<
        3>(*this, {row.x(), row.y(), row.z()}, ctx);
    }
    *out++ = ']';
    return out;
  }
};
#endif

#ifdef WHS31_COMMON_WITH_QT
Q_DECLARE_METATYPE(whs31::common::Matrix3x3)
#endif
