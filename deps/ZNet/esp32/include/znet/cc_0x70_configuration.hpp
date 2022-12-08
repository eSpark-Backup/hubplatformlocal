#pragma once

#include <znet/znet_command_class.hpp>
#include <znet/znet_request_status.hpp>

#include <map>

namespace znet {

enum class ConfigurationFormat : uint8_t;

class ZCc0x70Configuration final : public ZClass {
public:
    using ID = ZClassID<ZNET_COMMAND_CLASS_CONFIGURATION>;
    using ZClass::ZClass;

    struct Parameter {
        uint16_t m_id;

        // Properties
        ConfigurationFormat m_format;
        uint8_t m_size;
        bool m_readOnly;
        uint32_t m_minValue;
        uint32_t m_maxValue;
        uint32_t m_defaultValue;

        std::string m_name;
        std::string m_info;
    };

    using ParameterPtr = std::shared_ptr<Parameter>;

    bool interview( InterviewCallback callback, WeakJobContext context ) override;

    bool storeData( IFile& file ) const override;
    bool restoreData( IFile& file, int version ) override;

    virtual bool requestValue( const RequestValue& request ) override;

    bool addConfiguration( uint8_t parameterNumber, uint8_t configSize );
    bool removeConfiguration( uint8_t parameterNumber );

    RequestStatus::Enum setConfigurationValue( uint16_t configParamNum,
                                               uint8_t configSize,
                                               znet_cmd_configuration_value_t configValue );

    RequestStatus::Enum resetConfigurationValue( uint16_t configParamNum );

    RequestStatus::Enum resetAllConfigurationValues();

    bool requestParameterValue( uint16_t parameterNumber ) const;

    void forEachV3Parameter( std::function<void( const Parameter& )> callback ) const;

    const Parameter* findV3Parameter( uint16_t parameterNumber ) const;

    ZNET_DEFINE_ACCEPT()

private:
    bool requestNextV1Parameter( std::map<uint8_t, uint8_t>::const_iterator it,
                                 WeakJobContext jobContext,
                                 RequestValue::CallbackPtr cb );

    bool requestNextV3Parameter( size_t index, WeakJobContext jobContext, RequestValue::CallbackPtr cb );

    // V3 interview
    void interviewNextProperty( WeakJobContext jobContext, RequestValue::CallbackPtr cb, uint16_t id );

    void requestParameterName( WeakJobContext jobContext,
                               RequestValue::CallbackPtr cb,
                               ParameterPtr parameter,
                               utils::optional<uint8_t> reportsToWait = {} );

    void requestParameterInfo( WeakJobContext jobContext,
                               RequestValue::CallbackPtr cb,
                               ParameterPtr parameter,
                               utils::optional<uint8_t> reportsToWait = {} );

    void requestParameterProperties( WeakJobContext jobContext, RequestValue::CallbackPtr cb, ParameterPtr parameter );

    bool isLowMemoryDevice() const;

    Parameter createDefaultParameter( uint16_t id ) const;

private:
    std::map<uint8_t, uint8_t> m_v1Parameters;

    std::vector<Parameter> m_v3Parameters;
};

}  // namespace znet
