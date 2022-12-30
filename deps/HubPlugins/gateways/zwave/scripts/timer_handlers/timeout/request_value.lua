local params = ...

require("HUB:zwave/scripts/helpers/scripts_preload")

hubplugins.weak_loadfile("HUB:zwave/scripts/request_value")(
    {
        node_id = params.node_id,
        class_id = params.class_id,
        channel_id = params.channel_id
    }
)
