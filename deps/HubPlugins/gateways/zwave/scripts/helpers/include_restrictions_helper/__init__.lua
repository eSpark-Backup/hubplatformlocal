local self = ...

self:_register("include_restrictions_helper", "HUB:zwave/scripts/helpers")

self._core = assert(require("core"), "failed to load core")
self._storage = assert(require("storage"), "failed to load storage")
