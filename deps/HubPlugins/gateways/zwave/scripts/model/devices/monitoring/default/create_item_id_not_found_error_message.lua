local self, item_name, node_id, channel_id, class_id, item_tag = ...

return string.format(
        "Item id not found by item descriptor: item name: %s, node id: %g, channel id: %g, class id: 0x%X, tag: %s",
        item_name, node_id, channel_id, class_id, tostring(item_tag))
