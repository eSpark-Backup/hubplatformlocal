#!/usr/bin/env python3

import os
import sys
import argparse
from deps.opkg.build_packages import PluginPackageBuilder

directories_with_plugins = ["gateways", "extensions", "libraries", "plugins"]


def is_plugin_dir(dir_name):
    return os.path.isfile(os.path.join(dir_name, "config.json"))


def main():
    parser = argparse.ArgumentParser(description="Build OpenWrt packages for plugins.")
    parser.add_argument('--build', help='add third component to version of packages (e.g. MAJOR.MINOR.BUILD)')
    parser.add_argument('--openwrt_build_dir', help='package build directory')
    parser.add_argument('--compile_lua', action='store_true', help='Compile lua scripts before packing package')
    args = parser.parse_args()

    # getting directories
    utils_dir = os.path.dirname(os.path.realpath(__file__))
    root_dir = os.path.dirname(utils_dir)
    openwrt_build_dir = os.path.abspath(args.openwrt_build_dir) if args.openwrt_build_dir else os.path.join(root_dir, 'build', 'openwrt')
    tmp_dir = os.path.join(openwrt_build_dir, 'tmp')
    packages_dir = os.path.join(openwrt_build_dir, 'packages')

    builder = PluginPackageBuilder(args.build, ['*.lua', '*.json', '*.wav'], ['__pycache__', 'tests'], tmp_dir, packages_dir, args.compile_lua)

    for directory_with_plugins in directories_with_plugins:
        for dir_name, subdir_list, file_list in sorted(os.walk(os.path.join(root_dir, directory_with_plugins))):
            if not is_plugin_dir(dir_name):
                continue

            builder.Build(dir_name)


if __name__ == '__main__':
    sys.exit(main())
