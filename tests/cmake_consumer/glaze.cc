#include <whs31/common/glaze/angle.h>
#include <whs31/common/glaze/mass.h>
#include <whs31/common/glaze/uuid.h>

#ifdef WHS31_COMMON_WITH_QT
#  include <whs31/common/glaze/qgeocoordinate.h>
#endif

#include <glaze/json.hpp>

namespace
{
  template <typename T>
  bool round_trips(T const& value)
  {
    auto const json = glz::write_json(value);
    auto back = T {};
    return json.has_value() and not glz::read_json(back, *json) and back == value;
  }
}  // namespace

int main()
{
  using namespace whs31::common;

  auto success = round_trips(Angle::from_degrees(180.0)) and round_trips(Mass::from_grams(250.0))
             and round_trips(Uuid("67e55044-10b1-426f-9247-bb680e5fe0c8"));
#ifdef WHS31_COMMON_WITH_QT
  success = success and round_trips(QGeoCoordinate(55.7558, 37.6173, 156.5));
#endif
  return success ? 0 : 1;
}
