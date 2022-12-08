#!/usr/bin/env python3

from abc import ABCMeta, abstractmethod
import re


class BaseConverter(metaclass=ABCMeta):

    @abstractmethod
    def convert(self, _value):
        pass


class SnakeCaseConverter(BaseConverter):
    camel_pattern = re.compile('((?<=[a-z0-9])[A-Z]|(?!^)[A-Z](?=[a-z]))')

    def convert(self, _string):
        return self.camel_pattern.sub(r'_\1', _string).lower()


class LuaConverter(BaseConverter):

    def __init__(self):
        self.__caseConverter = SnakeCaseConverter()

    def convert(self, _value):
        if isinstance(_value, str):
            return self.__to_string(_value)
        elif isinstance(_value, bool): # must be before `isinstance(_value, (int, float)`
            return self.__to_boolean(_value)
        elif isinstance(_value, (int, float)):
            return self.__to_number(_value)
        elif isinstance(_value, dict):
            return self.__to_table(_value)
        elif isinstance(_value, (list, tuple)):
            return self.__to_array(_value)
        else:
            raise ValueError('Cannot convert to lua value: ' + str(_value))

    def __to_string(self, _value):
        return repr(_value)

    def __to_number(self, _value):
        return _value

    def __to_boolean(self, _value):
        return 'true' if _value else 'false'

    def __to_table(self, _table):
        pairs = ['{0} = {1}'.format(self.__caseConverter.convert(key), self.convert(value)) for key, value in _table.items()]
        return '{{{0}}}'.format(', '.join(str(x) for x in pairs))

    def __to_array(self, _array):
        items = [str(self.convert(item)) for item in _array]
        return '{{{0}}}'.format(', '.join(str(item) for item in items))


# Convert dictionary keys
class JsonConverter:

    def __init__(self):
        self.__caseConverter = SnakeCaseConverter()

    def convert(self, _value):
        if isinstance(_value, dict):
            return self.__convert_dict(_value)
        elif isinstance(_value, list):
            return self.__convert_list(_value)
        else:
            return _value

    def __convert_dict(self, _dict):
        return {self.__caseConverter.convert(key): self.convert(value) for key, value in _dict.items()}

    def __convert_list(self, _list):
        return [self.convert(item) for item in _list]
