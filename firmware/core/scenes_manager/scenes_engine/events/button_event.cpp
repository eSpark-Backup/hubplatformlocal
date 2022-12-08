#include "scenes_manager/scenes_engine/events/button_event.h"
#include "scenes_manager/scenes_engine/model/model_resources.h"
#include "cored/model/resources/cored_resources.h"
#include "json_utils/json_utils.h"

namespace SceneEngine {

ButtonStateEvent::ButtonStateEvent( const ItemID& itemId, const ButtonState& state )
: m_itemId( itemId )
, m_buttonState( state ) {}

ButtonStateEvent::ButtonStateEvent( ButtonStateEvent::ItemID&& itemId, ButtonStateEvent::ButtonState&& state )
: m_itemId( std::move( itemId ) )
, m_buttonState( std::move( state ) ) {}

std::string ButtonStateEvent::getMethodName() const {
    return Model::Resources::MethodNames::isButtonState;
}

bool ButtonStateEvent::compare( const ItemID& itemId, const ItemValue& value ) const {
    return compare( itemId ) && compare( value );
}

bool ButtonStateEvent::compare( const ItemID& itemId ) const {
    return m_itemId == itemId;
}

bool ButtonStateEvent::compare( const ItemValue& value ) const {
    using namespace HubPlatform::CoreD::Model::Resources;
    const auto state = json_get_str( value.getValue(), ButtonStateValueFields::ButtonStateFieldName );
    return m_buttonState == state;
}

const ButtonStateEvent::ItemID& ButtonStateEvent::getItemId() const {
    return m_itemId;
}

}  // namespace SceneEngine
