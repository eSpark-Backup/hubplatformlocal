#ifndef DEFS_H_
#define DEFS_H_

#include <cstdint>

namespace zdef {

enum Ack : uint8_t { ACK = 0x06, NAC = 0x15 };

enum Sof : uint8_t { SOF = 0x01 };

enum Type : uint8_t { Request = 0x00, Respons = 0x01 };

enum RetVal : uint8_t { False = 0x00, True = 0x01 };

enum FunctionClassId : uint8_t {
    DefaultCommand = 0x00,
    ApplicationNodeInformation = 0x03,
    ApplicationCommandHandler = 0x04,
    SendData = 0x13,
    SendDataMulti = 0x14,
    LibraryVersion = 0x15,
    ApplicationNodeInformationCommandClasses = 0x20,
    NVRGetValue = 0x28,
    SetThreshold = 0x3C,
    GetNodeProtocolInfo = 0x41,
    AddNodeToNetwork = 0x4A,
    SetSUCNodeID = 0x54,
    GetSUCNodeID = 0x56,
    SendDataBridge = 0xA9,
    SendDataMultiBridge = 0xAB,
    TypeLibrary = 0xBD,
};

}  // namespace zdef

#endif  // DEFS_H_
