#ifndef ADDNODETONETWORK_H_
#define ADDNODETONETWORK_H_

#include <vector>
#include "fc/Command.h"

class AddNodeToNetwork : public Command {
public:
    enum addMode {
        NODE_ANY = 0x01,          ///< Command to initiate inclusion (add) of new node
                                  ///< of any type
        NODE_CONTROLLER = 0x02,   ///< to add/remove/create new primary/controlle_r change
        NODE_SLAVE = 0x03,        ///< to add remove slave node
        NODE_EXISTING = 0x04,     ///< for add/remove node
        NODE_STOP = 0x05,         ///< to stop learn mode without error
        NODE_STOP_FAILED = 0x06,  ///< for add node/create new primary
        RESERVED = 0x07,
        HOME_ID = 0x08,      ///< Command for initiating the
                             ///< SMART START inclusion of a
                             ///< specific SMART START node
        SMART_START = 0x09,  ///< Command for entering SMART START Listen mode

        MODE_MASK = 0x0F,
        OPTION_NORMAL_POWER = 0x80,
        OPTION_NETWORK_WIDE = 0x40

    };
    enum nodeStatus {
        LEARN_READY = 0x01,        ///< Z-Wave protocol is ready to include new node
        NODE_FOUND = 0x02,         ///< Z-Wave protocol detected node
        ADDING_SLAVE = 0x03,       ///< Z-Wave protocol included a slave type node
        ADDING_CONTROLLER = 0x04,  ///< Z-Wave protocol included a controller type node
        PROTOCOL_DONE = 0x05,      ///< Z-Wave protocol completed operations related
                                   ///< to inclusion (add)
        DONE = 0x06,               ///< All operations completed. Protocol is ready to return
                                   ///< to idle state
        FAILED = 0x07,             ///< Z-Wave protocol reports that inclusion (add) was
                                   ///< not successful
        NOT_PRIMARY = 0x23         ///< Z-Wave protocol reports that the requested operation
                                   ///< cannot be performed since it requires that the node is in
                                   ///< primary controller state
    };

    AddNodeToNetwork( ZwaveHub& hub, std::vector<uint8_t> const& pack );
    AddNodeToNetwork() = delete;
    virtual ~AddNodeToNetwork() = default;
    virtual void execute() override;

private:
    std::vector<uint8_t> const& m_pack;
};

#endif /* ADDNODETONETWORK_H_ */
