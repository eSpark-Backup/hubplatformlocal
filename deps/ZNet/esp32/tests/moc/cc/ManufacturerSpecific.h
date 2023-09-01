#ifndef MANUFACTURERSPECIFIC_H_
#define MANUFACTURERSPECIFIC_H_

#include "../cc/CommanClass.h"

class ManufacturerSpecific : public CommandClass {
public:
    using idStruct_type = struct {
        std::uint16_t manufacturerId;
        std::uint16_t productTypeId;
        std::uint16_t productId;
    };

public:
    ManufacturerSpecific( idStruct_type idStruct );
    ManufacturerSpecific() = delete;
    virtual ~ManufacturerSpecific() = default;
    bool run( std::vector<std::vector<uint8_t>>& rev,
              std::vector<uint8_t>& ret,
              std::vector<uint8_t> const& data ) override;

private:
    enum Command { version = 0x01, get = 0x04, report = 0x05 };

private:
    idStruct_type m_idStruct;
};

#endif /* MANUFACTURERSPECIFIC_H_ */
