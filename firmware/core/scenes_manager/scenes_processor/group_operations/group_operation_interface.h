#pragma once

#include "json_utils/json_fwd.h"
#include "jsonrpc/message.h"
#include "item_group_operation.h"
#include "device_group_operation.h"
#include "service_interfaces/service.h"
#include "common_utils/cpp14.h"

namespace HubPlatform {
namespace ScenesManager {

class GroupOperationInterface {
public:
    explicit GroupOperationInterface( Services::Service& hostService );
    DeviceGroupOperation& deviceGroup();
    ItemGroupOperation& itemGroup();

private:
    DeviceGroupOperation m_deviceGroupOperations;
    ItemGroupOperation m_itemGroupOperations;
};

}  // namespace ScenesManager
}  // namespace HubPlatform
