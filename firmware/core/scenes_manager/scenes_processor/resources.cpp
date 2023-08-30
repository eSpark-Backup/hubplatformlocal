#include "scenes_manager/scenes_processor/resources.h"

namespace HubPlatform {
namespace ScenesManager {

namespace ActionBlockTypes {
const char* const Then = "then";
const char* const Else = "else";
const char* const ThenGroups = "thenGroups";
const char* const ElseGroups = "elseGroups";
}  // namespace ActionBlockTypes

namespace ActionFields {

namespace Fields {
const char* const ResetType = "type";
const char* const Block = "block";
const char* const Group = "group";
const char* const SceneId = "sceneId";
const char* const BlockId = "blockId";
const char* const Type = "type";
const char* const Name = "name";
};  // namespace Fields

namespace ExpressionFields {
const char* const Name = "name";
const char* const Type = "type";
const char* const Code = "code";
const char* const ValueType = "valueType";
const char* const Error = "error";
const char* const Variable = "variable";
const char* const Metadata = "metadata";
const char* const Params = "params";
const char* const Item = "item";
const char* const Expression = "expression";
}  // namespace ExpressionFields

namespace Values {
const char* const ResetTypeSoft = "soft";
const char* const ResetTypeFactory = "factory";
const char* const BlockReference = "block_reference";
const char* const ExpressionReference = "expression_reference";
}  // namespace Values

namespace ActionNames {
const char* const RebootHub = "rebootHub";
const char* const ResetHub = "resetHub";
const char* const ResetLatch = "resetLatch";
const char* const ResetSceneLatches = "resetSceneLatches";
const char* const RunScene = "runScene";
const char* const RunCustomScript = "runCustomScript";
const char* const RunPluginScript = "runPluginScript";

const char* const SwitchHouseMode = "switchHouseMode";
const char* const SetItemValue = "setItemValue";
const char* const SetDeviceArmed = "setDeviceArmed";
const char* const SendCloudAbstractCommand = "sendCloudAbstractCommand";
const char* const SendHttpRequest = "sendHttpRequest";
const char* const SetSceneState = "setSceneState";
const char* const CloudAPI = "cloudAPI";
const char* const SetExpression = "setExpression";
const char* const SetVariable = "setVariable";
const char* const ToggleValue = "toggleValue";
}  // namespace ActionNames

}  // namespace ActionFields

}  // namespace ScenesManager
}  // namespace HubPlatform
