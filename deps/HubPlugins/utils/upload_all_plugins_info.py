#!/usr/bin/env python3

import os
import sys
import argparse
from deps.marketplace.upload_plugins_info import PluginInfoUploader

directories_with_plugins = ["gateways", "extensions", "libraries"]

def is_plugin_dir(dir_name):
    return os.path.isfile(os.path.join(dir_name, "config.json"))

def main():
    parser = argparse.ArgumentParser(description="Upload information about all plugins to Cloud Marketplace.")
    parser.add_argument('--build', help='add third component to version of packages (e.g. MAJOR.MINOR.BUILD)', required=True)
    parser.add_argument('--server', help='The server URL for uploading plugin information', required=True)
    parser.add_argument('--token', help='Access token', required=True)
    args = parser.parse_args()

    # getting directories
    utils_dir = os.path.dirname(os.path.realpath(__file__))
    root_dir = os.path.dirname(utils_dir)

    pluginInfoUploader = PluginInfoUploader( args.build, args.server, args.token )

    for directory_with_plugins in directories_with_plugins:
        for dir_name, subdir_list, file_list in sorted(os.walk(os.path.join(root_dir, directory_with_plugins))):
            if not is_plugin_dir(dir_name):
                continue

            pluginInfoUploader.Upload(dir_name)

if __name__ == '__main__':
    sys.exit(main())
