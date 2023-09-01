#include "znet/cc_anti_theft_base.hpp"
#include "znet/znet_utils.hpp"

#include <cassert>

namespace znet {

bool ZCcAntiTheftBase::storeData( IFile& file ) const {
    const bool hasInfo = m_initialLockInfo.has_value();
    bool result = file.write( &hasInfo, sizeof( hasInfo ) );
    if( hasInfo ) {
        result &= file.write( &m_initialLockInfo->m_isRestricted, sizeof( m_initialLockInfo->m_isRestricted ) ) &&
                  utils::writeVectorToFile( file, m_initialLockInfo->m_hint ) &&
                  file.write( &m_initialLockInfo->m_manufacturerId, sizeof( m_initialLockInfo->m_manufacturerId ) ) &&
                  utils::writeOptionalToFile( file, m_initialLockInfo->m_zwaveAllianceLockingId );
    }

    return result;
}

bool ZCcAntiTheftBase::restoreData( IFile& file, int version ) {
    bool hasInfo = false;
    bool result = file.read( &hasInfo, sizeof( hasInfo ) );

    if( hasInfo ) {
        AntiTheftInfo info;

        result &= file.read( &info.m_isRestricted, sizeof( info.m_isRestricted ) ) &&
                  utils::readVectorFromFile( file, info.m_hint ) &&
                  file.read( &info.m_manufacturerId, sizeof( info.m_manufacturerId ) ) &&
                  utils::readOptionalFromFile( file, info.m_zwaveAllianceLockingId );

        setInitialLockInfo( std::move( info ) );
    }

    return result;
}

const AntiTheftInfoOpt& ZCcAntiTheftBase::getInitialLockInfo() const {
    return m_initialLockInfo;
}

void ZCcAntiTheftBase::setInitialLockInfo( AntiTheftInfoOpt&& info ) {
    assert( !m_initialLockInfo );

    m_initialLockInfo = std::move( info );
}

}  // namespace znet
