#include <znet/converters/central_scene_attributes_converter.hpp>

#include <znet/enums/central_scene_attributes.hpp>
#include <znet/converters/resources.hpp>
#include <znet/znet_utils.hpp>

extern "C" {
#include <znet_log.h>
}

namespace znet {
namespace central_scenes_attributes_converter {

using namespace resources::cc_central_scene_attributes;

namespace {

struct BarrierOperatorStateData {
    const CentralSceneAttributes attribute;
    const char* const name;
};

const BarrierOperatorStateData cantralSceneAttrinutesData[] = {
    { CentralSceneAttributes::Press1Time, Press1Time },   { CentralSceneAttributes::Released, Released },
    { CentralSceneAttributes::HeldDown, HeldDown },       { CentralSceneAttributes::Press2Times, Press2Times },
    { CentralSceneAttributes::Press3Times, Press3Times }, { CentralSceneAttributes::Press4Times, Press4Times },
    { CentralSceneAttributes::Press5Times, Press5Times },
};

const BarrierOperatorStateData* findAttributeData( CentralSceneAttributes attribute ) {
    for( const auto& data : cantralSceneAttrinutesData ) {
        if( data.attribute == attribute ) {
            return &data;
        }
    }

    ZNET_LOGE( "Unsupported CentralSceneAttributes(%d)\n\n", attribute );

    return nullptr;
}

}  // namespace

std::vector<std::string> maskToStrings( uint8_t bit_mask ) noexcept {
    std::vector<std::string> result;

    int start = static_cast<int>( CentralSceneAttributes::Start );
    int end = static_cast<int>( CentralSceneAttributes::End );

    for( int i = start; i <= end; ++i ) {
        if( bit_mask & ( 1 << i ) ) {
            const char* attribute_name = stateToString( static_cast<CentralSceneAttributes>( i ) );
            if( attribute_name ) {
                result.push_back( attribute_name );
            }
        }
    }

    return result;
}

const char* stateToString( CentralSceneAttributes state ) noexcept {
    auto date = findAttributeData( state );
    return date ? date->name : nullptr;
}

}  // namespace central_scenes_attributes_converter
}  // namespace znet
