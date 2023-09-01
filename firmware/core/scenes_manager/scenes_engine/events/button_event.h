#pragma once

#include "scenes_manager/scenes_engine/events/item_event_base.h"

namespace SceneEngine {

class ButtonStateEvent : public ItemEventBase {
public:
    using ItemID = std::string;
    using ItemEventBase::ItemValue;
    using ButtonState = std::string;

    ButtonStateEvent( const ItemID& itemId, const ButtonState& state );
    ButtonStateEvent( ItemID&& itemId, ButtonState&& state );

    std::string getMethodName() const override;

    bool compare( const ItemID& itemId, const ItemValue& itemValue ) const;
    bool compare( const ItemID& itemId ) const;
    bool compare( const ItemValue& value ) const override;

    const ItemID& getItemId() const;

private:
    ItemID m_itemId;
    ButtonState m_buttonState;
};

using ButtonStateEventPtr = std::shared_ptr< ButtonStateEvent >;

}  // namespace SceneEngine
