#ifndef __ARRAY_BIN_SEARCH_H__
#define __ARRAY_BIN_SEARCH_H__

//#include <stdbool.h>
#include "typedefs.h"

// uncomment if you want to check input args (array,key,foundItemIndex) != 0
//#define SEARCH_CHECK_ARGS

// uncomment if you want to check key is in bound values
//#define SEARCH_CHECK_BOUNDS

//---------------------------------------------------------------------------
typedef int SearchByKeyInArrayCmpFunc_t( const void* key, const void* array, const int index );
//---------------------------------------------------------------------------
BOOL ICACHE_FLASH_ATTR SearchByKeyInArray( const void* key,
                                           const void* array,
                                           const int arrayLen,
                                           SearchByKeyInArrayCmpFunc_t* CmpFunc,
                                           int* foundIndex );
int ICACHE_FLASH_ATTR FindIndexByKeyInArray( const void* key,
                                             const void* array,
                                             const int arrayLen,
                                             SearchByKeyInArrayCmpFunc_t* CmpFunc );

//---------------------------------------------------------------------------
typedef int SearchByKeyCmpFunc_t( const void* key, const int index );
//---------------------------------------------------------------------------
BOOL ICACHE_FLASH_ATTR SearchByKey( const void* key,
                                    const int arrayLen,
                                    SearchByKeyCmpFunc_t* CmpFunc,
                                    int* foundIndex );
int ICACHE_FLASH_ATTR FindIndexByKey( const void* key, const int arrayLen, SearchByKeyCmpFunc_t* CmpFunc );

//---------------------------------------------------------------------------
typedef int SearchInArrayCmpFunc_t( const void* array, const int index );
//---------------------------------------------------------------------------
BOOL ICACHE_FLASH_ATTR SearchInArray( const void* array,
                                      const int arrayLen,
                                      SearchInArrayCmpFunc_t* CmpFunc,
                                      int* foundIndex );
int ICACHE_FLASH_ATTR FindIndexInArray( const void* array, const int arrayLen, SearchInArrayCmpFunc_t* CmpFunc );

//---------------------------------------------------------------------------
typedef int SearchCmpFunc_t( const int index );
//---------------------------------------------------------------------------
BOOL ICACHE_FLASH_ATTR Search( const int arrayLen, SearchCmpFunc_t* CmpFunc, int* foundIndex );
int ICACHE_FLASH_ATTR FindIndex( const int arrayLen, SearchCmpFunc_t* CmpFunc );

#endif  // #ifndef __ARRAY_BIN_SEARCH_H__
