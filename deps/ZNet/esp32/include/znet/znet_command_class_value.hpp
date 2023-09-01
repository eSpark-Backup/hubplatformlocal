#pragma once

#include <znet/znet.h>
#include <znet/variant.hpp>
#include <znet/enums/color_component.hpp>

#include <string>
#include <vector>

namespace znet {

using ColorComponents = std::vector<znet_cl_component_t>;

using CommandClassValue = utils::variant<uint8_t, std::string, ColorComponents>;

}  // namespace znet
