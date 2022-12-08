#!/usr/bin/env python3

from importlib import import_module
from dev_utils.base_device_processor import BaseDeviceProcessor
DeviceInfoBuilder = getattr(import_module('2gig.device_info_builder'), 'DeviceInfoBuilder')
SupportedDevicesBuilder = getattr(import_module('2gig.supported_devices_builder'), 'SupportedDevicesBuilder')


class DeviceProcessor(BaseDeviceProcessor):

    def __init__(self, _lookup_table, _plugin_name):
        super().__init__(_plugin_name, [
            DeviceInfoBuilder(_lookup_table),
            SupportedDevicesBuilder(_lookup_table)
        ])
