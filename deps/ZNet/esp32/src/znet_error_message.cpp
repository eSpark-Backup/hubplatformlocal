#include "znet/znet_error_message.hpp"
#include "znet/converters/resources.hpp"

namespace res = ::znet::resources::err;

namespace znet {
namespace err {

znet_error_message_t message( znet_error_t code ) {
    return znet_error_message_t{ code, description( code ), {} };
}

znet_error_message_t message( znet_error_t code, RequestFailType failType ) {
    return znet_error_message_t{
        code,
        description( code ),
        { { res::FailType, std::to_string( static_cast<std::underlying_type<RequestFailType>::type>( failType ) ) } }
    };
}

znet_error_message_t message( znet_error_t code, znet_node_id_t nodeId ) {
    return znet_error_message_t{ code, description( code ), { { res::NodeId, std::to_string( nodeId ) } } };
}

znet_error_message_t interview::message( znet_error_t code, znet_node_id_t nodeId, InterviewStep step ) {
    return znet_error_message_t{
        code, description( code ), { { res::NodeId, std::to_string( nodeId ) }, { res::Step, std::to_string( step ) } }
    };
}

znet_error_message_t bkp::message( znet_error_t code, znet_backup_status_t status ) {
    return znet_error_message_t{ code, description( code ), { { res::Status, std::to_string( status ) } } };
}

znet_error_message_t bkp::message( znet_error_t code, uint16_t requestedOffset, uint16_t reportedOffset ) {
    return znet_error_message_t{ code,
                                 description( code ),
                                 { { res::ReportedOffset, std::to_string( reportedOffset ) },
                                   { res::RequestedOffset, std::to_string( requestedOffset ) } } };
}

znet_error_message_t values::values_getting_failed( znet_node_channel_id_t channel,
                                                    znet_command_class_t commandClass,
                                                    RequestFailType failType ) {
    return znet_error_message_t{
        ZNET_ERR_VALUES_GETTING,
        description( ZNET_ERR_VALUES_GETTING ),
        { { res::Channel, std::to_string( channel ) },
          { res::CommandClass, std::to_string( commandClass ) },
          { res::FailType, std::to_string( static_cast<std::underlying_type<RequestFailType>::type>( failType ) ) } }
    };
}

znet_error_message_t time::time_request_failed( znet_node_id_t nodeId,
                                                const char* requestName,
                                                RequestFailType failType ) {
    return znet_error_message_t{
        ZNET_ERR_TIME_REQ,
        description( ZNET_ERR_TIME_REQ ),
        { { res::NodeId, std::to_string( nodeId ) },
          { res::RequestName, requestName },
          { res::FailType, std::to_string( static_cast<std::underlying_type<RequestFailType>::type>( failType ) ) } }
    };
}

znet_error_message_t bkp::wrong_backup_read_status( znet_backup_status_t status, uint16_t offset, uint8_t dataLength ) {
    return znet_error_message_t{ ZNET_ERR_BKP_READ_STATUS,
                                 description( ZNET_ERR_BKP_READ_STATUS ),
                                 { { res::Status, std::to_string( status ) },
                                   { res::Offset, std::to_string( offset ) },
                                   { res::DataLength, std::to_string( dataLength ) } } };
}

znet_error_message_t bkp::wrong_backup_write_status( znet_backup_status_t status, uint16_t offset ) {
    return znet_error_message_t{ ZNET_ERR_BKP_WRITE_STATUS,
                                 description( ZNET_ERR_BKP_WRITE_STATUS ),
                                 { { res::Status, std::to_string( status ) },
                                   { res::Offset, std::to_string( offset ) } } };
}

znet_error_message_t bkp::backup_write_eof_expected( size_t readBytes, uint16_t offset ) {
    return znet_error_message_t{ ZNET_ERR_BKP_WRITE_EOF,
                                 description( ZNET_ERR_BKP_WRITE_EOF ),
                                 { { res::ReadBytes, std::to_string( readBytes ) },
                                   { res::Offset, std::to_string( offset ) } } };
}

const char* description( znet_error_t code ) {
    switch( code ) {
    case ZNET_ERR_REQUEST_SENT_OK:
        return "hub.gateway.zwave.err.sent.ok";
    case ZNET_ERR_OK:
        return "hub.gateway.zwave.err.ok";
    case ZNET_ERR_FAULT:
        return "hub.gateway.zwave.err.fault";
    case ZNET_ERR_INVAL:
        return "hub.gateway.zwave.err.inval";
    case ZNET_ERR_NOMEM:
        return "hub.gateway.zwave.err.nomem";
    case ZNET_ERR_TIME:
        return "hub.gateway.zwave.err.time";
    case ZNET_ERR_BUSY:
        return "hub.gateway.zwave.err.busy";
    case ZNET_ERR_COMM:
        return "hub.gateway.zwave.err.comm";
    case ZNET_ERR_UNREACH:
        return "hub.gateway.zwave.err.unreach";
    case ZNET_ERR_NOTFOUND:
        return "hub.gateway.zwave.err.notfound";
    case ZNET_ERR_INPROGR:
        return "hub.gateway.zwave.err.inprog";
    case ZNET_ERR_UNSUPPORT:
        return "hub.gateway.zwave.err.unsupport";
    case ZNET_ERR_NETWORK_BUSY:
        return "hub.gateway.zwave.err.netbusy";
    case ZNET_ERR_CALLBACK:
        return "hub.gateway.zwave.err.callback";
    case ZNET_ERR_INV_RES:
        return "hub.gateway.zwave.err.result";
    case ZNET_ERR_NOT_SIS:
        return "hub.gateway.zwave.err.notsis";
    case ZNET_ERR_REPORT_EXPIRED:
        return "hub.gateway.zwave.err.report.expired";
    case ZNET_ERR_VALUES_GETTING:
        return "hub.gateway.zwave.err.values.getting";
    case ZNET_ERR_MVALUES_GETTING:
        return "hub.gateway.zwave.err.mvalues.getting";
    case ZNET_ERR_DISCOVER_REQ:
        return "hub.gateway.zwave.err.discover.req";
    case ZNET_ERR_TIME_REQ:
        return "hub.gateway.zwave.err.time.req";
    case ZNET_ERR_NIF_REQ:
        return "hub.gateway.zwave.err.nif.req";
    case ZNET_ERR_NIF_TIME:
        return "hub.gateway.zwave.err.nif.time";
    case ZNET_ERR_NIF_INVAL:
        return "hub.gateway.zwave.err.nif.inval";
    case ZNET_ERR_NODE_EXISTS:
        return "hub.gateway.zwave.err.node.exists";
    case ZNET_ERR_NODE_NOT_FOUND:
        return "hub.gateway.zwave.err.node.notfound";
    case ZNET_ERR_INTERVIEW:
        return "hub.gateway.zwave.err.interview";
    case ZNET_ERR_SMART_SECURITY:
        return "hub.gateway.zwave.err.smart.security";
    case ZNET_ERR_BKP_OPEN:
        return "hub.gateway.zwave.err.bkp.open";
    case ZNET_ERR_BKP_OPEN_STATUS:
        return "hub.gateway.zwave.err.bkp.open.status";
    case ZNET_ERR_BKP_CLOSE:
        return "hub.gateway.zwave.err.bkp.close";
    case ZNET_ERR_BKP_CLOSE_STATUS:
        return "hub.gateway.zwave.err.bkp.close.status";
    case ZNET_ERR_BKP_CLOSE_REQ:
        return "hub.gateway.zwave.err.bkp.close.req";
    case ZNET_ERR_BKP_READ:
        return "hub.gateway.zwave.err.bkp.read";
    case ZNET_ERR_BKP_READ_STATUS:
        return "hub.gateway.zwave.err.bkp.read.status";
    case ZNET_ERR_BKP_READ_REQ:
        return "hub.gateway.zwave.err.bkp.read.req";
    case ZNET_ERR_BKP_WRITE:
        return "hub.gateway.zwave.err.bkp.write";
    case ZNET_ERR_BKP_WRITE_STATUS:
        return "hub.gateway.zwave.err.bkp.write.status";
    case ZNET_ERR_BKP_WRITE_REQ:
        return "hub.gateway.zwave.err.bkp.write.req";
    case ZNET_ERR_BKP_WRITE_EOF:
        return "hub.gateway.zwave.err.bkp.write.eof";
    case ZNET_ERR_BKP_RST_OPEN:
        return "hub.gateway.zwave.err.bkp.rst.open";
    case ZNET_ERR_BKP_RST_OPEN_STATUS:
        return "hub.gateway.zwave.err.bkp.rst.open.status";
    case ZNET_ERR_BKP_RST_SET_DEFAULT:
        return "hub.gateway.zwave.err.bkp.rst.setdefault";
    case ZNET_ERR_INCLUDE_PERM:
        return "hub.gateway.zwave.err.include.perm";
    default:
        return "hub.gateway.zwave.err.unknown";
    }
}

}  // namespace err
}  // namespace znet
