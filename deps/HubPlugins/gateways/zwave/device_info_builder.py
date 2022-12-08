#!/usr/bin/env python3

from dev_utils.base_device_info_builder import BaseDeviceInfoBuilder
from zwave.device_signature_provider import generate_device_signature
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
            path.join(script_folder, "scripts", "model", "devices", "devices_info", "generated"),
            "device_info"
        )

    def generate_entries(self, _device):
        yield from super().generate_entries(_device)

        settings = self.lookup_table.get_settings(generate_device_signature(_device))
        if settings is not None:
            yield from self.process_device_settings(settings)

    def generate_device_file_path(self, _device):
        return path.join(self.devices_dir, "{}.lua".format(generate_device_signature(_device)))

    def get_file_skeleton_template(self):
        return template_skeleton

    def generate_device_info(self, _device):
        return {
            "manufacturer": self.lookup_table.get_manufacturer(_device["manufacturerId"]),
            "model": _device["model"]
        }
