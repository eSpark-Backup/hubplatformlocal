
#ifndef __LISTS_H__
#define __LISTS_H__

//----------------------------------------------------------------------------
#include "lists_cfg.h"

#include "macros.h"

#include "typedefs.h"

typedef enum { List_Insert_Positions_Before, List_Insert_Positions_After } List_Insert_Positions_e;

typedef enum {
    LIST_ENTRY_MOVE_FIRST = ( -2 ),
    LIST_ENTRY_MOVE_PREV = ( -1 ),
    LIST_ENTRY_MOVE_STAY = 0,
    LIST_ENTRY_MOVE_NEXT = 1,
    LIST_ENTRY_MOVE_LAST = 2
} List_Entry_Move_Directions_e;

//----------------------------------------------------------------------------

struct _List_Entry;
#define List_Entry struct _List_Entry*

struct _List;
#define List struct _List*

typedef struct _List_Entry {
    List _list;
    List_Entry _prev;
    List_Entry _next;
    void* const _object;

} List_Entry_t;

//----------------------------------------------------------------------------
typedef struct _List {
    List_Entry _first;
    List_Entry _last;
    int _count;

} ZList_t;

//----------------------------------------------------------------------------
// next declarations look the same but they have different sense
// first return TRUE if an item meet certain condition during item search
// second return TRUE if we xan continue to map actions to remain items, else we
// have to break a ForEach "loop".
typedef void ( *List_ObjectFunc )( List_Entry entry, void* arg );
typedef bool ( *List_CheckObjectFunc )( const void* object, const void* arg );
typedef bool ( *List_CmpObjectsFunc )( const void* object1, const void* object2 );  //(*CmpFun)(object1,object2)
                                                                                    // must return TRUE if object1
                                                                                    //"<=" object2
typedef void ( *List_FreeObjectFunc )( void* object );

//----------------------------------------------------------------------------

List ICACHE_FLASH_ATTR List_Create( void );
bool ICACHE_FLASH_ATTR List_Init( List list );
bool ICACHE_FLASH_ATTR List_DeInit( List list, List_FreeObjectFunc FreeObjectFunc );
bool ICACHE_FLASH_ATTR List_Free( List list, List_FreeObjectFunc FreeObjectFunc );

// List_Clear just removes items all from the list (if FreeItemFunc == NULL) or
// frees them (if FreeItemFunc != NULL)
bool ICACHE_FLASH_ATTR List_Clear( List list, List_FreeObjectFunc FreeObjectFunc );
bool ICACHE_FLASH_ATTR List_IsEmpty( const List list );

List_Entry ICACHE_FLASH_ATTR List_Add( List list, const void* const object );
List_Entry ICACHE_FLASH_ATTR List_Insert( List list,
                                          const void* const object,
                                          List_Entry entry,
                                          List_Insert_Positions_e pos );
bool ICACHE_FLASH_ATTR List_Remove( List list, List_Entry entry, List_FreeObjectFunc FreeObjectFunc );

bool ICACHE_FLASH_ATTR List_Move( List list, List_Entry entry, List_Entry_Move_Directions_e direction );

List_Entry ICACHE_FLASH_ATTR List_First( const List list );
List_Entry ICACHE_FLASH_ATTR List_Prev( const List list, List_Entry entry );
List_Entry ICACHE_FLASH_ATTR List_Next( const List list, List_Entry entry );
List_Entry ICACHE_FLASH_ATTR List_Last( const List list );

void ICACHE_FLASH_ATTR List_ForEach( const List list, List_ObjectFunc ObjectFunc, void* itemFuncArg );
//(*CmpFun)(object1,object2) must return TRUE if object1 "<=" object2
bool ICACHE_FLASH_ATTR List_Sort( List list, List_CmpObjectsFunc CmpFunc );

List_Entry ICACHE_FLASH_ATTR List_Find( const List list, List_CheckObjectFunc CheckFunc, const void* checkFuncArg );
List_Entry ICACHE_FLASH_ATTR List_FindByObject( const List list, const void* object );

bool ICACHE_FLASH_ATTR List_Assign( List list, const List src );

//----------------------------------------------------------------------------

bool ICACHE_FLASH_ATTR Lists_Init( void );

//----------------------------------------------------------------------------

#if( defined( _WINDOWS ) || defined( __APPLE__ ) || defined( __linux__ ) )
void ICACHE_FLASH_ATTR List_Test( void );
#endif  // defined(_WINDOWS) || defined(__APPLE__) || defined(__linux__))

#endif  // #ifndef __LISTS_H__
