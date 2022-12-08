#include <znet/converters/configuration_format_converter.hpp>

#include <znet/enums/configuration_format.hpp>
#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

extern "C" {
#include <znet_log.h>
}

namespace znet {
namespace configuration_format_converter {

using namespace resources::cc_configuration_format;

namespace {

struct ConfigurationFormatData {
    const ConfigurationFormat format;
    const char* const name;
};

const ConfigurationFormatData configurationFormatData[] = { { ConfigurationFormat::SignedInteger, SignedInteger },
                                                            { ConfigurationFormat::UnsignedInteger, UnsignedInteger },
                                                            { ConfigurationFormat::Enumerated, Enumerated },
                                                            { ConfigurationFormat::BitField, BitField } };

const ConfigurationFormatData* findConfigurationFormatData( ConfigurationFormat format ) {
    for( const auto& data : configurationFormatData ) {
        if( data.format == format ) {
            return &data;
        }
    }

    ZNET_LOGE( "Unsupported ConfigurationFormat %u\n", format );

    return nullptr;
}

const ConfigurationFormatData* findConfigurationFormatData( const char* name ) {
    for( const auto& data : configurationFormatData ) {
        if( strcmp( data.name, name ) == 0 ) {
            return &data;
        }
    }

    ZNET_LOGE( "Unsupported ConfigurationFormat name %s\n", name );

    return nullptr;
}

}  // namespace

const char* formatToString( ConfigurationFormat format ) noexcept {
    auto date = findConfigurationFormatData( format );
    return date ? date->name : nullptr;
}

ConfigurationFormat stringToSignal( const char* name ) noexcept {
    auto date = findConfigurationFormatData( name );
    return date ? date->format : ConfigurationFormat::Unknown;
}

}  // namespace configuration_format_converter
}  // namespace znet
