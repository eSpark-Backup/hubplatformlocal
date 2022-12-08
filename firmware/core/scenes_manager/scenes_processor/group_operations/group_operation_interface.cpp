#include "group_operation_interface.h"
#include "device_group_operation.h"

namespace HubPlatform {
namespace ScenesManager {

GroupOperationInterface::GroupOperationInterface( Services::Service& hostService )
: m_deviceGroupOperations( hostService )
, m_itemGroupOperations( hostService ) {}

DeviceGroupOperation& GroupOperationInterface::deviceGroup() {
    return m_deviceGroupOperations;
}

ItemGroupOperation& GroupOperationInterface::itemGroup() {
    return m_itemGroupOperations;
}

}  // namespace ScenesManager
}  // namespace HubPlatform
