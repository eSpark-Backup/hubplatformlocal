#pragma once

#include <znet/enums/multilevel_sensor.hpp>

#include <stdint.h>

namespace znet {
namespace multilevel_sensor_converter {

const char* typeToString( MultilevelSensor::Type sensorType );
const char* scaleToString( MultilevelSensor::Type sensorType, uint8_t sensorScale );

}  // namespace multilevel_sensor_converter
}  // namespace znet
