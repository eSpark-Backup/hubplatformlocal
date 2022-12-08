#!/usr/bin/env python3

from abc import ABCMeta, abstractmethod
import os


class BaseBuilder(metaclass=ABCMeta):

    def __init__(self, _lookup_table):
        self.__lookupTable = _lookup_table

    def initialize(self):
        pass

    def finalize(self):
        pass

    @abstractmethod
    def process_device(self, _device):
        pass

    def create_file(self, file_path, content):
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
        with open(file_path, 'w') as file:
            file.write(content)

    @property
    def lookup_table(self):
        return self.__lookupTable
