#include "scenes_manager/scenes_engine/model/comparison_info.h"
#include "scenes_manager/scenes_engine/model/event_names.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"

namespace SceneEngine {
namespace Model {

const std::string ComparisonMethodsVersion = "1.0.0";

const OperatorsMapper::EventNameEnumOperatorInfoMap& OperatorsMapper::operatorInfoMap() {
    using namespace SceneEngine::Model::Resources;

    static EventNameEnumOperatorInfoMap map = {

        { EventName::Enum::compareNumberRange,
          { { { { ValueComparators::Between, Labels::Between }, { ValueComparators::NotBetween, Labels::NotBetween } },
              MethodNames::compareNumberRange,
              ValueFamilies::Numeric,
              "enum" } } },

        { EventName::Enum::compareNumbers,
          { { { { { ValueComparators::Less, Labels::Less },
                  { ValueComparators::Greater, Labels::Greater },
                  { ValueComparators::LessEqual, Labels::LessEqual },
                  { ValueComparators::GreaterEqual, Labels::GreaterEqual },
                  { ValueComparators::Equal, Labels::Equal },
                  { ValueComparators::NotEqual, Labels::NotEqual } } },
              MethodNames::compareNumbers,
              ValueFamilies::Numeric,
              "enum" } } },

        { EventName::Enum::compareStrings,
          { { { { { ValueComparators::Less, Labels::Less },
                  { ValueComparators::Greater, Labels::Greater },
                  { ValueComparators::LessEqual, Labels::LessEqual },
                  { ValueComparators::GreaterEqual, Labels::GreaterEqual },
                  { ValueComparators::Equal, Labels::Equal },
                  { ValueComparators::NotEqual, Labels::NotEqual } } },
              MethodNames::compareStrings,
              ValueFamilies::Strings,
              "enum" } } },

        { EventName::Enum::stringOperation,
          { { { { StringOperations::Begin, Labels::Begin },
                { StringOperations::End, Labels::End },
                { StringOperations::Contain, Labels::Contain },
                { StringOperations::Length, Labels::Length },
                { StringOperations::NotBegin, Labels::NotBegin },
                { StringOperations::NotEnd, Labels::NotEnd },
                { StringOperations::NotContain, Labels::NotContain },
                { StringOperations::NotLength, Labels::NotLength } },
              MethodNames::stringOperation,
              ValueFamilies::Strings,
              "enum" } } },

        { EventName::Enum::inArray,
          { { { { Resources::ArrayOperations::In, Labels::In }, { Resources::ArrayOperations::NotIn, Labels::NotIn } },
              MethodNames::inArray,
              ValueFamilies::Array,
              "enum" } } },

        { EventName::Enum::compareValues,
          {
              { { { ValueComparators::Less, Labels::Less },
                  { ValueComparators::Greater, Labels::Greater },
                  { ValueComparators::LessEqual, Labels::LessEqual },
                  { ValueComparators::GreaterEqual, Labels::GreaterEqual },
                  { ValueComparators::Equal, Labels::Equal },
                  { ValueComparators::NotEqual, Labels::NotEqual } },
                MethodNames::compareValues,
                ValueFamilies::ValuesWithLess,
                "enum" },

              { { { ValueComparators::Equal, Labels::Equal }, { ValueComparators::NotEqual, Labels::NotEqual } },
                MethodNames::compareValues,
                ValueFamilies::ValuesWithoutLess,
                "enum" },
          } }

    };
    return map;
}

const std::map< SceneEngine::Model::EventName::Enum, std::vector< MethodInfo > >& OperatorsMapper::methodInfoMap() {
    using namespace Resources::DataSources;
    static std::map< SceneEngine::Model::EventName::Enum, std::vector< MethodInfo > > map = {
        { EventName::Enum::isItemState,
          { { 0, "", { DeviceItem, Item, DeviceGroup, ItemGroup } }, { 1, "", { Constant, Expression } } } },
        { EventName::Enum::isDeviceState, { { 0, "", { Device, DeviceGroup } }, { 1, "", { Constant } } } },
        { EventName::Enum::isItemStateChanged,
          { { 0, "", { Item, Expression } },
            { 1, "start", { Constant, Expression } },
            { 2, "finish", { Constant, Expression } } } },

        { EventName::Enum::compareNumbers,
          { { 0, "", { Item, Expression, DeviceGroup, ItemGroup } }, { 1, "", { Constant, Item, Expression } } } },
        { EventName::Enum::compareNumberRange,
          { { 0, "", { Item, Expression } }, { 1, "startValue", { Constant } }, { 2, "endValue", { Constant } } } },
        { EventName::Enum::compareStrings,
          { { 0, "", { Item, Expression, DeviceGroup, ItemGroup } }, { 1, "", { Constant, Item, Expression } } } },
        { EventName::Enum::stringOperation,
          { { 0, "", { Item, Expression } }, { 1, "", { Constant, Item, Expression } } } },
        { EventName::Enum::compareValues,
          { { 0, "", { Item, Expression, DeviceGroup, ItemGroup } }, { 1, "", { Constant, Item, Expression } } } },

        { EventName::Enum::inArray, { { 0, "", { Item, Expression } }, { 1, "", { Constant } } } }
    };

    return map;
}

const std::map< std::string, SceneEngine::Model::EventName::Enum >& OperatorsMapper::enumFromMethodNameMap() {
    namespace Methods = SceneEngine::Model::Resources::MethodNames;
    static std::map< std::string, SceneEngine::Model::EventName::Enum > res = {
        { Methods::isBatteryLevel, EventName::Enum::isBatteryLevel },
        { Methods::isBatteryState, EventName::Enum::isBatteryState },
        { Methods::isButtonState, EventName::Enum::isButtonState },
        { Methods::isCloudState, EventName::Enum::isCloudState },
        { Methods::compareNumberRange, EventName::Enum::compareNumberRange },
        { Methods::compareNumbers, EventName::Enum::compareNumbers },
        { Methods::compareStrings, EventName::Enum::compareStrings },
        { Methods::inArray, EventName::Enum::inArray },
        { Methods::stringOperation, EventName::Enum::stringOperation },
        { Methods::compareValues, EventName::Enum::compareValues },
        { Methods::isDate, EventName::Enum::isDate },
        { Methods::isDateRange, EventName::Enum::isDateRange },
        { Methods::isDictionaryChanged, EventName::Enum::isDictionaryChanged },
        { Methods::isFirmwareUpdateState, EventName::Enum::isFirmwareUpdateState },
        { Methods::isGroupState, EventName::Enum::isGroupState },
        { Methods::isHouseModeChanged, EventName::Enum::isHouseModeChanged },
        { Methods::isItemState, EventName::Enum::isItemState },
        { Methods::isItemStateChanged, EventName::Enum::isItemStateChanged },
        { Methods::isNetworkState, EventName::Enum::isNetworkState },
        { Methods::isOnce, EventName::Enum::isOnce },
        { Methods::isSceneState, EventName::Enum::isSceneState },
        { Methods::isSunState, EventName::Enum::isSunState },
        { Methods::isUserLockOperation, EventName::Enum::isUserLockOperation },
        { Methods::hasAtLeastOneDictionaryValue, EventName::Enum::hasAtLeastOneDictionaryValue },
    };

    return res;
}

SceneEngine::Model::EventName::Enum OperatorsMapper::getEnumFromMethodName( const std::string& methodName ) {
    auto mapper = enumFromMethodNameMap();
    auto it = mapper.find( methodName );
    if( it != mapper.end() )
        return it->second;
    return SceneEngine::Model::EventName::Enum::None;
}

std::string OperatorsMapper::getMethodNameFromEnum( const SceneEngine::Model::EventName::Enum enumValue ) {
    auto mapper = enumFromMethodNameMap();
    for( const auto& item : mapper ) {
        if( item.second == enumValue ) {
            return item.first;
        }
    }
    return "";
}

std::map< std::string, std::vector< std::string > > OperatorsMapper::valueTypeFamilies() {
    using namespace Model::Resources;
    std::map< std::string, std::vector< std::string > > families = {
        { ValueFamilies::Numeric, { "int", "float", "scalableValueTypes" } },
        { ValueFamilies::Strings, { "string", "token" } },
        { ValueFamilies::ValuesWithLess, { "int", "float", "scalableValueTypes", "string" } },
        // TODO: EFWR-3337: fill value types list
        { ValueFamilies::ValuesWithoutLess, { "otherTypes??" } }
    };

    return families;
}

}  // namespace Model
}  // namespace SceneEngine
