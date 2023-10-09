#include "value.h"
#include <iomanip>
#include <sstream>

BoolValue::BoolValue(std::string sd) : ConstValue(new BoolType()) {
  std::istringstream(sd) >> std::boolalpha >> data;
}