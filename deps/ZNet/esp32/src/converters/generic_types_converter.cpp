#include <znet/converters/generic_types_converter.hpp>

#include <znet/enums/generic_type.hpp>
#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

extern "C" {
#include <znet_log.h>
}

namespace znet {
namespace generic_types_converter {

using namespace resources::generic_types;

namespace {

struct GenericTypeData {
    const GenericType type;
    const char* const name;
};

const GenericTypeData genericTypeData[] = { { GenericType::GenericTypeSwitchMultilevel, GenericTypeSwitchMultilevel },
                                            { GenericType::GenericTypeSwitchBinary, GenericTypeSwitchBinary },
                                            { GenericType::GenericTypeSwitchRemote, GenericTypeSwitchRemote },
                                            { GenericType::GenericTypeSwitchToggle, GenericTypeSwitchToggle },
                                            { GenericType::GenericTypeThermostat, GenericTypeThermostat },
                                            { GenericType::GenericTypeEntryControl, GenericTypeEntryControl },
                                            { GenericType::GenericTypeWindowCovering, GenericTypeWindowCovering },
                                            { GenericType::GenericTypeGenericController, GenericTypeGenericController },
                                            { GenericType::GenericTypeWallController, GenericTypeWallController },
                                            { GenericType::GenericTypeStaticController, GenericTypeStaticController },
                                            { GenericType::GenericTypeRepeaterSlave, GenericTypeRepeaterSlave },
                                            { GenericType::GenericTypeNetworkExtender, GenericTypeNetworkExtender },
                                            { GenericType::GenericTypeAvControlPoint, GenericTypeAvControlPoint },
                                            { GenericType::GenericTypeAppliance, GenericTypeAppliance },
                                            { GenericType::GenericTypeSensorNotification,
                                              GenericTypeSensorNotification },
                                            { GenericType::GenericTypeSecurityPanel, GenericTypeSecurityPanel },
                                            { GenericType::GenericTypeSensorBinary, GenericTypeSensorBinary },
                                            { GenericType::GenericTypeSensorMultilevel, GenericTypeSensorMultilevel } };

const GenericTypeData* findGenericTypeData( GenericType type ) {
    for( const auto& data : genericTypeData ) {
        if( data.type == type ) {
            return &data;
        }
    }

    ZNET_LOGE( "Unsupported GenericTypeData: 0x%x\n", type );

    return nullptr;
}

}  // namespace

const char* typeToString( GenericType type ) noexcept {
    auto date = findGenericTypeData( type );
    return date ? date->name : Unknown;
}

}  // namespace generic_types_converter
}  // namespace znet
