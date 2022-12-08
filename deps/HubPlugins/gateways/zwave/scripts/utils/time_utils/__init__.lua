local self = ...

self:_register("time_utils", "HUB:zwave/scripts/utils")

self._core = assert(require("core"), "failed to load core")
