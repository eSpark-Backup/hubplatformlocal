#include "znet/znet_command_class.hpp"
#include "znet/cc_0x32_meter.hpp"
#include "znet/znet_meter.hpp"

#include "znet/znet_node.hpp"
#include "znet/znet_requests_manager.hpp"

extern "C" {
#include "znet_log.h"
}

namespace znet {

ZCc0x32Meter::ZCc0x32Meter( ZChannel& parent, znet_command_class_version_t version )
: ZClass( parent, version )
, m_status( Status::Idle ) {}

bool ZCc0x32Meter::resetMeter( uint8_t dataSize, const uint8_t* data ) {
    if( getVersion() < 6 ) {
        ZNET_LOGE( "cc_0x%x: meter reset with type is not supported for v%u: error: %i\n\n", getId(), getVersion() );
        return false;
    }

    znet_error_t result = znet_node_cmd_meter_reset_v6(
        getParentNode().getId(), getParentChannel().getId(), static_cast<uint8_t>( m_type ), dataSize, data );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: set v6: error: %i\n\n", getId(), result );
        return false;
    }
    return true;
}

bool ZCc0x32Meter::resetMeter() {
    znet_error_t result = znet_node_cmd_meter_reset( getParentNode().getId(), getParentChannel().getId() );
    if( ZNET_ERR_OK != result ) {
        ZNET_LOGE( "cc_0x%x: set: error: %i\n\n", getId(), result );
        return false;
    }
    return true;
}

bool ZCc0x32Meter::setValue( const uint8_t /*value*/ ) {
    return resetMeter();
}

bool ZCc0x32Meter::requestMeterComponent( uint8_t scale ) {
    if( !hasScale( scale ) ) {
        ZNET_LOGE( "cc_0x32: scale %u is not supported\n\n", scale );
        return false;
    }

    znet_error_t result =
        znet_node_cmd_meter_get( getParentNode().getId(), getParentChannel().getId(), scale, getVersion() );
    if( result != ZNET_ERR_OK ) {
        ZNET_LOGE( "cc_0x32: value request error: %i\n\n", result );
        return false;
    }

    return true;
}

bool ZCc0x32Meter::requestValue( const RequestValue& request ) {
    if( m_status == Status::Polling ) {
        ZNET_LOGW( "cc_0x32: cannot start request value, status is pollin\n" );
        return false;
    }

    m_status = Status::Polling;

    const auto result =
        !m_scales.empty() &&
        requestNextComponent(
            request.resultCallback ? std::make_shared<RequestValue::Callback>( request.resultCallback ) : nullptr,
            request.jobContext );

    if( !result ) {
        m_status = Status::Idle;
    }

    return result;
}

bool ZCc0x32Meter::requestNextComponent( RequestValue::CallbackPtr callbackPtr,
                                         WeakJobContext jobContext,
                                         uint8_t index ) {
    assert( m_status == Status::Polling );

    auto onFinished = [=]( RequestFailTypeOpt failType ) {
        m_status = Status::Idle;
        onRequestsFinished( callbackPtr, failType );
    };

    if( index >= m_scales.size() ) {
        onFinished( {} );
        return true;
    }

    return getParentNode().getRequestsManager().doRequest(
        MeterRequest( getNodeId(), getChannelId(), m_scales[index], getVersion() ),
        jobContext,
        [=]( const znet_meter_report_t& ) {
            if( !requestNextComponent( callbackPtr, jobContext, index + 1 ) ) {
                onFinished( RequestFailType::SendingError );
            }
        },
        onFinished );
}

bool ZCc0x32Meter::interview( InterviewCallback callback, WeakJobContext context ) {
    if( getVersion() > 1 ) {
        return singleInterviewRequestImpl(
            *this,
            MeterSupportedTypesRequest( getNodeId(), getChannelId() ),
            context,
            [this]( const znet_meter_supported_report_t& report ) {
                m_type = static_cast<meter::MeterType>( report.type );
                m_resetSupported = report.reset != 0;

                m_scales.reserve( utils::getBitsCount( report.scales_bitmask, report.scales_bitmask_size ) );
                utils::forEachBit( report.scales_bitmask, report.scales_bitmask_size, [this]( uint8_t index ) {
                    if(::znet::meter::isScaleSupported( m_type, index ) ) {
                        m_scales.push_back( index );
                    }
                } );
            },
            std::move( callback ) );
    }
    else {
        return singleInterviewRequestImpl( *this,
                                           MeterUnknownScaleRequest( getNodeId(), getChannelId() ),
                                           context,
                                           [this]( const znet_meter_report_t& report ) {
                                               m_type = static_cast<meter::MeterType>( report.type );
                                               if(::znet::meter::isScaleSupported( m_type, report.scale ) ) {
                                                   m_scales.push_back( report.scale );
                                               }
                                           },
                                           std::move( callback ) );
    }
}

bool ZCc0x32Meter::restoreData( IFile& file, int version ) {
    size_t count = 0;
    uint8_t param = 0;
    bool result = true;

    result = result && file.read( &param, sizeof( param ) );
    m_type = static_cast<meter::MeterType>( param );

    result = result && file.read( &count, sizeof( count ) );
    m_scales.reserve( count );
    for( size_t i = 0; i < count; ++i ) {
        result = result && file.read( &param, sizeof( param ) );
        m_scales.push_back( param );
    }

    result = result && file.read( &param, sizeof( param ) );
    setResetSupported( param != 0 );
    return result;
}

bool ZCc0x32Meter::storeData( IFile& file ) const {
    bool result = true;

    uint8_t param = static_cast<uint8_t>( getType() );
    result = result && file.write( &param, sizeof( param ) );

    size_t count = getScaleCount();
    result = result && file.write( &count, sizeof( count ) );
    for( size_t i = 0; i < count; ++i ) {
        param = getScale( i );
        result = result && file.write( &param, sizeof( param ) );
    }
    param = isResetSupported();
    result = result && file.write( &param, sizeof( param ) );

    return result;
}

void ZCc0x32Meter::setResetSupported( bool reset ) {
    m_resetSupported = reset;
}

bool ZCc0x32Meter::isResetSupported() const {
    return m_resetSupported;
}

meter::MeterType ZCc0x32Meter::getType() const {
    return m_type;
}

size_t ZCc0x32Meter::getScaleCount() const {
    return m_scales.size();
}

bool ZCc0x32Meter::hasScale( meter::Scale scale ) const {
    return std::any_of(
        m_scales.begin(), m_scales.end(), std::bind( std::equal_to<uint8_t>(), std::placeholders::_1, scale ) );
}

meter::Scale ZCc0x32Meter::getScale( uint8_t index ) const {
    return index < m_scales.size() ? m_scales[index] : 0;
}

const std::vector<uint8_t>& ZCc0x32Meter::getScales() const {
    return m_scales;
}

}  // namespace znet
