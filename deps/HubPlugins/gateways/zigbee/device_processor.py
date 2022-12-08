#!/usr/bin/env python3

from dev_utils.base_device_processor import BaseDeviceProcessor

from zigbee.device_info_builder import DeviceInfoBuilder


class DeviceProcessor(BaseDeviceProcessor):

    def __init__(self, _lookup_table, _plugin_name):
        super().__init__(_plugin_name, [
            DeviceInfoBuilder(_lookup_table)
        ])
