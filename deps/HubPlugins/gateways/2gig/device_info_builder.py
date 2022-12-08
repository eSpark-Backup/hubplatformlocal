#!/usr/bin/env python3

from dev_utils.base_device_info_builder import BaseDeviceInfoBuilder

import os
from os import path


script_folder = path.dirname(path.realpath(__file__))


def get_template(file_name):
    with open(path.join(script_folder, "templates", "device_info", file_name), "r") as file:
        return file.read()


template_skeleton = get_template("skeleton.lua.template")


class DeviceInfoBuilder(BaseDeviceInfoBuilder):

    def __init__(self, _lookup_table):
        super().__init__(
            _lookup_table,
            path.join(script_folder, "scripts", "model", "devices", "generated"),
            "device_info"
        )
        self.base_devices_dir = path.join(script_folder, "scripts", "model", "devices")

    def process_device(self, _device):
        device_type = _device["variables"]["deviceType"]
        if not self.is_supported_device_type(device_type):
            print(f"Warning: unsupported 2GIG device type '{device_type}' ({_device['model']})")
            return

        file_path = self.generate_device_file_path(_device)

        entries = "\n".join(self.generate_entries(_device))
        content = self.get_file_skeleton_template().format(device_type=device_type, entries=entries)

        self.create_file(file_path, content)

    def generate_entries(self, _device):
        yield from super().generate_entries(_device)

        yield self.create_entry("device_type_id", "{manufacturer}-{model}".format(**self.generate_device_info(_device)))

        child_devices = _device["variables"].get("childDevices", {})
        for idx, device_info in enumerate(child_devices, start=1):
            for name, value in self.base_device_info_items(device_info):
                yield self.create_entry(name, value, _handler_name=f"{self.handler_name}.child_devices[{idx}]")

    def process_device_variables(self, _device_variables):
        supported_variables = [
            "settings"
        ]

        yield from self.process_variables(_device_variables, supported_variables)

    def generate_device_file_path(self, _device):
        return path.join(self.devices_dir, "{manufacturer}-{model}.lua".format(**self.generate_device_info(_device)))

    def get_file_skeleton_template(self):
        return template_skeleton

    def generate_device_info(self, _device):
        return {
            "manufacturer": self.lookup_table.get_manufacturer(_device["manufacturerId"]),
            "model": _device["model"]
        }

    def is_supported_device_type(self, device_type):
        file_path = path.join(self.base_devices_dir, f"{device_type}.lua")
        return os.path.isfile(file_path)
