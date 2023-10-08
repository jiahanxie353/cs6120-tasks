#include "value.h"
#include <iomanip>
#include <sstream>

BoolValue::BoolValue(Type *bt, bool d)
    : ConstValue(static_cast<BoolType *>(bt)), data(d) {}

BoolValue::BoolValue(Type *bt, std::string sd)
    : ConstValue(static_cast<BoolType *>(bt)) {
  std::istringstream(sd) >> std::boolalpha >> data;
}