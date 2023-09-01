#pragma once

#include <vector>
#include <string>

namespace znet {

enum class CentralSceneAttributes;

namespace central_scenes_attributes_converter {

std::vector<std::string> maskToStrings( uint8_t bit_mask ) noexcept;

const char* stateToString( CentralSceneAttributes state ) noexcept;

}  // namespace central_scenes_attributes_converter
}  // namespace znet
