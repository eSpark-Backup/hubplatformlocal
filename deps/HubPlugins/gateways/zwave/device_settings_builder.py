#!/usr/bin/env python3

from dev_utils.base_builder import BaseBuilder
from dev_utils.converter import LuaConverter
from dev_utils.setting_validator import validate_settings
from zwave.device_signature_provider import generate_device_signature

from os import path
import shutil


script_folder = path.dirname(path.realpath(__file__))


class DeviceSettingsBuilder(BaseBuilder):

    def __init__(self, lookup_table):
        super().__init__(lookup_table)
        self.__lua_converter = LuaConverter()
        self.__device_setting_file_path = path.join(script_folder, "scripts", "model", "devices", "device_settings")

    def initialize(self):
        if path.isdir(self.__device_setting_file_path):
            shutil.rmtree(self.__device_setting_file_path)

    def process_device(self, _device):
        device_signature = generate_device_signature(_device)

        settings = self.lookup_table.get_settings(device_signature)

        if settings is not None:
            validate_settings(settings)
            idx = 0
            for setting in settings:
                idx = idx + 1
                file_path = path.join(self.__device_setting_file_path, device_signature, "{}.lua".format(idx))
                self.create_file(file_path, "return {}".format(self.__lua_converter.convert(setting)))
