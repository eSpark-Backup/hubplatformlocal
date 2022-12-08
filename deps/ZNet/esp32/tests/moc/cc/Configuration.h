#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <map>
#include <memory>
#include <functional>
#include <string>

#include "../cc/CommanClass.h"

class Configuration : public CommandClass {
public:
    enum Type { u8 = 1, u16 = 2, u32 = 4 };

public:
    Configuration();
    virtual ~Configuration() = default;
    bool run( std::vector<std::vector<uint8_t>>& rev,
              std::vector<uint8_t>& ret,
              std::vector<uint8_t> const& data ) override;
    void parameterAdd( uint8_t parameterNumber, Configuration::Type type, std::uint32_t value );
    void parameterSet( uint8_t parameterNumber, std::uint32_t value );
    std::uint32_t parameterGet( uint8_t parameterNumber );
    void parameterReset( uint8_t parameterNumber );
    uint8_t parameterGetSize( uint8_t parameterNumber );

    void v3ParameterAdd( uint16_t id,
                         uint8_t format,
                         Type size,
                         uint32_t minValue,
                         uint32_t maxValue,
                         uint32_t defaultValue,
                         uint32_t value,
                         const std::string& name,
                         const std::string& info );

    void forEachV3ParameterName( std::function<void( uint16_t, const std::string& )> callback ) const;
    void forEachV3ParameterInfo( std::function<void( uint16_t, const std::string& )> callback ) const;
    void forEachV3ParameterProperties(
        std::function<void( uint16_t, uint8_t, Type, uint32_t, uint32_t, uint32_t )> callback ) const;

private:
    enum Command {
        Version = 0x01,
        Set = 0x04,
        Get = 0x05,
        Report = 0x06,
        BulkSet = 0x07,
        BulkGet = 0x08,
        BulkReport = 0x09,
        NameGet = 0x0A,
        NameReport = 0x0B,
        InfoGet = 0x0C,
        InfoReport = 0x0D,
        PropertiesGet = 0x0E,
        PropertiesReport = 0x0F
    };

private:
    bool setValue( std::vector<uint8_t> const& data );
    bool getValue( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data );

    bool getName( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data );
    bool getInfo( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data );
    bool getProperties( std::vector<uint8_t>& ret, std::vector<uint8_t> const& data );

    std::string getV3ParameterName( uint16_t id ) const;
    std::string getV3ParameterInfo( uint16_t id ) const;

private:
    struct Parameter {
        Type type;
        std::uint32_t value;
        std::uint32_t defvalue;
    };
    std::map<uint8_t, Parameter> m_parameters;

    struct V3Parameter {
        uint16_t m_id;

        // Properties
        uint8_t m_format;
        Type m_size;
        uint32_t m_minValue;
        uint32_t m_maxValue;
        uint32_t m_defaultValue;

        uint32_t m_value;

        std::string m_name;
        std::string m_info;
    };
    std::map<uint16_t, V3Parameter> m_v3Parameters;
};

#endif /* CONFIGURATION_H_ */
