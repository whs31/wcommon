#include <radar/mms/common/angle.h>

int main()
{
  auto const angle = radar::mms::common::Angle::from_degrees(180.0F);
  return angle.to_string().empty() ? 1 : 0;
}
