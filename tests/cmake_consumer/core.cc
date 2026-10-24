#include <whs31/common/angle.h>

int main()
{
  auto const angle = whs31::common::Angle::from_degrees(180.0);
  return angle.to_string().empty() ? 1 : 0;
}
