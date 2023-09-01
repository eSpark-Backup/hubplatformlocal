#!/usr/bin/env python3

def generate_device_signature(_device):
    manufacturer = _device["manufacturerId"]
    product_type = _device["variables"]["productType"]
    device_id = _device["deviceId"]

    return "{}_{}_{}".format(manufacturer, product_type, device_id)
