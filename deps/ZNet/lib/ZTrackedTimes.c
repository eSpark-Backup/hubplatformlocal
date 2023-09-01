
//////////////////////////////////////////////////////////////////////////////

#include "ZTrackedTimes.h"

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR TrackedTimes_Init( TrackedTimes this ) {
    if( this == NULL )
        return FALSE;
    this->_updated = this->_invalidated = SystemTime_Now();
    TrackedTimes_Update( this );
    return TRUE;
}
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR TrackedTimes_CorrectInvalidated( TrackedTimes this ) {
    if( this->_updated == 0 )
        ++this->_updated;
    this->_invalidated = this->_updated - 1;
}
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR TrackedTimes_CorrectUpdated( TrackedTimes this ) {
    if( this->_invalidated == 0 )
        ++this->_invalidated;
    this->_updated = this->_invalidated - 1;
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR TrackedTimes_Update( TrackedTimes this ) {
    if( ( this->_updated = SystemTime_Now() ) <= this->_invalidated )
        TrackedTimes_CorrectInvalidated( this );
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR TrackedTimes_Invalidate( TrackedTimes this ) {
    if( ( this->_invalidated = SystemTime_Now() ) <= this->_updated )
        TrackedTimes_CorrectUpdated( this );
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR TrackedTimes_IsUpdated( TrackedTimes this ) {
    return ( this->_updated > this->_invalidated );
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR TrackedTimes_IsInvalidated( TrackedTimes this ) {
    return ( this->_invalidated > this->_updated );
}
