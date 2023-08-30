#
# Component Makefile
#

COMPONENT_NAME = znet

CFLAGS += -DBUILD_ZNET_LIBRARY
CXXFLAGS += -DBUILD_ZNET_LIBRARY

COMPONENT_ADD_INCLUDEDIRS = esp32/include

COMPONENT_SRCDIRS := . \
    datastructs \
    lib \
    lib/cc \
    lib/cc/Application \
    lib/cc/Managment \
    lib/cc/Network-Protocol \
    lib/cc/Transport-Encapsulation \
    lib/cc/Transport-Encapsulation/transport_service \
    lib/fc \
    libs2 \
    multitasking \
    serial_port \
    system_time \
    esp32/src \
    esp32/src/command_classes

COMPONENT_PRIV_INCLUDEDIRS = . \
    datastructs \
    lib \
    lib/cc \
    lib/cc/Application \
    lib/cc/Managment \
    lib/cc/Network-Protocol \
    lib/cc/Transport-Encapsulation \
    lib/cc/Transport-Encapsulation/transport_service \
    lib/fc \
    libs2 \
    multitasking \
    serial_port \
    system_time \
    esp32/src
