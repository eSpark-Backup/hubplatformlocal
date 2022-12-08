#!/usr/bin/env python3

import os
import shutil
from base_builder import BaseBuilder
from converter import LuaConverter, SnakeCaseConverter
from abc import ABCMeta, abstractmethod


class BaseDeviceInfoBuilder(BaseBuilder, metaclass=ABCMeta):

    def __init__(self, _lookup_table, _devices_dir, handler_name = 'handler'):
        super().__init__(_lookup_table)
        self.devices_dir = _devices_dir
        self.handler_name = handler_name
        self.__lua_converter = LuaConverter()
        self.__case_converter = SnakeCaseConverter()

    def initialize(self):
        if os.path.isdir(self.devices_dir):
            shutil.rmtree(self.devices_dir)

    def process_device(self, _device):
        file_path = self.generate_device_file_path(_device)

        entries = "\n".join(self.generate_entries(_device))
        content = self.get_file_skeleton_template().format(self.handler_name, entries)

        self.create_file(file_path, content)

    def generate_entries(self, _device):
        for key, value in self.base_device_info_items(_device):
            yield self.create_entry(key, value)

        device_info = self.generate_device_info(_device)
        if device_info is not None:
            yield self.create_entry("info", device_info)

        variables = _device.get("variables")
        if variables is not None:
            yield from self.process_device_variables(variables)

    def base_device_info_items(self, _device):
        name = _device.get("name")
        if name is not None:
            yield "name", name

        type_id = _device.get("typeId")
        if type_id is not None:
            device_type = self.lookup_table.get_type(type_id)
            yield "type", device_type

        category_id = _device.get("categoryId")
        if category_id is not None:
            category = self.lookup_table.get_category(category_id)
            yield "category", category

            subcategory_id = _device.get("subcategoryId")
            if subcategory_id is not None:
                subcategory = self.lookup_table.get_subcategory(category_id, subcategory_id)
                yield "subcategory", subcategory or ""
            else:
                yield "subcategory", ""

    def process_variables(self, _variables, _supported_variables, _custom_handlers = {}):
        for variable_name in _supported_variables:
            if variable_name in _variables:
                handler = _custom_handlers.get(variable_name, self.process_generic_variable)
                yield from handler(variable_name, _variables[variable_name])

    def process_device_variables(self, _device_variables):
        supported_variables = [
            "sceneActions",
            "simpleAvControlActions",
            "supportedItemStates",
            "supportedUserCodes",
            "requestValueDelay",
            "hailProducingCommandClasses",
            "attributesConfigurations",
            "minMaxValues",
            "fullBatteryVoltage",
            "hasIasZoneTamper",
            "mfgSpecificAttributesConfigurations",
            "endpointsInfo",
            "panicTriggers",
            "panicButtonPolicy",
            "subdevicesMergePolicy",
            "thermostatSetpointsSpread",
            "unsetPinCodesCountToStopRequesting"
        ]

        custom_handlers = {
            "endpointsInfo": self.process_endpoints,
            "supportedUserCodes": self.process_user_codes,
            "attributesConfigurations": self.process_attributes_configurations,
            "mfgSpecificAttributesConfigurations": self.process_mfg_specific_attributes_configurations,
        }

        yield from self.process_variables(_device_variables, supported_variables, custom_handlers)

    def process_device_settings(self, _settings):
        yield self.create_entry('settings_count', len(_settings))

    def process_generic_variable(self, _name, _value):
        yield self.create_entry(self.__case_converter.convert(_name), _value)

    def process_endpoints(self, _name, _endpoints_info):
        entry_name = self.__case_converter.convert(_name)
        yield self.create_entry(entry_name, [])

        def channel_items(endpoint_record):
            ignored = endpoint_record.get("ignored")
            if ignored is True:
                yield "ignored", ignored
            else:
                merge_with = endpoint_record.get("mergeWith")
                if merge_with is not None:
                    yield "merge_with", merge_with
                else:
                    yield from self.base_device_info_items(endpoint_record)
            readonly = endpoint_record.get("readonly")
            if readonly is not None:
                yield "readonly", readonly

        for record in _endpoints_info:
            yield self.create_entry("{}[{}]".format(entry_name, record["channelId"]), dict(channel_items(record)))

    def process_user_codes(self, _name, _user_codes):
        supported_variables = [
            "firstUserCode",
            "endUserCode",
            "userCodeRestriction",
            "ignoreUserCodeModes"
        ]

        yield from self.process_variables(_user_codes, supported_variables)

    def process_attributes_configurations(self, _name, _attributes_configurations):
        for configuration in _attributes_configurations:
            yield self.create_attribute_configuration_entry("attr_configurations", "clusterName", "attributeName", configuration)

    def process_mfg_specific_attributes_configurations(self, _name, _attributes_configurations):
        for configuration in _attributes_configurations:
            yield self.create_attribute_configuration_entry("mfg_specific_attr_configurations", "clusterId", "attributeId", configuration)

    def create_entry(self, _key, _value, _handler_name=None):
        return '{0}.{1} = {2}'.format(_handler_name or self.handler_name, _key, self.__lua_converter.convert(_value))

    def create_attribute_configuration_entry(self, _name, _cluster_key, _attribute_key, _configuration):
        return 'if not {0}.{1}["{2}"] then {0}.{1}["{2}"] = {{}} end; {0}.{1}["{2}"]["{3}"] = {4}'.format(
            self.handler_name,
            _name,
            _configuration[_cluster_key],
            _configuration[_attribute_key],
            self.__lua_converter.convert(_configuration)
        )

    @abstractmethod
    def generate_device_file_path(self, _device):
        pass

    @abstractmethod
    def get_file_skeleton_template(self):
        pass

    @abstractmethod
    def generate_device_info(self, _device):
        pass
