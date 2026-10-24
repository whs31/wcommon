#include <whs31/common/matrix3x3.h>
#include <whs31/common/quaternion.h>

#include <qgenericmatrix.h>
#include <qmetatype.h>
#include <qquaternion.h>
#include <qstring.h>

#include <catch2/catch_test_macros.hpp>

using namespace whs31::common;

TEST_CASE("Linear algebra values round-trip Qt Gui types", "[linalg][qt]")
{
  auto constexpr matrix = Matrix3x3::from_rows({1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.5});
  auto const qmatrix = matrix.to_qmatrix3x3();

  REQUIRE(qmatrix(0, 1) == 2.0F);
  REQUIRE(qmatrix(1, 0) == 4.0F);
  REQUIRE(qmatrix(2, 2) == 9.5F);
  REQUIRE(Matrix3x3::from_qmatrix3x3(qmatrix) == matrix);
  REQUIRE(Matrix3x3(QMatrix3x3()) == Matrix3x3::identity());
  REQUIRE(Matrix3x3::identity().to_qstring() == QString("[(1, 0, 0), (0, 1, 0), (0, 0, 1)]"));

  auto constexpr quaternion = Quaternion(0.5, -0.5, 0.25, 1.0);
  auto const qquaternion = quaternion.to_qquaternion();

  REQUIRE(qquaternion == QQuaternion(0.5F, -0.5F, 0.25F, 1.0F));
  REQUIRE(Quaternion::from_qquaternion(qquaternion) == quaternion);
  REQUIRE(Quaternion(QQuaternion()) == Quaternion::identity());
  REQUIRE(quaternion.to_qstring() == QString("(0.5, -0.5, 0.25, 1)"));

  REQUIRE(QMetaType::fromType<Matrix3x3>().isValid());
  REQUIRE(QMetaType::fromType<Quaternion>().isValid());
  REQUIRE(Matrix3x3::staticMetaObject.indexOfProperty("determinant") >= 0);
  REQUIRE(Quaternion::staticMetaObject.indexOfProperty("w") >= 0);
  REQUIRE(Vector3::staticMetaObject.indexOfProperty("isFinite") >= 0);
}
