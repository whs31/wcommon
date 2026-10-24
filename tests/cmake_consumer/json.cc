#include <whs31/common/json/angle.h>
#include <whs31/common/json/chrono.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <whs31/common/json/qgeocoordinate.h>
#endif

#include <nlohmann/json.hpp>

int main()
{
  auto const angle = whs31::common::Angle::from_degrees(180.0);
  auto const json = nlohmann::json(angle);
  auto const duration = std::chrono::seconds(42);
  auto success = json.get<whs31::common::Angle>() == angle
              && nlohmann::json(duration).get<std::chrono::seconds>() == duration;
#ifdef WHS31_COMMON_WITH_QT
  auto const coordinate = QGeoCoordinate(55.7558, 37.6173, 156.5);
  success = success && nlohmann::json(coordinate).get<QGeoCoordinate>() == coordinate;
#endif
  return success ? 0 : 1;
}
