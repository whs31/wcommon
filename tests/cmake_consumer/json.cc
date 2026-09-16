#include <radar/mms/common/json/angle.h>
#include <radar/mms/common/json/chrono.h>

#ifdef RCOMMON_WITH_QT
#  include <radar/mms/common/json/qgeocoordinate.h>
#endif

#include <nlohmann/json.hpp>

int main()
{
  auto const angle = radar::mms::common::Angle::from_degrees(180.0F);
  auto const json = nlohmann::json(angle);
  auto const duration = std::chrono::seconds(42);
  auto success = json.get<radar::mms::common::Angle>() == angle
              && nlohmann::json(duration).get<std::chrono::seconds>() == duration;
#ifdef RCOMMON_WITH_QT
  auto const coordinate = QGeoCoordinate(55.7558, 37.6173, 156.5);
  success = success && nlohmann::json(coordinate).get<QGeoCoordinate>() == coordinate;
#endif
  return success ? 0 : 1;
}
