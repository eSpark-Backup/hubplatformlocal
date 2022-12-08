#!/usr/bin/env python3

from dev_utils.base_builder import BaseBuilder
from dev_utils.converter import LuaConverter

import os
from os import path


script_folder = path.dirname(path.realpath(__file__))


def get_template(file_name):
    with open(path.join(script_folder, "templates", "device_info", file_name), "r") as file:
        return file.read()


template_skeleton = get_template("supported_devices.lua.template")


class SupportedDevicesBuilder(BaseBuilder):

    def __init__(self, _lookup_table):
        super().__init__(_lookup_table)
        self.base_devices_dir = path.join(script_folder, "scripts", "model", "devices")
        self.devices_dir = path.join(self.base_devices_dir, "generated")
        self.supported_devices = {}
        self.lua_converter = LuaConverter()

    def finalize(self):
        content = self.generate_content_of_file()
        file_path = path.join(self.devices_dir, "supported_devices.lua")

        self.create_file(file_path, content)

    def process_device(self, _device):
        device_type = _device["variables"]["deviceType"]
        if not self.is_supported_device_type(device_type):
            return

        if device_type not in self.supported_devices:
            self.supported_devices[device_type] = []

        self.supported_devices[device_type].append(self.get_device_model(_device))

    def generate_content_of_file(self):
        models = (f"{device_type} = {self.lua_converter.convert(models)}" for device_type, models in self.supported_devices.items())

        return template_skeleton.format(',\n    '.join(models))

    def get_device_model(self, _device):
        manufacturer = self.lookup_table.get_manufacturer(_device["manufacturerId"])
        model = _device["model"]

        return f"{manufacturer}-{model}"

    def is_supported_device_type(self, device_type):
        file_path = path.join(self.base_devices_dir, f"{device_type}.lua")
        return os.path.isfile(file_path)
