#include "item_group_validator.h"
#include "scenes_manager/modify_scene/scene_scale_modifier.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"
#include "group_operations_utils.h"
#include "database_client/dbqueryparser.h"
#include "jsonrpc/jsonrpc.h"
#include "common_utils/hub_info_helper.h"
#include "cored/model/values/value_factory.h"
#include "cored/model/values/values.h"
#include "scenes_manager/scenes_engine/model/comparison_info.h"

#include "database_client/idbclient.h"

namespace HubPlatform {
namespace ScenesManager {
namespace ItemGroupValidator {

bool checkValueTypeFamily( const std::string& checkedFamily ) {
    for( const auto family : SceneEngine::Model::OperatorsMapper::valueTypeFamilies() ) {
        if( family.first == checkedFamily )
            return true;
    }
    return false;
}

JsonObject checkValueTypeFamilyWithValueType( const std::string& checkedFamily, const std::string& checkedValueType ) {
    for( const auto family : SceneEngine::Model::OperatorsMapper::valueTypeFamilies() ) {
        if( family.first == checkedFamily ) {
            for( const auto& type : family.second ) {
                if( type == checkedValueType ) {
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}

JsonObject validateCreateItemGroupRequest( const JsonObject& params, bool checkRequiredParams ) {
    using namespace SceneEngine::Model::Resources;
    using namespace HubPlatform::ScenesManager;
    JsonObject answer = JSON_EMPTY_OBJ;
    JsonPath path;
    auto gv = path.makeGuard( "params" );

    if( !ScenesGroupOperationsUtils::checkStringParam(
            answer, params, ItemGroup::Name, path, checkRequiredParams && true ) ||
        !ScenesGroupOperationsUtils::checkStringArrayParam(
            answer, params, ItemGroup::ItemNames, path, checkRequiredParams && true ) ||
        !ScenesGroupOperationsUtils::checkBoolParam( answer, params, ItemGroup::Persistent, path, false ) ||
        !ScenesGroupOperationsUtils::checkBoolParam( answer, params, ItemGroup::HasGetter, path, false ) ||
        !ScenesGroupOperationsUtils::checkBoolParam( answer, params, ItemGroup::HasSetter, path, false ) ||
        !ScenesGroupOperationsUtils::checkStringParam( answer, params, ItemGroup::ValueType, path, false ) ||
        !ScenesGroupOperationsUtils::checkStringArrayParam( answer, params, ItemGroup::Enum, path, false ) ||
        !ScenesGroupOperationsUtils::checkStringParam( answer, params, ItemGroup::ValueTypeFamily, path, false ) ) {
        return answer;
    }

    auto checkNameLength = [&answer]( const std::string& name ) {
        try {
            jsonCheckString( name );
        }
        catch( ... ) {
            setErrorWithField( answer, Errors::TOO_LONG, "", ItemGroup::Name );
        }
    };

    auto name = json_get_str( params, ItemGroup::Name );

    checkNameLength( name );
    if( !answer.empty() ) {
        return answer;
    }

    if( params.find( ItemGroup::ItemNames ) != params.cend() ) {
        auto& names = params[ ItemGroup::ItemNames ];
        for( auto nameObject : names ) {
            checkNameLength( json_get_str( nameObject ) );
            if( !answer.empty() ) {
                return answer;
            }
        }
    }

    if( params.find( ItemGroup::Enum ) != params.cend() ) {
        auto enumArray = params[ ItemGroup::Enum ];
        if( !enumArray.empty() ) {
            if( params.find( ItemGroup::ValueType ) == params.cend() ) {
                auto gv = path.makeGuard( ItemGroup::ValueType );
                setErrorWithField( answer, Errors::WRONG_PARAMS, "Item value type should be set", path.toString() );
                return answer;
            }
        }
    }

    if( params.find( ItemGroup::ValueType ) == params.cend() ) {
        if( !ScenesGroupOperationsUtils::checkStringParam(
                answer, params, ItemGroup::ValueTypeFamily, path, checkRequiredParams && true ) ) {
            return answer;
        }
    }

    using namespace CoreD::Model;
    auto valueType = json_get_str( params, ItemGroup::ValueType );
    if( !valueType.empty() ) {
        if( ValueType::fromString( valueType ) == ValueType::Enum::NoValue ) {
            auto gv = path.makeGuard( ItemGroup::ValueType );
            setErrorWithField( answer, Errors::WRONG_PARAMS, "Wrong item value type", path.toString() );
            return answer;
        }
    }

    using namespace CoreD::Model;
    auto valueTypeFamily = json_get_str( params, ItemGroup::ValueTypeFamily );
    if( !valueTypeFamily.empty() ) {
        if( !checkValueTypeFamily( valueTypeFamily ) ||
            ( !valueType.empty() && !checkValueTypeFamilyWithValueType( valueTypeFamily, valueType ) ) ) {
            auto gv = path.makeGuard( ItemGroup::ValueTypeFamily );
            setErrorWithField( answer, Errors::WRONG_PARAMS, "Wrong item value type family", path.toString() );
            return answer;
        }
    }

    return answer;
}

}  // namespace ItemGroupValidator
}  // namespace ScenesManager
}  // namespace HubPlatform
