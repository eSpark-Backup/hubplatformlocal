#!/usr/bin/env python3

from dev_utils.base_device_info_builder import BaseDeviceInfoBuilder
from os import path


script_folder = path.dirname(path.realpath(__file__))


class DeviceInfoBuilder(BaseDeviceInfoBuilder):

    def __init__(self, _lookup_table):
        super().__init__(
            _lookup_table,
            path.join(script_folder, "scripts", "devices", "generated")
        )

    def generate_device_file_path(self, _device):
        device_id = _device["deviceId"]
        model = _device["model"]

        return path.join(self.devices_dir, "{0}_{1}.lua".format(model, device_id))
    
    def generate_device_info(self, _device):
        pass

    def get_file_skeleton_template(self):
        return 'local {0} = require "HUB:bluetooth/scripts/devices/handler"\n\n{1}\n\nreturn {0}'
