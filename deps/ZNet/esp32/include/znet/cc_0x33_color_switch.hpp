#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/znet_color_switch.hpp>
#include <znet/enums/value_change_command.hpp>

#include <memory>

namespace znet {

class ZCc0x33ColorSwitch : public ZClass {
public:
    enum class Status : uint8_t { Idle, Polling };

    using RequestCallbackPtr = std::shared_ptr<RequestValue::Callback>;

public:
    class ComponentsCache {
    public:
        ComponentsCache( ZCc0x33ColorSwitch& relatedCC );

        void addComponent( const ColorComponentInfo& component );

        bool isCompleted() const;

        bool hasComponent( ColorComponentID componentId ) const;

        ColorComponentInfos takeComponents();

    private:
        ZCc0x33ColorSwitch& m_relatedCC;
        ColorComponentInfos m_components;
    };

    using ID = ZClassID<ZNET_COMMAND_CLASS_SWITCH_COLOR>;

    ZCc0x33ColorSwitch( ZChannel& parent, znet_command_class_version_t version = 1 );

    bool requestValue( const RequestValue& request ) override;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool isComponentSupported( ColorComponentID componentId ) const;

    bool setLevelChange( ColorComponentID componentId, ValueChangeCommand command, const SecondsOpt& duration = {} );

    bool startLevelChange( ColorComponentID componentId,
                           bool downDirection,
                           utils::optional<uint8_t> startLevel = {},
                           const SecondsOpt& duration = {} );

    bool stopLevelChange( ColorComponentID componentId );

    std::vector<ColorComponentID> getSupportedColors() const;

    ComponentsCache& getUpdatedComponentsCache();

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    ZNET_DEFINE_ACCEPT()

protected:
    bool setRgb( ColorComponents& components ) override;

    bool setRgbMulticast( const uint8_t nodeQuantity,
                          const znet_node_id_t* listNodeId,
                          ColorComponents& components,
                          bool sendSingleCasts ) override;

private:
    bool requestNextComponent( RequestCallbackPtr callback, WeakJobContext jobContext, uint8_t componentId = 0 );

private:
    ComponentsCache m_componentsCache;

    uint16_t m_supportedComponentsMask = 0;
    Status m_status = Status::Idle;
};

}  // namespace znet
