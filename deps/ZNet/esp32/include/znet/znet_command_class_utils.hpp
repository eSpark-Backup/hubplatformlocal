#pragma once

#include <znet/znet.h>
#include <memory>

namespace znet {

class ZClass;
class ZChannel;
using ClassPtr = std::shared_ptr<ZClass>;

znet_command_class_t getCommandClassId( const ZClass& zclass );

ClassPtr createCommandClass( znet_command_class_t id, ZChannel& parent, znet_command_class_version_t version = 1 );

bool canCreateCommandClass( znet_command_class_t id );

}  // namespace znet
