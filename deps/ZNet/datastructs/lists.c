//////////////////////////////////////////////////////////////////////////////

#include "lists_cfg.h"

#include <stdlib.h>
#include <stdio.h>

#include "heap.h"
#include "lists.h"

#include "logger.h"

#ifdef USE_LISTS_POOLS

//////////////////////////////////////////////////////////////////////////////

List_Entry entryPool = NULL;

//----------------------------------------------------------------------------
static List_Entry ICACHE_FLASH_ATTR List_Entry_Pool_Get( void ) {
    List_Entry entry = entryPool;
    if( entryPool )
        entryPool = entryPool->_next;
    return entry;
}
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR List_Entry_Pool_Put( List_Entry entry ) {
    if( entryPool )
        entry->_next = entryPool;
    else
        entry->_next = NULL;
    entryPool = entry;
}

//////////////////////////////////////////////////////////////////////////////

List listPool = NULL;

//----------------------------------------------------------------------------
static List ICACHE_FLASH_ATTR List_Pool_Get( void ) {
    List list = listPool;
    if( listPool )
        listPool = (List)listPool->_first;
    return list;
}
//----------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR List_Pool_Put( List list ) {
    if( listPool )
        list->_first = (List_Entry)listPool;
    else
        list->_first = NULL;
    listPool = list;
}

#endif  // USE_LISTS_POOL

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
static List_Entry ICACHE_FLASH_ATTR List_Entry_Create( const void* const object ) {
    List_Entry entry;

#ifdef USE_LISTS_POOLS
    if( ( entry = List_Entry_Pool_Get() ) == NULL )
#endif  // USE_LISTS_POOL
        entry = (List_Entry)Heap_Alloc( sizeof( List_Entry_t ) );

    if( entry ) {
        entry->_list = NULL;
        entry->_prev = NULL;
        entry->_next = NULL;
        *( (void**)&entry->_object ) = (void*)object;
    }
    return entry;
}
//----------------------------------------------------------------------------

static void ICACHE_FLASH_ATTR List_Entry_Free( List_Entry this ) {
#ifdef USE_LISTS_POOLS
    List_Entry_Pool_Put( this );
#else
    Heap_Free( this );
#endif  // USE_LISTS_POOL
}

//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR List_IncludeEntry( List this, List_Entry entry, List_Entry prev, List_Entry next ) {
    if( this == NULL || entry == NULL || entry->_list || entry->_object == NULL )
        return FALSE;

    switch( ( ( prev ? 1 : 0 ) << 1 ) | ( next ? 1 : 0 ) ) {
        /*0bpn*/
        /*   n----> next != NULL */
        /*  p-----> prev != NULL */
    case 0 /*0b00*/:
        if( this->_count > 0 || this->_first != NULL || this->_last != NULL )
            return FALSE;
        this->_first = this->_last = entry;
        break;

    case 1 /*0b01*/:
        if( ( this->_count == 0 || this->_first != next || this->_last == NULL ) ||
            ( next->_list != this || next->_prev != NULL ) )
            return FALSE;
        ( ( this->_first = entry )->_next = next )->_prev = entry;
        break;

    case 2 /*0b10*/:
        if( ( this->_count == 0 || this->_first == NULL || this->_last != prev ) ||
            ( prev->_list != this || prev->_next != NULL ) )
            return FALSE;
        ( ( this->_last = entry )->_prev = prev )->_next = entry;
        break;

    case 3 /*0b11*/:
        if( ( this->_count < 2 || this->_first == NULL || this->_last == NULL ) ||
            ( prev->_list != this || prev->_next != next ) || ( next->_list != this || next->_prev != prev ) )
            return FALSE;
        ( entry->_prev = prev )->_next = ( entry->_next = next )->_prev = entry;
        break;
    default:
        break;
    }
    entry->_list = this;
    ++this->_count;
    return TRUE;
}
//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR List_ExcludeEntry( List this, List_Entry entry ) {
    List_Entry prev;
    List_Entry next;

    if( this == NULL || entry == NULL || entry->_list != this || entry->_object == NULL )
        return FALSE;

    switch( ( ( ( prev = entry->_prev ) ? 1 : 0 ) << 1 ) | ( ( next = entry->_next ) ? 1 : 0 ) ) {
        /*0bpn*/
        /*   n----> next != NULL */
        /*  p-----> prev != NULL */
    case 0 /*0b00*/:
        if( ( this->_count == 0 || this->_first != entry || this->_last != entry ) )
            return FALSE;
        this->_first = this->_last = NULL;
        break;

    case 1 /*0b01*/:
        if( ( this->_count < 2 || this->_first != entry || this->_last == NULL ) ||
            ( next->_list != this || next->_prev != entry ) )
            return FALSE;
        entry->_next = ( this->_first = entry->_next )->_prev = NULL;
        break;

    case 2 /*0b10*/:
        if( ( this->_count < 2 || this->_first == NULL || this->_last != entry ) ||
            ( prev->_list != this || prev->_next != entry ) )
            return FALSE;
        entry->_prev = ( this->_last = entry->_prev )->_next = NULL;
        break;

    case 3 /*0b11*/:
        if( ( this->_count < 3 || this->_first == NULL || this->_last == NULL ) ||
            ( prev->_list != this || prev->_next != entry ) || ( next->_list != this || next->_prev != entry ) )
            return FALSE;
        entry->_prev->_next = entry->_next;
        entry->_next->_prev = entry->_prev;
        entry->_next = entry->_prev = NULL;
        break;
    default:
        break;
    }
    --this->_count;
    entry->_list = NULL;
    return TRUE;
}
//----------------------------------------------------------------------------
static bool ICACHE_FLASH_ATTR List_InsertEntry( List this,
                                                List_Entry newEntry,
                                                List_Entry entry,
                                                List_Insert_Positions_e pos ) {
    if( entry ) {
        // if( ! List_IsEntryInList( this, entry ) ) return FALSE;
        switch( pos ) {
        case List_Insert_Positions_Before:
            return List_IncludeEntry( this, newEntry, entry->_prev, entry );
        case List_Insert_Positions_After:
            return List_IncludeEntry( this, newEntry, entry, entry->_next );
        default:
            break;
        }
    }
    else {  // entry == NULL
        switch( pos ) {
        case List_Insert_Positions_Before:
            return List_IncludeEntry( this, newEntry, NULL,
                                      this->_first );  // add as a new first in a list
        case List_Insert_Positions_After:
            return List_IncludeEntry( this, newEntry, this->_last,
                                      NULL );  // add as a new last in a list
        default:
            break;
        }
    }
    return FALSE;
}

//----------------------------------------------------------------------------
List ICACHE_FLASH_ATTR List_Create( void ) {
    List list;

#ifdef USE_LISTS_POOLS
    if( ( list = List_Pool_Get() ) == NULL )
#endif  // USE_LISTS_POOL
        list = Heap_Alloc( sizeof( ZList_t ) );

    if( list ) {
        if( !List_Init( list ) ) {
#ifdef USE_LISTS_POOLS
            List_Pool_Put( list );
#else
            Heap_Free( list );
#endif  // USE_LISTS_POOL
            return NULL;
        }
    }
    return list;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR List_Init( List this ) {
    this->_first = NULL;
    this->_last = NULL;
    this->_count = 0;
    return TRUE;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR List_DeInit( List this, List_FreeObjectFunc FreeObjectFunc ) {
    return List_Clear( this, FreeObjectFunc );
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR List_Free( List this, List_FreeObjectFunc FreeObjectFunc ) {
    if( this == NULL )
        return FALSE;
    if( !List_DeInit( this, FreeObjectFunc ) )
        return FALSE;

#ifdef USE_LISTS_POOLS
    List_Pool_Put( this );
#else
    Heap_Free( this );
#endif  // USE_LISTS_POOL

    return TRUE;
}
//----------------------------------------------------------------------------
// List_Clear just removes items all from the list (if FreeItemFunc == NULL) or
// frees them (if FreeItemFunc != NULL)
bool ICACHE_FLASH_ATTR List_Clear( List this, List_FreeObjectFunc FreeObjectFunc ) {
    List_Entry entry;

    if( this == NULL )
        return FALSE;

    while( ( entry = this->_first ) )
        if( !List_Remove( this, entry, FreeObjectFunc ) )
            return FALSE;
    return TRUE;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR List_IsEmpty( const List this ) {
    return ( this == NULL || this->_count == 0 );
}
//----------------------------------------------------------------------------
List_Entry ICACHE_FLASH_ATTR List_Add( List this, const void* object ) {
    if( this == NULL || object == NULL )
        return NULL;

    List_Entry entry = List_Entry_Create( object );
    if( entry ) {
        if( !List_IncludeEntry( this, entry, this->_last, NULL ) ) {
            List_Entry_Free( entry );
            return NULL;
        }
    }
    return entry;
}
//----------------------------------------------------------------------------
List_Entry ICACHE_FLASH_ATTR List_Insert( List this,
                                          const void* const object,
                                          List_Entry entry,
                                          List_Insert_Positions_e pos ) {
    if( this == NULL || object == NULL )
        return NULL;

    List_Entry newEntry = List_Entry_Create( object );
    if( newEntry ) {
        if( !List_InsertEntry( this, newEntry, entry, pos ) ) {
            List_Entry_Free( newEntry );
            return NULL;
        }
    }
    return newEntry;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR List_Remove( List this, List_Entry entry, List_FreeObjectFunc FreeObjectFunc ) {
    // const void* object;

    if( this == NULL || entry == NULL )
        return FALSE;

    void* object = entry->_object;
    if( object == NULL )
        return FALSE;

    if( !List_ExcludeEntry( this, entry ) )
        return FALSE;
    // object = entry->_object;
    List_Entry_Free( entry );
    if( FreeObjectFunc ) {
        ( *FreeObjectFunc )( object );
        // if( (entry = List_Entry_Create( object )) == NULL ) return FALSE;
        // List_IncludeEntry( this, entry, NULL, this->_first );
        // return FALSE;
    }
    return TRUE;
}
//----------------------------------------------------------------------------
bool ICACHE_FLASH_ATTR List_Move( List this, List_Entry entry, List_Entry_Move_Directions_e direction ) {
    List_Entry prev;
    List_Entry next;

    if( this == NULL || entry == NULL || entry->_list != this )
        return FALSE;

    switch( direction ) {
    case LIST_ENTRY_MOVE_FIRST:
        if( this->_first == entry )
            return TRUE;
        prev = NULL;
        next = this->_first;
        break;

    case LIST_ENTRY_MOVE_PREV:
        if( this->_first == entry )
            return TRUE;
        next = entry->_prev;
        prev = next->_prev;
        break;

    case LIST_ENTRY_MOVE_STAY:
        return TRUE;
        break;

    case LIST_ENTRY_MOVE_NEXT:
        if( this->_last == entry )
            return TRUE;
        prev = entry->_next;
        next = prev->_next;
        break;

    case LIST_ENTRY_MOVE_LAST:
        if( this->_last == entry )
            return TRUE;
        prev = this->_last;
        next = NULL;
        break;
    default:
        return FALSE;
    }
    if( !List_ExcludeEntry( this, entry ) )
        return FALSE;
    if( !List_IncludeEntry(
            this, entry, prev, next ) ) {  // !!! here is a VERY BAD THING! an non-freed object is hang in a memory
        // !!!
        // List_Entry_Free( entry );
        return FALSE;
    }
    return TRUE;
}
//----------------------------------------------------------------------------
List_Entry ICACHE_FLASH_ATTR List_First( const List this ) {
    if( this == NULL || this->_count == 0 || this->_last == NULL )
        return NULL;
    return this->_first;
}
//----------------------------------------------------------------------------
List_Entry ICACHE_FLASH_ATTR List_Prev( const List this, List_Entry entry ) {
    List_Entry prev;

    if( ( this == NULL || entry == NULL || entry->_list != this || this->_count == 0 || this->_first == entry ) ||
        ( ( prev = entry->_prev ) && ( prev->_next != entry || prev->_list != this ) ) )
        return NULL;
    return prev;
}
//----------------------------------------------------------------------------
List_Entry ICACHE_FLASH_ATTR List_Next( const List this, List_Entry entry ) {
    List_Entry next;

    if( ( this == NULL || entry == NULL || entry->_list != this || this->_count == 0 || this->_last == entry ) ||
        ( ( next = entry->_next ) && ( next->_prev != entry || next->_list != this ) ) )
        return NULL;
    return next;
}
//----------------------------------------------------------------------------
List_Entry ICACHE_FLASH_ATTR List_Last( const List this ) {
    if( this == NULL || this->_count == 0 || this->_first == NULL )
        return NULL;
    return this->_last;
}
//----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR List_ForEach( const List this, List_ObjectFunc ObjectFunc, void* itemFuncArg ) {
    List_Entry entry;

    if( this == NULL || ObjectFunc == NULL )
        return;

    for( entry = this->_first; entry; entry = entry->_next )
        ( *ObjectFunc )( entry, itemFuncArg );
}
//----------------------------------------------------------------------------
//(*CmpFun)(object1,object2) must return TRUE if object1 "<=" object2
bool ICACHE_FLASH_ATTR List_Sort( List this, List_CmpObjectsFunc CmpObjectsFunc ) {
    List_Entry otherFirst;

    if( this == NULL || CmpObjectsFunc == NULL )
        return FALSE;

    if( this->_first == NULL )
        return TRUE;
    otherFirst = this->_first->_next;
    if( otherFirst == NULL )
        return TRUE;
    ( this->_last = this->_first )->_next = NULL;
    while( otherFirst ) {
        List_Entry otherNext = otherFirst->_next;

        List_Entry curr = this->_first;
        for( ; curr && ( *CmpObjectsFunc )( curr->_object, otherFirst->_object ) /*<= 0*/;
             curr = curr->_next ) /* !!! */
            ;
        if( ( otherFirst->_next = curr ) ) {
            if( ( otherFirst->_prev = curr->_prev ) )
                curr->_prev->_next = otherFirst;
            else
                this->_first = otherFirst;
            curr->_prev = otherFirst;
        }
        else {  // curr == NULL
            otherFirst->_prev = this->_last;
            this->_last->_next = otherFirst;
            this->_last = otherFirst;
        }

        otherFirst = otherNext;
    }
    return TRUE;
}
//----------------------------------------------------------------------------
List_Entry ICACHE_FLASH_ATTR List_Find( const List this,
                                        List_CheckObjectFunc CheckObjectFunc,
                                        const void* checkObjectFuncArg ) {
    List_Entry entry;

    if( this == NULL || CheckObjectFunc == NULL )
        return NULL;

    for( entry = this->_first; entry && !( *CheckObjectFunc )( entry->_object, checkObjectFuncArg );
         entry = entry->_next ) /* !!! */
        ;
    return entry;
}
//----------------------------------------------------------------------------
List_Entry ICACHE_FLASH_ATTR List_FindByObject( const List this, const void* object ) {
    List_Entry entry;

    if( this == NULL )
        return NULL;

    for( entry = this->_first; entry && entry->_object != object; entry = entry->_next ) /* !!! */
        ;
    return entry;
}

//----------------------------------------------------------------------------

static void ICACHE_FLASH_ATTR CopyObject( List_Entry entry, void* arg ) {
    List_Add( (List)arg /*dst*/, entry->_object );
}

bool ICACHE_FLASH_ATTR List_Assign( List this, const List src ) {
    bool result;

    if( this == NULL || this->_count || src == NULL )
        return FALSE;

    List_ForEach( src, &CopyObject, this );
    if( !( result = ( this->_count == src->_count ) ) )
        List_Clear( this, NULL );
    return result;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

bool ICACHE_FLASH_ATTR Lists_Init( void ) {
#ifdef USE_LISTS_POOLS
    for( int i = LISTENTRY_POOL_START_COUNT; i; --i ) {
        List_Entry entry = (List_Entry)Heap_Alloc( sizeof( *entry ) );
        if( entry == NULL )
            return FALSE;
        List_Entry_Pool_Put( entry );
    }
    for( int i = LIST_POOL_START_COUNT; i; --i ) {
        List list = (List)Heap_Alloc( sizeof( *list ) );
        if( list == NULL ) {
            for( int j = i + 1; j <= LIST_POOL_START_COUNT; ++j ) {
                List list = List_Pool_Get();
                Heap_Free( list );
            }
            for( int j = 1; j <= LISTENTRY_POOL_START_COUNT; ++j ) {
                List_Entry entry = List_Entry_Pool_Get();
                Heap_Free( entry );
            }
            return FALSE;
        }
        List_Pool_Put( list );
    }
#endif  // USE_LISTS_POOLS

    return TRUE;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#if( defined( _WINDOWS ) || defined( __APPLE__ ) || defined( __linux__ ) )

//----------------------------------------------------------------------------

static void ICACHE_FLASH_ATTR FreeObjectFunc( void* object ) {
    Heap_Free( object );  // returns FALSE if fault
}

/*
static void ICACHE_FLASH_ATTR ShowObjectFunc( List_Entry entry, void* arg )
{
        Log_Write( "ShowObjectFunc : value = %d | %s\r\n",
*(int*)entry->_object, (char*)arg );
}
*/

void ICACHE_FLASH_ATTR List_Test( void ) {
    setvbuf( stdout, NULL, _IONBF, 0 );
    setvbuf( stderr, NULL, _IONBF, 0 );

    ZList_t list;

    // Heap_Show();

    if( !List_Init( &list ) ) {
        Log_Write( "\n!!! List_Init ERROR !!!\r\n" );
        return;
    }

    int iFinish = 1;
    for( int i = 0; i < iFinish; ++i ) {
        Log_Write( "\n========================================================="
                   "====\r\n" );

        int* pI;
        int value = 0;
        while( /*value < 5 &&*/ ( pI = (int*)Heap_Alloc( ( size_t )( (int)sizeof( int ) + ( rand() % 257 ) /*1000000*/
                                                                     ) ) ) ) {
            *pI = value++;
            if( List_Add( &list, pI ) == NULL ) {
                Log_Write( "\n!!! List_Add ERROR 1 : value = %d !!!\r\n", *pI );
                Heap_Free( pI );
                break;
            }
        }

        // List_ForEach( &list, ShowObjectFunc, ": 1" );
        Log_Write( "1: list: count: %d\r\n", list._count );
        // Heap_Show();

        List_Entry entry = List_First( &list );
        while( entry ) {
            List_Entry nextEntry = List_Next( &list, entry );
            if( *(int*)entry->_object & 1 ) {
                if( !List_Remove( &list, entry, &FreeObjectFunc ) ) {
                    Log_Write( "\n!!! List_Remove ERROR : value = %d !!!\r\n", *(int*)entry->_object );
                    break;
                }
            }
            entry = nextEntry;
        }

        // List_ForEach( &list, ShowObjectFunc, ": 2" );
        Log_Write( "2: list: count: %d\r\n", list._count );
        // Heap_Show();

        while( /*value < 5 &&*/ ( pI = (int*)Heap_Alloc( ( size_t )( (int)sizeof( int ) + ( rand() % 257 ) /*1000000*/
                                                                     ) ) ) ) {
            *pI = value++;
            if( List_Add( &list, pI ) == NULL ) {
                Log_Write( "\n!!! List_Add ERROR 2 : value = %d !!!\r\n", *pI );
                Heap_Free( pI );
                break;
            }
        }

        // List_ForEach( &list, ShowObjectFunc, ": 3" );
        Log_Write( "3: list: count: %d\r\n", list._count );
        // Heap_Show();

        if( !List_Clear( &list, &FreeObjectFunc ) ) {
            Log_Write( "\n!!! List_Clear ERROR !!!\r\n" );
            break;
        }

        Log_Write( "after Clear:: list: count: %d\r\n", list._count );
        // List_ForEach( &list, ShowObjectFunc, ": 4" );
        // Heap_Show();

        Log_Write( "\n========================================================="
                   "====\r\n" );
    }

    if( !List_DeInit( &list, &FreeObjectFunc ) ) {
        Log_Write( "\n!!! List_DeInit ERROR !!!\r\n" );
        return;
    }

    // Heap_Show();
}

#endif  // (defined(_WINDOWS) || defined(__APPLE__) || defined(__linux__))
