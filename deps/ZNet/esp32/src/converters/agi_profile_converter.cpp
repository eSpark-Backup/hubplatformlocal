#include <znet/converters/agi_profile_converter.hpp>
#include <znet/converters/multilevel_sensor_converter.hpp>

#include <znet/znet_notification.hpp>
#include <znet/znet_meter.hpp>
#include <znet/converters/resources.hpp>

#include <functional>

extern "C" {
#include <znet_log.h>
}

namespace znet {
namespace agi_profile_converter {

using namespace resources::agi_profiles;

namespace {

struct AgiProfileData {
    const uint8_t msb;
    const char* const masterName;
    const std::function<std::string( uint8_t )> lsbProcessor;
};

std::string processGeneralProfile( uint8_t lsb ) {
    switch( lsb ) {
    case 0x00:
        return NA;
    case 0x01:
        return Lifeline;
    default:
        ZNET_LOGW( "Unsupported general AGI profile." );
        return "Unknown";
    }
}

std::string uintToString( uint8_t value ) {
    return ( value < 10 ? std::string( "0" ) : std::string() ) + std::to_string( value );
}

std::string processKeyProfile( uint8_t lsb ) {
    return Key + uintToString( lsb );
}

std::string processIrrigationProfile( uint8_t lsb ) {
    return std::string( Channel ) + " " + uintToString( lsb );
}

std::string prettyTypeString( const char* typeName ) {
    if( !typeName ) {
        return "Unknown";
    }
    else {
        std::string result = typeName;
        for( int i = 0; i < result.size(); ++i ) {
            if( i == 0 ) {
                result[i] = std::toupper( result[i] );
            }
            else if( result[i] == '_' ) {
                result[i] = ' ';
                if( i < result.size() - 1 ) {
                    result[i + 1] = std::toupper( result[i + 1] );
                }
            }
        }
        return result;
    }
}

std::string processSensorProfile( uint8_t lsb ) {
    return prettyTypeString( multilevel_sensor_converter::typeToString( static_cast<MultilevelSensor::Type>( lsb ) ) );
}

std::string processNotificationProfile( uint8_t lsb ) {
    return prettyTypeString( getNotificationName( static_cast<NotificationType>( lsb ) ) );
}

std::string processMeterProfile( uint8_t lsb ) {
    return prettyTypeString( meter::typeToString( static_cast<meter::MeterType>( lsb ) ) );
}

const AgiProfileData agiProfileData[] = { { 0x00, General, &processGeneralProfile },
                                          { 0x20, Control, &processKeyProfile },
                                          { 0x31, Sensor, &processSensorProfile },
                                          { 0x71, resources::agi_profiles::Notification, &processNotificationProfile },
                                          { 0x32, Meter, &processMeterProfile },
                                          { 0x6b, Irrigation, &processIrrigationProfile } };

const AgiProfileData* findAgiProfileData( uint8_t msb ) {
    for( const auto& data : agiProfileData ) {
        if( data.msb == msb ) {
            return &data;
        }
    }

    ZNET_LOGE( "Unsupported AGI profile\n\n" );

    return nullptr;
}

}  // namespace

std::string profileToString( uint8_t msb, uint8_t lsb ) noexcept {
    auto data = findAgiProfileData( msb );
    if( data ) {
        return std::string( data->masterName ) + ":" + data->lsbProcessor( lsb );
    }

    return "";
}

}  // namespace agi_profile_converter
}  // namespace znet
