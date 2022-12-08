#pragma once

#include <znet/znet_errors.h>

namespace znet {
namespace RequestStatus {

enum Enum { Sent, Pending, Failed };

inline Enum fromStatus( znet_error_t status ) {
    switch( status ) {
    case ZNET_ERR_OK:
    case ZNET_ERR_REQUEST_SENT_OK:
        return Enum::Sent;

    case ZNET_ERR_INPROGR:
        return Enum::Pending;

    default:
        return Enum::Failed;
    }
}

inline const char* toString( const znet::RequestStatus::Enum status ) {
    switch( status ) {
    case znet::RequestStatus::Sent:
        return "synced";

    case znet::RequestStatus::Pending:
        return "pending";

    case znet::RequestStatus::Failed:
        return "failed";

    default:
        return "unknown";
    }
}

}  // namespace RequestStatus
}  // namespace znet
