#!/usr/bin/env python3

from dev_utils.base_builder import BaseBuilder
from dev_utils.converter import JsonConverter, SnakeCaseConverter
from zwave.device_signature_provider import generate_device_signature

import os
from os import path
import json

script_folder = path.dirname(path.realpath(__file__))


class DeviceConfigurationBuilder(BaseBuilder):

    def __init__(self, lookup_table):
        super().__init__(lookup_table)
        self.__case_converter = SnakeCaseConverter()
        self.__device_setting_file_path = path.join(script_folder, "configs", "device_setting.json")
        self.__settings = {}

    def initialize(self):
        if path.exists(self.__device_setting_file_path):
            os.remove(self.__device_setting_file_path)

    def finalize(self):
        self.create_file(
            self.__device_setting_file_path,
            json.dumps(JsonConverter().convert(self.__settings), sort_keys=True, indent=4)
        )

    def process_device(self, _device):
        device_setting = {}

        device_signature = generate_device_signature(_device)

        settings = self.lookup_table.get_settings(device_signature)
        assert not(settings and "configurations" in _device["variables"]), "Device settings and the 'configurations' variable must not be used together."

        if settings is not None:
            configurations = [setting["configuration"] for setting in settings if "value" in setting["configuration"]]
            if configurations:
                device_setting["configurations"] = configurations

        device_setting.update(self.process_variables(_device))
            
        if device_setting:
            self.__settings[device_signature] = device_setting

    def process_variables(self, _device: dict):
        supported_variables = {
            "configurations",  # Todo: remove this when all configurations are described as settings
            "associations",
            "wakeUpInterval",
            "shouldRequestNotifications",
            "supportedUserCodes",
            "ignoreCcInterview"
        }

        for variable_name, variable in _device["variables"].items():
            if variable_name in supported_variables:
                yield self.__case_converter.convert(variable_name), variable
