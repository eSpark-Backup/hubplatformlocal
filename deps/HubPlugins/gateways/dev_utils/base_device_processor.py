#!/usr/bin/env python3

class BaseDeviceProcessor:

    def __init__(self, _plugin_name, _builders):
        self.__plugin_name = _plugin_name
        self.__builders = _builders
        self.processed_devices_count = 0

    def initialize(self):
        for builder in self.__builders:
            builder.initialize()

    def finalize(self):
        print("Processed {} {} device(s)".format(self.processed_devices_count, self.__plugin_name))

        for builder in self.__builders:
            builder.finalize()

    def process_device(self, _device):
        for builder in self.__builders:
            builder.process_device(_device)

        self.processed_devices_count = self.processed_devices_count + 1
