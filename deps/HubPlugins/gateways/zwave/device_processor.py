#!/usr/bin/env python3

from dev_utils.base_device_processor import BaseDeviceProcessor
from zwave.device_info_builder import DeviceInfoBuilder
from zwave.device_configuration_builder import DeviceConfigurationBuilder
from zwave.device_settings_builder import DeviceSettingsBuilder
from zwave.devices_list_builder import DevicesListBuilder

import itertools
from typing import Iterator, List


class DeviceProcessor(BaseDeviceProcessor):

    def __init__(self, _lookup_table, _plugin_name):
        super().__init__(_plugin_name, [
            DeviceInfoBuilder(_lookup_table),
            DeviceConfigurationBuilder(_lookup_table),
            DeviceSettingsBuilder(_lookup_table),
            DevicesListBuilder(_lookup_table)
        ])

    def process_device(self, _device):
        for entry in self.unroll_entry(_device):
            super().process_device(entry)

    def unroll_entry(self, _device: dict) -> Iterator[dict]:
        manufacturer_ids = self.expand_ranges(_device.get("manufacturerIds") or [_device["manufacturerId"]])
        product_types = self.expand_ranges(_device["variables"].get("productTypes") or [_device["variables"]["productType"]])
        device_ids = self.expand_ranges(_device.get("deviceIds") or [_device["deviceId"]])

        for manufacturer_id, product_type, device_id in itertools.product(manufacturer_ids, product_types, device_ids):
            _device["manufacturerId"] = manufacturer_id
            _device["variables"]["productType"] = product_type
            _device["deviceId"] = device_id
            yield _device

    def expand_ranges(self, _values: List[str]) -> Iterator[str]:
        for value in _values:
            bounds = value.split("..")

            if len(bounds) == 1:
                yield from bounds
            elif len(bounds) == 2:
                lower, upper = [int(bound) for bound in bounds]
                # add 1 to the upper bound to include it in the range
                yield from [str(val) for val in range(lower, upper + 1)]
            else:
                raise RuntimeError("Can't expand '" + value + "'")
