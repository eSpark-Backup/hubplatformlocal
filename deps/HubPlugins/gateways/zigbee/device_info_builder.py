#!/usr/bin/env python3

from dev_utils.base_device_info_builder import BaseDeviceInfoBuilder
from os import path


script_folder = path.dirname(path.realpath(__file__))


class DeviceInfoBuilder(BaseDeviceInfoBuilder):

    def __init__(self, _lookup_table):
        super().__init__(
            _lookup_table,
            path.join(script_folder, "scripts", "devices", "device_info", "generated"),
            'device_info'
        )

    def generate_device_file_path(self, _device):
        manufacturer = _device["manufacturerId"]
        device_id = _device["deviceId"]

        return path.join(self.devices_dir, "{0}_{1}.lua".format(manufacturer, device_id))

    def get_file_skeleton_template(self):
        return 'local {0} = hubplugins.weak_loadfile("HUB:zigbee/scripts/devices/device_info/default")()\n\n{1}\n\nreturn {0}'

    def generate_device_info(self, _device):
        return {
            "manufacturer": _device["manufacturerId"],
            "model": _device["model"]
        }
