#include "group_operations_utils.h"
#include "scenes_manager/modify_scene/scene_scale_modifier.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"
#include "jsonrpc/jsonrpc.h"
#include "common_utils/hub_info_helper.h"
#include "cored/model/values/value_factory.h"

namespace HubPlatform {
namespace ScenesManager {
namespace ScenesGroupOperationsUtils {

using namespace SceneEngine::Model::Resources;

bool checkStringParam( JsonObject& answer,
                       const JsonObject& params,
                       const std::string& key,
                       JsonPath& path,
                       bool required ) {
    auto gv = path.makeGuard( key );
    if( params.find( key ) != params.cend() ) {
        if( !params[ key ].is_string() ) {
            setErrorWithField( answer, Errors::WRONG_PARAMS, "", path.toString() );
            return false;
        }
    }
    else {
        if( required ) {
            setErrorWithField( answer, Errors::WRONG_FIELD, "", path.toString() );
            return false;
        }
    }
    return true;
}

bool checkBoolParam( JsonObject& answer,
                     const JsonObject& params,
                     const std::string& key,
                     JsonPath& path,
                     bool required ) {
    auto gv = path.makeGuard( key );
    if( params.find( key ) != params.cend() ) {
        if( !params[ key ].is_boolean() ) {
            setErrorWithField( answer, Errors::WRONG_PARAMS, "", path.toString() );
            return false;
        }
    }
    else {
        if( required ) {
            setErrorWithField( answer, Errors::WRONG_FIELD, "", path.toString() );
            return false;
        }
    }
    return true;
}

bool checkStringArrayParam( JsonObject& answer,
                            const JsonObject& params,
                            const std::string& key,
                            JsonPath& path,
                            bool required ) {
    auto gv = path.makeGuard( key );
    if( params.find( key ) != params.cend() ) {
        if( !params[ key ].is_array() ) {
            setErrorWithField( answer, Errors::WRONG_PARAMS, "", path.toString() );
            return false;
        }
        auto& strings = params[ key ];
        for( auto& element : strings ) {
            if( !element.is_string() ) {
                setErrorWithField( answer, Errors::WRONG_PARAMS, "", path.toString() );
                return false;
            }
        }
    }
    else {
        if( required ) {
            setErrorWithField( answer, Errors::WRONG_FIELD, "", path.toString() );
            return false;
        }
    }

    return true;
}

bool checkArrayParam( JsonObject& answer, const JsonObject& params, const std::string& key, JsonPath& path ) {
    auto gv = path.makeGuard( key );
    if( params.find( key ) != params.cend() ) {
        if( !params[ key ].is_array() ) {
            setErrorWithField( answer, Errors::WRONG_PARAMS, "", path.toString() );
            return false;
        }
    }
    return true;
}

bool checkArrayParam( JsonObject& answer,
                      const JsonObject& params,
                      const std::string& key,
                      JsonPath& path,
                      bool required ) {
    auto gv = path.makeGuard( key );
    if( params.find( key ) != params.cend() ) {
        if( !params[ key ].is_array() ) {
            setErrorWithField( answer, Errors::WRONG_PARAMS, "", path.toString() );
            return false;
        }

        auto& strings = params[ key ];
        for( auto& element : strings ) {
            if( !element.is_string() ) {
                setErrorWithField( answer, Errors::WRONG_PARAMS, "", path.toString() );
                return false;
            }
        }
    }

    return true;
}

bool areStringArraysEqual( const JsonArray& leftArray, const JsonArray& rightArray ) {
    if( !leftArray.is_array() || !rightArray.is_array() )
        return false;

    if( leftArray.size() != rightArray.size() )
        return false;

    std::vector< std::string > left = json_get_vec< std::string >( leftArray );
    std::vector< std::string > right = json_get_vec< std::string >( rightArray );

    std::sort( begin( left ), end( left ) );
    std::sort( begin( right ), end( right ) );

    std::vector< std::string > difference;
    std::set_difference( begin( right ), end( right ), begin( left ), end( left ), std::back_inserter( difference ) );

    return difference.empty();
}

bool areObjectArraysEqual( const JsonArray& leftArray, const JsonArray& rightArray ) {
    if( leftArray.size() != rightArray.size() ) {
        return false;
    }

    for( const auto& elementUpdated : leftArray ) {
        bool found = false;
        for( const auto& elementSaved : rightArray ) {
            if( elementUpdated == elementSaved ) {
                found = true;
                break;
            }
        }
        if( !found ) {
            return false;
        }
    }

    return true;
}

}  // namespace ScenesGroupOperationsUtils
}  // namespace ScenesManager
}  // namespace HubPlatform
