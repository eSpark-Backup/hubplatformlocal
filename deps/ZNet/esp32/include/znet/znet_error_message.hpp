#pragma once

#include <string>
#include <vector>

#include <znet/znet.h>
#include <znet/optional.hpp>
#include <znet/znet_errors.h>
#include <znet/znet_request_fail.hpp>

namespace znet {
namespace err {

typedef struct {
    znet_error_t m_code;
    std::string m_data;
    std::vector<std::pair<std::string, std::string>> m_details;
} znet_error_message_t;

znet_error_message_t message( znet_error_t );
znet_error_message_t message( znet_error_t, RequestFailType );
znet_error_message_t message( znet_error_t, znet_node_id_t );

const char* description( znet_error_t code );

namespace interview {
using InterviewStep = uint8_t;  // Corresponds to the underlying type of ::znet::InterviewHandlerBase::Step
znet_error_message_t message( znet_error_t, znet_node_id_t, InterviewStep );
}  // namespace interview

namespace values {
znet_error_message_t values_getting_failed( znet_node_channel_id_t, znet_command_class_t, RequestFailType );
}  // namespace values

namespace time {
znet_error_message_t time_request_failed( znet_node_id_t, const char* requestName, RequestFailType );
}  // namespace time

namespace bkp {
znet_error_message_t message( znet_error_t, znet_backup_status_t );
znet_error_message_t message( znet_error_t, uint16_t reportedOffset, uint16_t requestedOffset );
znet_error_message_t wrong_backup_read_status( znet_backup_status_t status, uint16_t offset, uint8_t dataLength );
znet_error_message_t wrong_backup_write_status( znet_backup_status_t status, uint16_t offset );
znet_error_message_t backup_write_eof_expected( size_t readBytes, uint16_t offset );
}  // namespace bkp

}  // namespace err
using ErrorOpt = utils::optional<err::znet_error_message_t>;
}  // namespace znet
