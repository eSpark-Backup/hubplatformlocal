local self = ...

self:_register("storage_helper", "HUB:zwave/scripts/utils")

self._storage = assert(require("storage"), "failed to load storage")
