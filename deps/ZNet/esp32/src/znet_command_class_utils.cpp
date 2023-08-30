#include "znet/znet_command_class_utils.hpp"
#include "znet/znet_all_command_classes.hpp"

#include "znet/znet_utils.hpp"

extern "C" {
#include "znet_log.h"
}

#include <array>

namespace znet {

namespace {

struct IDGetter {
    template <typename Class>
    znet_command_class_t operator()( const Class& ) const {
        return Class::ID::value;
    }
};

template <typename Class>
ClassPtr createCommandClassImpl( ZChannel& parent, znet_command_class_version_t version ) {
    return std::make_shared<Class>( parent, version );
}

struct __attribute__( ( packed ) ) CommandClassInitialData {
    znet_command_class_t id;
    ClassPtr ( *creator )( ZChannel& parent, znet_command_class_version_t version );
};

template <typename T>
struct CommandClassInitialDataGetter;

template <typename... Classes>
struct CommandClassInitialDataGetter<visitors::BaseAcceptor<Classes...> > {
    static_assert( utils::AreTypesUnique<typename Classes::ID...>::value, "Class Ids must be unique for each cc" );

    const CommandClassInitialData* operator()( znet_command_class_t id ) const {
        static const auto sortedCcData = []() {
            std::array<CommandClassInitialData, sizeof...( Classes )> result{ { CommandClassInitialData{
                Classes::ID::value, &createCommandClassImpl<Classes> }... } };
            std::sort(
                result.begin(), result.end(), []( const CommandClassInitialData& a, const CommandClassInitialData& b ) {
                    return a.id < b.id;
                } );
            return result;
        }();

        const auto found = std::lower_bound(
            sortedCcData.begin(),
            sortedCcData.end(),
            id,
            []( const CommandClassInitialData& data, znet_command_class_t id ) { return data.id < id; } );

        return found != sortedCcData.end() && found->id == id ? &*found : nullptr;
    }
};

}  // namespace

znet_command_class_t getCommandClassId( const ZClass& zclass ) {
    return visitors::visit( IDGetter(), zclass );
}

ClassPtr createCommandClass( znet_command_class_t id, ZChannel& parent, znet_command_class_version_t version ) {
    const auto ccInitialData = CommandClassInitialDataGetter<ZClassAcceptor>()( id );

    if( !ccInitialData ) {
        ZNET_LOGD( "Command class 0x%x is not supported\n", id );
        return {};
    }

    return ccInitialData->creator( parent, version );
}

bool canCreateCommandClass( znet_command_class_t id ) {
    const auto ccInitialData = CommandClassInitialDataGetter<ZClassAcceptor>()( id );
    return ccInitialData != nullptr;
}

}  // namespace znet
