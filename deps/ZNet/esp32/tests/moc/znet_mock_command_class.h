#pragma once

#include <znet/znet_command_class.hpp>
#include <gmock/gmock.h>

namespace znet {
namespace ut {

namespace details {
template <typename CommandClass>
class MockCommandClassBase : public CommandClass {
public:
    using CommandClass::CommandClass;

    bool requestValue( const RequestValue& request ) override {
        const auto result = CommandClass::requestValue( request );

        onValueRequested();

        return result;
    }

protected:
    virtual void onValueRequested() = 0;
};

}  // namespace details

template <typename CommandClass>
class MockCommandClass : public details::MockCommandClassBase<CommandClass> {
public:
    using Base = details::MockCommandClassBase<CommandClass>;
    using Base::Base;

    MOCK_METHOD0( onValueRequested, void() );

    MOCK_METHOD1( setToken, bool( const std::string& value ) );

    MOCK_METHOD1( setValue, bool( uint8_t value ) );

    MOCK_METHOD1( setRgb, bool( ColorComponents& components ) );

    MOCK_METHOD4( setTokenMulticast, bool( const uint8_t, const znet_node_id_t*, const std::string& value, bool ) );

    MOCK_METHOD4( setValueMulticast, bool( const uint8_t, const znet_node_id_t*, const uint8_t value, bool ) );

    MOCK_METHOD4( setRgbMulticast, bool( const uint8_t, const znet_node_id_t*, ColorComponents&, bool ) );
};

}  // namespace ut
}  // namespace znet
