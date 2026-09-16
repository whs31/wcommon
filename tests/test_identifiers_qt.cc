#include <radar/mms/common/uuid.h>
#include <radar/mms/common/version.h>
#include <radar/mms/common/version_range.h>

#if ! defined(RCOMMON_WITH_QT)
#  error "Qt configuration must enable RCOMMON_WITH_QT"
#endif

#include <qmetaobject.h>
#include <qmetatype.h>
#include <qstring.h>
#include <qstringview.h>
#include <quuid.h>
#include <qvariant.h>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

TEST_CASE("Uuid exposes Qt conversions metatype and properties", "[uuid][qt]")
{
  auto const qt = QUuid("{00112233-4455-6677-8899-aabbccddeeff}");
  auto const uuid = Uuid(qt);
  auto const property = Uuid::staticMetaObject.property(
    Uuid::staticMetaObject.indexOfProperty("string")
  );

  STATIC_CHECK(QMetaTypeId2<Uuid>::Defined);
  CHECK(uuid.to_quuid() == qt);
  CHECK(Uuid::from_qstring(QStringView {u"00112233-4455-6677-8899-aabbccddeeff"}) == uuid);
  CHECK(uuid.to_qstring() == QString("00112233-4455-6677-8899-aabbccddeeff"));
  CHECK(QVariant::fromValue(uuid).value<Uuid>() == uuid);
  REQUIRE(property.isValid());
  CHECK(property.readOnGadget(&uuid).toString() == uuid.to_qstring());
}

TEST_CASE(
  "Version and VersionRange expose Qt conversions metatypes and properties",
  "[version][qt]"
)
{
  auto const version = Version::from_qstring(QStringView {u"2.3.4-beta+build.7"});
  auto const range = VersionRange::from_qstring(QStringView {u"^2.3.4"});
  auto const unrestricted = VersionRange::any_with_prerelease();
  auto const major = Version::staticMetaObject.property(
    Version::staticMetaObject.indexOfProperty("major")
  );
  auto const any_with_prerelease = VersionRange::staticMetaObject.property(
    VersionRange::staticMetaObject.indexOfProperty("any_with_prerelease")
  );

  STATIC_CHECK(QMetaTypeId2<Version>::Defined);
  STATIC_CHECK(QMetaTypeId2<VersionRange>::Defined);
  CHECK(version.to_qstring() == QString("2.3.4-beta+build.7"));
  CHECK(version.prerelease_qstring() == QString("beta"));
  CHECK(version.build_metadata_qstring() == QString("build.7"));
  CHECK(range.to_qstring() == QString("^2.3.4"));
  CHECK(QVariant::fromValue(version).value<Version>() == version);
  CHECK(QVariant::fromValue(range).value<VersionRange>() == range);
  REQUIRE(major.isValid());
  REQUIRE(any_with_prerelease.isValid());
  CHECK(major.readOnGadget(&version).toULongLong() == 2);
  CHECK(any_with_prerelease.readOnGadget(&unrestricted).toBool());
  CHECK(VersionRange::staticMetaObject.indexOfMethod("contains(Version)") >= 0);
  CHECK(VersionRange::staticMetaObject.indexOfMethod("satisfies(Version)") >= 0);
}
