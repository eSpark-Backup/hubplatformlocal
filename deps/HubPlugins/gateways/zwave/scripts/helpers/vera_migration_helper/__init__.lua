local self = ...

self:_register("vera_migration_helper", "HUB:zwave/scripts/helpers")

self._core = assert(require("core"), "failed to load core")
self._storage = hubplugins.object("storage_helper", "HUB:zwave/scripts/utils")
