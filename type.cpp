#include "type.h"
#include <stdexcept>

using std::string;

std::shared_ptr<Type> Type::createType(const string typeStr) {
  if (typeStr == "int") {
    return std::make_shared<IntType>();
  } else if (typeStr == "bool") {
    return std::make_shared<BoolType>();
  } else {
    throw std::runtime_error("Invalid type: " + typeStr);
  }
}