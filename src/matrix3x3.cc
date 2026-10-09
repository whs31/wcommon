#include <radar/mms/common/matrix3x3.h>

#include <array>
#include <ostream>

#include <fmt/format.h>

RADAR_COMMON_NAMESPACE_BEGIN

std::string Matrix3x3::to_string() const { return fmt::format("{}", *this); }

#ifdef RCOMMON_WITH_QT
Matrix3x3::Matrix3x3(QMatrix3x3 const& matrix) noexcept
  : m_rows {
      Vector3(matrix(0, 0), matrix(0, 1), matrix(0, 2)),
      Vector3(matrix(1, 0), matrix(1, 1), matrix(1, 2)),
      Vector3(matrix(2, 0), matrix(2, 1), matrix(2, 2)),
    }
{}

QMatrix3x3 Matrix3x3::to_qmatrix3x3() const noexcept
{
  auto const& [a, b, c] = this->m_rows;
  // row-major, as the QGenericMatrix constructor expects
  auto const values = std::array {
    static_cast<float>(a.x()),
    static_cast<float>(a.y()),
    static_cast<float>(a.z()),
    static_cast<float>(b.x()),
    static_cast<float>(b.y()),
    static_cast<float>(b.z()),
    static_cast<float>(c.x()),
    static_cast<float>(c.y()),
    static_cast<float>(c.z()),
  };
  return QMatrix3x3(values.data());
}

QString Matrix3x3::to_qstring() const { return QString::fromStdString(this->to_string()); }

Matrix3x3 Matrix3x3::from_qmatrix3x3(QMatrix3x3 const& matrix) noexcept
{
  return Matrix3x3(matrix);
}
#endif

std::ostream& operator<<(std::ostream& os, Matrix3x3 const& matrix)
{
  return os << matrix.to_string();
}

RADAR_COMMON_NAMESPACE_END
