#include "../moc/cc/Configuration.h"
#include "MainFixture.h"
#include "gtest/gtest.h"

static const char* logtag = "znet_configuration";
#define LOG_LOCAL_LEVEL P_LOG_DEBUG

TEST_F( MainFixture, znet_configuration ) {
    static uint8_t deViceId = 3;
    static uint8_t channelId = 0;
    std::shared_ptr<Device> dev0 = std::make_shared<Device>( 0xd1ac1b2d, deViceId );
    m_hub->deviceAdd( dev0 );

    /* Add command classes */
    std::shared_ptr<Configuration> conf = std::make_shared<Configuration>();
    using param_t = struct {
        uint8_t addr;
        Configuration::Type type;
        std::uint32_t value;
    };
    std::array<param_t, 5> parameters = { {
        { 1, Configuration::Type::u8, 0x12 },
        { 2, Configuration::Type::u16, 0x3456 },
        { 3, Configuration::Type::u32, 0x87654321 },
        { 4, Configuration::Type::u32, 0x00000000 },
        { 5, Configuration::Type::u32, 0xFFFFFFFF },
    } };
    for( auto p : parameters ) {
        conf->parameterAdd( p.addr, p.type, p.value );
    }
    dev0->addCC( { conf } );

    /* znet_init */
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }

    /* znet_node_add */
    P_LOGI( logtag, "Test znet_node_add" );
    static bool nodeAddResultEnter = false;
    dev0->inclusionSetReady();
    m_callbacks.node_add_result = []( int err, const znet_nodeinfo_t* node_info, void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        nodeAddResultEnter = true;
        ASSERT_EQ( err, 0 ) << "Add node fail!\n";
        ASSERT_EQ( node_info->node_id, deViceId );
        printf( "Add node succ! Node ID: %hhu\n", node_info->node_id );
        MainFixture::znPrintNodeInfo( node_info );
    };
    ASSERT_EQ( znet_node_add(), ZNET_ERR_OK );
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( nodeAddResultEnter );

    /* znet_node_cmd_configuration_set */
    for( auto p : parameters ) {
        P_LOGI( logtag, "Test znet_node_cmd_configuration_set" );
        ASSERT_EQ( znet_node_cmd_configuration_set( deViceId, channelId, p.addr, p.type, 0, p.value ), ZNET_ERR_OK );
        for( int i = 0; i < m_procIteration; ++i ) {
            P_LOGV( logtag, "call znet_proc(): %d", i );
            znet_proc();
        }
    }

    /* znet_node_cmd_configuration_get */
    for( auto p : parameters ) {
        P_LOGI( logtag, "Test znet_node_cmd_configuration_get" );
        static bool configurationGetResultEnter = false;
        static param_t parameter;
        parameter = p;
        m_callbacks.node_cmd_configuration_result = []( int err,
                                                        znet_node_id_t node_id,
                                                        znet_node_channel_id_t channel_id,
                                                        uint8_t parameter_number,
                                                        const znet_configuration_report_t* value,
                                                        void* arg ) -> void {
            if( err == ZNET_ERR_REQUEST_SENT_OK )
                return;

            configurationGetResultEnter = true;

            ASSERT_EQ( err, 0 ) << "node_cmd_configuration_result fail!\n";
            ASSERT_EQ( node_id, deViceId );
            ASSERT_EQ( channel_id, channelId );
            ASSERT_NE( value, nullptr );
            ASSERT_EQ( value->param_number, parameter.addr );
            ASSERT_EQ( value->data_count, parameter.type );
            ASSERT_EQ( value->value, parameter.value );

            P_LOGI( logtag,
                    "node_cmd_configuration_result: \n\t"
                    "err: %i, \n\t"
                    "node_id: 0x%02X, \n\t"
                    "channel_id: 0x%02X, \n\t"
                    "value:\n\t\t"
                    "param_number: %u\n\t\t"
                    "data_count: %u\n\t\t"
                    "value: 0x%X\n\t"
                    "arg: %p\n\t",
                    err,
                    node_id,
                    channel_id,
                    value->param_number,
                    value->data_count,
                    value->value,
                    arg );
        };

        ASSERT_EQ( znet_node_cmd_configuration_get( deViceId, channelId, parameter.addr ), ZNET_ERR_OK );
        for( int i = 0; i < m_procIteration; ++i ) {
            P_LOGV( logtag, "call znet_proc(): %d", i );
            znet_proc();
        }
        ASSERT_TRUE( configurationGetResultEnter );
    }
}

TEST_F( MainFixture, znet_configuration_name_get ) {
    static uint8_t deViceId = 3;
    static uint8_t channelId = 0;
    std::shared_ptr<Device> dev0 = std::make_shared<Device>( 0xd1ac1b2d, deViceId );
    m_hub->deviceAdd( dev0 );

    std::shared_ptr<Configuration> conf = std::make_shared<Configuration>();
    // add test parameters
    conf->v3ParameterAdd(
        1, 1, Configuration::Type::u8, 0, 255, 100, 100, "Test parameter 1 name.", "Test parameter 1 info" );
    conf->v3ParameterAdd(
        260, 2, Configuration::Type::u16, 0, 255, 100, 100, "Test parameter 2 name..", "Test parameter 3 info" );
    conf->v3ParameterAdd(
        1000, 4, Configuration::Type::u32, 0, 255, 100, 100, "Test parameter 3 name...", "Test parameter 3 info" );
    dev0->addCC( { conf } );

    /* znet_init */
    P_LOGI( logtag, "Test znet_init" );
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }

    /* znet_node_add */
    P_LOGI( logtag, "Test znet_node_add" );
    static bool nodeAddResultEnter = false;
    dev0->inclusionSetReady();
    m_callbacks.node_add_result = []( int err, const znet_nodeinfo_t* node_info, void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        nodeAddResultEnter = true;
        ASSERT_EQ( err, 0 ) << "Add node fail!\n";
        ASSERT_EQ( node_info->node_id, deViceId );
        printf( "Add node succ! Node ID: %hhu\n", node_info->node_id );
        MainFixture::znPrintNodeInfo( node_info );
    };
    ASSERT_EQ( znet_node_add(), ZNET_ERR_OK );
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( nodeAddResultEnter );

    conf->forEachV3ParameterName( [this]( uint16_t id, const std::string& name ) {
        static bool configurationGetResultEnterS = false;
        static uint16_t paramIdS;
        paramIdS = id;
        static std::string paramNameS;
        paramNameS = name;

        m_callbacks.node_cmd_configuration_name_result = []( znet_error_t err,
                                                             znet_node_id_t node_id,
                                                             znet_node_channel_id_t channel_id,
                                                             znet_cmd_configuration_id_t param_number,
                                                             const znet_configuration_name_report_t* value,
                                                             void* arg ) -> void {
            if( err == ZNET_ERR_REQUEST_SENT_OK )
                return;

            configurationGetResultEnterS = true;

            ASSERT_EQ( err, 0 ) << "node_cmd_configuration_name_result fail!\n";
            ASSERT_EQ( node_id, deViceId );
            ASSERT_EQ( channel_id, channelId );
            ASSERT_EQ( param_number, paramIdS );
            ASSERT_NE( value, nullptr );
            ASSERT_EQ( value->param_number, paramIdS );
            ASSERT_EQ( value->rep_to_follows, 0 );
            ASSERT_EQ( value->name_length, paramNameS.size() );

            for( int i = 0; i < paramNameS.size(); ++i ) {
                ASSERT_EQ( value->data[i], paramNameS[i] );
            }
        };

        ASSERT_EQ( znet_node_cmd_configuration_name_get( deViceId, channelId, id, true ), ZNET_ERR_OK );
        for( int i = 0; i < m_procIteration; ++i ) {
            P_LOGV( logtag, "call znet_proc(): %d", i );
            znet_proc();
        }
        ASSERT_TRUE( configurationGetResultEnterS );
    } );
}

TEST_F( MainFixture, znet_configuration_info_get ) {
    static uint8_t deViceId = 3;
    static uint8_t channelId = 0;
    std::shared_ptr<Device> dev0 = std::make_shared<Device>( 0xd1ac1b2d, deViceId );
    m_hub->deviceAdd( dev0 );

    std::shared_ptr<Configuration> conf = std::make_shared<Configuration>();
    // add test parameters
    conf->v3ParameterAdd(
        1, 1, Configuration::Type::u8, 0, 255, 100, 100, "Test parameter 1 name.", "Test parameter 1 info." );
    conf->v3ParameterAdd(
        260, 2, Configuration::Type::u16, 0, 255, 100, 100, "Test parameter 2 name..", "Test parameter 2 info.." );
    conf->v3ParameterAdd(
        1000, 4, Configuration::Type::u32, 0, 255, 100, 100, "Test parameter 3 name...", "Test parameter 3 info..." );
    dev0->addCC( { conf } );

    /* znet_init */
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }

    /* znet_node_add */
    P_LOGI( logtag, "Test znet_node_add" );
    static bool nodeAddResultEnter = false;
    dev0->inclusionSetReady();
    m_callbacks.node_add_result = []( int err, const znet_nodeinfo_t* node_info, void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        nodeAddResultEnter = true;
        ASSERT_EQ( err, 0 ) << "Add node fail!\n";
        ASSERT_EQ( node_info->node_id, deViceId );
        printf( "Add node succ! Node ID: %hhu\n", node_info->node_id );
        MainFixture::znPrintNodeInfo( node_info );
    };
    ASSERT_EQ( znet_node_add(), ZNET_ERR_OK );
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( nodeAddResultEnter );

    conf->forEachV3ParameterInfo( [this]( uint16_t id, const std::string& info ) {
        static bool configurationGetResultEnterS = false;
        static uint16_t paramIdS;
        paramIdS = id;
        static std::string paramInfoS;
        paramInfoS = info;

        m_callbacks.node_cmd_configuration_info_result = []( znet_error_t err,
                                                             znet_node_id_t node_id,
                                                             znet_node_channel_id_t channel_id,
                                                             znet_cmd_configuration_id_t param_number,
                                                             const znet_configuration_info_report_t* value,
                                                             void* arg ) -> void {
            if( err == ZNET_ERR_REQUEST_SENT_OK )
                return;

            configurationGetResultEnterS = true;

            ASSERT_EQ( err, 0 ) << "node_cmd_configuration_info_result fail!\n";
            ASSERT_EQ( node_id, deViceId );
            ASSERT_EQ( channel_id, channelId );
            ASSERT_EQ( param_number, paramIdS );
            ASSERT_NE( value, nullptr );
            ASSERT_EQ( value->param_number, paramIdS );
            ASSERT_EQ( value->rep_to_follows, 0 );
            ASSERT_EQ( value->info_length, paramInfoS.size() );

            for( int i = 0; i < paramInfoS.size(); ++i ) {
                ASSERT_EQ( value->data[i], paramInfoS[i] );
            }
        };

        ASSERT_EQ( znet_node_cmd_configuration_info_get( deViceId, channelId, id, true ), ZNET_ERR_OK );
        for( int i = 0; i < m_procIteration; ++i ) {
            P_LOGV( logtag, "call znet_proc(): %d", i );
            znet_proc();
        }
        ASSERT_TRUE( configurationGetResultEnterS );
    } );
}

TEST_F( MainFixture, znet_configuration_properties_get ) {
    static uint8_t deViceId = 3;
    static uint8_t channelId = 0;
    std::shared_ptr<Device> dev0 = std::make_shared<Device>( 0xd1ac1b2d, deViceId );
    m_hub->deviceAdd( dev0 );

    std::shared_ptr<Configuration> conf = std::make_shared<Configuration>();
    // add test parameters
    conf->v3ParameterAdd(
        1, 1, Configuration::Type::u8, 0, 255, 100, 100, "Test parameter 1 name.", "Test parameter 1 info." );
    conf->v3ParameterAdd(
        260, 2, Configuration::Type::u16, 0, 43690, 100, 43000, "Test parameter 2 name..", "Test parameter 2 info.." );
    conf->v3ParameterAdd( 1000,
                          4,
                          Configuration::Type::u32,
                          0,
                          2863311530,
                          43690,
                          100,
                          "Test parameter 3 name...",
                          "Test parameter 3 info..." );
    dev0->addCC( { conf } );

    /* znet_init */
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }

    /* znet_node_add */
    P_LOGI( logtag, "Test znet_node_add" );
    static bool nodeAddResultEnter = false;
    dev0->inclusionSetReady();
    m_callbacks.node_add_result = []( int err, const znet_nodeinfo_t* node_info, void* arg ) -> void {
        if( err == ZNET_ERR_REQUEST_SENT_OK )
            return;

        nodeAddResultEnter = true;
        ASSERT_EQ( err, 0 ) << "Add node fail!\n";
        ASSERT_EQ( node_info->node_id, deViceId );
        printf( "Add node succ! Node ID: %hhu\n", node_info->node_id );
        MainFixture::znPrintNodeInfo( node_info );
    };
    ASSERT_EQ( znet_node_add(), ZNET_ERR_OK );
    for( int i = 0; i < m_procIteration; ++i ) {
        P_LOGV( logtag, "call znet_proc(): %d", i );
        znet_proc();
    }
    ASSERT_TRUE( nodeAddResultEnter );

    conf->forEachV3ParameterProperties( [this]( uint16_t id,
                                                uint8_t format,
                                                Configuration::Type type,
                                                uint32_t minValue,
                                                uint32_t maxValue,
                                                uint32_t defaultValue ) {
        static bool configurationGetResultEnterS;
        static uint16_t paramIdS;
        static uint8_t formatS;
        static Configuration::Type typeS;
        static uint32_t minValueS;
        static uint32_t maxValueS;
        static uint32_t defaultValueS;

        configurationGetResultEnterS = false;
        paramIdS = id;
        formatS = format;
        typeS = type;
        minValueS = minValue;
        maxValueS = maxValue;
        defaultValueS = defaultValue;

        m_callbacks.node_cmd_configuration_properties_result = []( znet_error_t err,
                                                                   znet_node_id_t node_id,
                                                                   znet_node_channel_id_t channel_id,
                                                                   znet_cmd_configuration_id_t param_number,
                                                                   const znet_configuration_properties_report_t* value,
                                                                   void* arg ) -> void {
            if( err == ZNET_ERR_REQUEST_SENT_OK )
                return;

            configurationGetResultEnterS = true;

            ASSERT_EQ( err, 0 ) << "node_cmd_configuration_properties_result fail!\n";
            ASSERT_EQ( node_id, deViceId );
            ASSERT_EQ( channel_id, channelId );
            ASSERT_EQ( param_number, paramIdS );
            ASSERT_NE( value, nullptr );
            ASSERT_EQ( value->param_number, paramIdS );
            ASSERT_EQ( value->param_format, formatS );
            ASSERT_EQ( value->param_size, typeS );
            ASSERT_EQ( value->param_min_value, minValueS );
            ASSERT_EQ( value->param_max_value, maxValueS );
            ASSERT_EQ( value->param_default_value, defaultValueS );
            ASSERT_EQ( value->next_param_number, 0 );
        };

        ASSERT_EQ( znet_node_cmd_configuration_properties_get( deViceId, channelId, id ), ZNET_ERR_OK );
        for( int i = 0; i < m_procIteration; ++i ) {
            P_LOGV( logtag, "call znet_proc(): %d", i );
            znet_proc();
        }
        ASSERT_TRUE( configurationGetResultEnterS );
    } );
}
