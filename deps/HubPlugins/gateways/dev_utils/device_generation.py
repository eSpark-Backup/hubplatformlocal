#!/usr/bin/env python3

import os
import os.path as path
import jsonref
import importlib
import sys

dev_utils_dir = path.dirname(path.realpath(__file__))
gateways_dir = path.dirname(dev_utils_dir)

sys.path.append(gateways_dir)
os.chdir(gateways_dir)

class LookupTable:

    def __init__(self, _devices_info, _device_settings):
        self.__data = _devices_info
        self.__settings = _device_settings

    def get_plugin(self, _id):
        return self.__data["plugins"][_id]

    def get_manufacturer(self, _id):
        return self.__data["manufacturers"][_id]

    def get_type(self, _id):
        return self.__data["types"][_id]

    def get_category(self, _id):
        data = self.__get_category_data(_id)
        return data["name"]

    def get_subcategory(self, _category_id, _subcategory_id):
        data = self.__get_category_data(_category_id)
        subcategories = data["subcategories"]
        if _subcategory_id in subcategories:
            return subcategories[_subcategory_id]
        return None

    def get_settings(self, _device_id):
        return self.__settings.get(_device_id)

    def __get_category_data(self, _id):
        return self.__data["categories"][_id]


class DeviceGenerator:

    def __init__(self):
        self.__plugins = {}

        devices_info_file = path.join(dev_utils_dir, "devices_info.json")
        device_settings_file = path.join(dev_utils_dir, "device_settings.json")

        self.__devicesInfo = self.__load_json_file(devices_info_file)
        self.__deviceSettings = self.__load_json_file(device_settings_file)

        self.__lookupTable = LookupTable(self.__devicesInfo, self.__deviceSettings)

    def run(self):
        self.__collect_plugins()
        self.__initialize_processors()
        self.__process_devices()
        self.__finalize_processors()

    def __process_devices(self):
        for device in self.__devicesInfo["devices"]:
            self.__process_device(device)

    def __process_device(self, _device):
        processor = self.__get_suitable_dev_processor(_device)
        processor.process_device(_device)

    def __collect_plugins(self):
        for dir_name, subdir_list, file_list in os.walk(gateways_dir):
            if not self.__is_plugin_dir(dir_name):
                continue

            plugin = self.__get_plugin_id(dir_name)
            instance = self.__get_dev_processor_instance(plugin)
            self.__plugins[plugin] = instance

    def __initialize_processors(self):
        for plugin, processor in self.__plugins.items():
            if processor:
                processor.initialize()

    def __finalize_processors(self):
        for plugin, processor in self.__plugins.items():
            if processor:
                processor.finalize()

    def __is_plugin_dir(self, _dir_name):
        return path.isfile(path.join(_dir_name, "config.json"))

    def __get_plugin_id(self, _plugin_dir):
        config = self.__load_json_file(path.join(_plugin_dir, "config.json"))
        return config["id"]

    def __get_dev_processor_instance(self, _package):
        if os.path.isfile("{}/{}.py".format(_package, "device_processor")):
            module_name = "{}.{}".format(_package, "device_processor")
            module = importlib.import_module(module_name)
            class_type = getattr(module, "DeviceProcessor")
            return class_type(self.__lookupTable, _package)

        return None

    def __get_suitable_dev_processor(self, _device):
        plugin = self.__lookupTable.get_plugin(_device["pluginId"])
        return self.__plugins[plugin]

    def __load_json_file(self, _file_name):
        with open(_file_name, 'r') as file:
            return jsonref.load(file)


if __name__ == "__main__":
    generator = DeviceGenerator()
    generator.run()
