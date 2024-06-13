#include "Core/Core.hpp"
#include "Core/VersionInfo_Core.h"

namespace core {
  std::string version() {
    return std::string{ PRODUCT_VERSION_FULL_STR };
  }
}