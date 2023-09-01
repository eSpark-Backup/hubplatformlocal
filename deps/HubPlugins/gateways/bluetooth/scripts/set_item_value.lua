local params = ...

local mapper = require("HUB:bluetooth/scripts/utils/mapper")

local itemInfo = mapper:get_item_info_by_id(params.item_id)
require(itemInfo.setter, { deviceAddress = itemInfo.deviceAddress, value = params.value })
