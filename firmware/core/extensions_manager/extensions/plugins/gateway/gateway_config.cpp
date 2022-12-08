#include "extensions_manager/extensions/plugins/gateway/gateway_config.h"

#include "log_utils/ezlo_log.h"
#include "json_utils/json_utils.h"
#include "json_utils/json_custom_getters.h"
#include "common_utils/cpp14.h"

#include <stdexcept>

namespace HubPlatform {
namespace ExtensionsManager {

namespace {
/* a section of data within a plugin's config specific to 'gateway' */
constexpr const char DataSectionName[] = "gateway";
}  // namespace

std::unique_ptr< GatewayConfig > GatewayConfig::getGatewayConfig( const Json& joConfig ) {
    using namespace JsonCustomGetters;
    using StringChecking = CheckJsonStringStrictPolicy;

    using UnreachableReason = GatewayUnreachableReasons::value_type;
    auto unreachableReasonsGetter = makeVectorGetter(
        makeStructGetter( makeMemberMapping( &UnreachableReason::first, "reason", StringChecking() ),
                          makeMemberMapping( &UnreachableReason::second, "advice", StringChecking() ) ) );

    using UnreachableAction = GatewayUnreachableActions::value_type;
    auto unreachableActionsGetter = makeVectorGetter(
        makeStructGetter( makeMemberMapping( &UnreachableAction::first, "label", StringChecking() ),
                          makeMemberMapping( &UnreachableAction::second, "method", StringChecking() ) ) );

    auto operationsGetter =
        makeVectorGetter( makeStructGetter( makeMemberMapping( &Operation::m_template, "template" ),
                                            makeMemberMapping( &Operation::m_name, "operation" ) ) );

    const auto configGetter = makeFieldGetter(
        DataSectionName,
        makeStructGetter(
            makeMemberMapping( &GatewayConfig::m_name, "name", StringChecking() ),
            makeMemberMapping( &GatewayConfig::m_label, "label", StringChecking() ),
            makeMemberMapping( &GatewayConfig::m_reason, "reason", OptionalMemberPolicy() ),
            makeMemberMapping( &GatewayConfig::m_setItemValueCommand, "setItemValueCommand", StringChecking() ),
            makeMemberMapping(
                &GatewayConfig::m_setItemValueResponsePolicy, "setItemValueResponsePolicy", OptionalMemberPolicy() ),
            makeMemberMapping( &GatewayConfig::m_addItemDictionaryValueCommand,
                               "addItemDictionaryValueCommand",
                               OptionalMemberPolicy() ),
            makeMemberMapping( &GatewayConfig::m_setItemDictionaryValueCommand,
                               "setItemDictionaryValueCommand",
                               OptionalMemberPolicy() ),
            makeMemberMapping( &GatewayConfig::m_removeItemDictionaryValueCommand,
                               "removeItemDictionaryValueCommand",
                               OptionalMemberPolicy() ),
            makeMemberMapping(
                &GatewayConfig::m_clearItemDictionaryCommand, "clearItemDictionaryCommand", OptionalMemberPolicy() ),
            makeMemberMapping( &GatewayConfig::m_setGatewaySettingValueCommand,
                               "setGatewaySettingValueCommand",
                               OptionalMemberPolicy() ),
            makeMemberMapping(
                &GatewayConfig::m_setSettingValueCommand, "setSettingValueCommand", OptionalMemberPolicy() ),
            makeMemberMapping( &GatewayConfig::m_setSettingDictionaryValueCommand,
                               "setSettingDictionaryValueCommand",
                               OptionalMemberPolicy() ),
            makeMemberMapping( &GatewayConfig::m_resetSettingCommand, "resetSettingCommand", OptionalMemberPolicy() ),
            makeMemberMapping(
                &GatewayConfig::m_forceRemoveDeviceCommand, "forceRemoveDeviceCommand", OptionalMemberPolicy() ),
            makeMemberMapping(
                &GatewayConfig::m_updateDeviceFirmwareCommand, "updateDeviceFirmwareCommand", OptionalMemberPolicy() ),
            makeMemberMapping( &GatewayConfig::m_checkDeviceCommand, "checkDeviceCommand", OptionalMemberPolicy() ),
            makeMemberMapping(
                &GatewayConfig::m_unreachableReasons,
                makeFieldGetter(
                    "unreachableReasons", std::move( unreachableReasonsGetter ), OptionalMemberPolicy() ) ),
            makeMemberMapping(
                &GatewayConfig::m_unreachableActions,
                makeFieldGetter(
                    "unreachableActions", std::move( unreachableActionsGetter ), OptionalMemberPolicy() ) ),
            makeMemberMapping( &GatewayConfig::m_createAbstractSubscriptionCommand,
                               "createAbstractSubscriptionCommand",
                               OptionalMemberPolicy() ),
            makeMemberMapping( &GatewayConfig::m_updateAbstractSubscriptionCommand,
                               "updateAbstractSubscriptionCommand",
                               OptionalMemberPolicy() ),
            makeMemberMapping( &GatewayConfig::m_deleteAbstractSubscriptionCommand,
                               "deleteAbstractSubscriptionCommand",
                               OptionalMemberPolicy() ),
            makeMemberMapping(
                &GatewayConfig::m_operations,
                makeFieldGetter( "operations", std::move( operationsGetter ), OptionalMemberPolicy() ) ) ) );

    auto result = std::make_unique< GatewayConfig >();
    if( !configGetter( joConfig, *result ) ) {
        Log::error( cc::error( "Error during loading gateway config from json" ) );
        return nullptr;
    }

    return result;
}

}  // namespace ExtensionsManager
}  // namespace HubPlatform
