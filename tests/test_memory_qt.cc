#include <radar/mms/common/memory/observer_ptr.h>

#ifndef RCOMMON_WITH_QT
#  error "Qt configuration must enable RCOMMON_WITH_QT"
#endif

#include <qbytearrayview.h>
#include <qmetaobject.h>
#include <qmetatype.h>
#include <qobject.h>
#include <qvariant.h>

#include <catch2/catch_test_macros.hpp>

using namespace radar::mms;

// moc generates code that refers to the class from outside an anonymous namespace
class ObserverPtrQtObject final : public QObject  // NOLINT(misc-use-internal-linkage)
{
  Q_OBJECT
  Q_PROPERTY(
    radar::mms::observer_ptr<int> watched READ watched WRITE setWatched NOTIFY watchedChanged
  )

 public:
  [[nodiscard]] observer_ptr<int> watched() const noexcept { return this->m_watched; }

  // Qt property and signal naming; moc defines the signal with its own parameter names
  // NOLINTBEGIN(readability-identifier-naming,readability-inconsistent-declaration-parameter-name)
  void setWatched(observer_ptr<int> const watched)
  {
    if(this->m_watched == watched)
      return;

    this->m_watched = watched;
    emit this->watchedChanged(this->m_watched);
  }

 signals:
  void watchedChanged(observer_ptr<int> watched);
  // NOLINTEND(readability-identifier-naming,readability-inconsistent-declaration-parameter-name)

 private:
  observer_ptr<int> m_watched;
};

TEST_CASE("observer_ptr is usable as a Qt metatype, property and signal argument", "[memory][qt]")
{
  STATIC_CHECK(QMetaTypeId2<observer_ptr<int>>::Defined);

  auto value = 17;
  auto const observer = make_observer(&value);
  auto object = ObserverPtrQtObject();
  auto signal_count = 0;
  auto emitted_observer = observer_ptr<int> {};

  auto const type = QMetaType::fromType<observer_ptr<int>>();
  REQUIRE(type.isValid());
  REQUIRE(QByteArrayView(type.name()).contains("observer_ptr"));

  auto const variant = QVariant::fromValue(observer);
  REQUIRE(variant.isValid());
  REQUIRE(variant.value<observer_ptr<int>>() == observer);

  QObject::connect(
    &object,
    &ObserverPtrQtObject::watchedChanged,
    &object,
    [&](observer_ptr<int> const watched) {
      ++signal_count;
      emitted_observer = watched;
    }
  );

  REQUIRE(object.setProperty("watched", variant));
  REQUIRE(object.watched() == observer);
  REQUIRE(object.property("watched").value<observer_ptr<int>>() == observer);
  REQUIRE(signal_count == 1);
  REQUIRE(emitted_observer == observer);

  auto const property = object.metaObject()->property(
    object.metaObject()->indexOfProperty("watched")
  );
  REQUIRE(property.isValid());
  REQUIRE(QByteArrayView(property.typeName()).contains("observer_ptr"));
}

#include "test_memory_qt.moc"
