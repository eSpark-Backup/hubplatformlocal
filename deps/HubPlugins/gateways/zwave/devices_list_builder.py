#!/usr/bin/env python3

from dev_utils.base_builder import BaseBuilder
from dev_utils.converter import LuaConverter
from zwave.device_signature_provider import generate_device_signature

import os
from os import path


script_folder = path.dirname(path.realpath(__file__))


class DevicesListBuilder(BaseBuilder):

    def __init__(self, lookup_table):
        super().__init__(lookup_table)
        self.__lua_converter = LuaConverter()
        self.__device_setting_file_path = path.join(script_folder, "configs", "devices_list.lua")
        self.__devices = []

    def initialize(self):
        if path.exists(self.__device_setting_file_path):
            os.remove(self.__device_setting_file_path)

    def finalize(self):
        self.create_file(
            self.__device_setting_file_path,
            "return {}".format(self.__lua_converter.convert(self.__devices))
        )

    def process_device(self, _device):
        device_signature = generate_device_signature(_device)
        self.__devices.append(device_signature)
