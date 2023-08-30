//===========================================================================

//#include <stdint.h>
//#include <string.h>

#include "array_bin_search.h"

//===========================================================================

//---------------------------------------------------------------------------
BOOL ICACHE_FLASH_ATTR SearchByKeyInArray( const void* key,
                                           const void* array,
                                           const int arrayLen,
                                           SearchByKeyInArrayCmpFunc_t* CmpFunc,
                                           int* foundIndex ) {
#ifdef SEARCH_CHECK_ARGS
    if( foundIndex == NULL ) {
        return false;
    }
    if( ( arrayLen < 0 ) || ( key == NULL ) || ( array == NULL ) || ( CmpFunc == NULL ) ) {
        *foundIndex = -1;
        return false;
    }
#endif  // #ifdef SEARCH_CHECK_ARGS
/* Начальная проверка, которую, в принципе, можно опустить — но тогда см. ниже!
 */
#ifdef SEARCH_CHECK_BOUNDS
    if( arrayLen == 0 ) { /* массив пуст */
        *foundIndex = 0;
        return false;
    }
    else if( /*x<array[0]     */ CmpFunc( key, array, 0 ) < 0 ) { /* искомый элемент меньше всех в массиве */
        *foundIndex = 0;
        return false;
    }
    else if( /*a[arrayLen-1]<x*/ CmpFunc( key, array, arrayLen - 1 ) > 0 ) { /* искомый элемент больше всех в массиве */
        *foundIndex = arrayLen;
        return false;
    }
#endif  // #ifdef SEARCH_CHECK_BOUNDS

    int first = 0;       /* Номер первого элемента в массиве */
    int last = arrayLen; /* Номер элемента в массиве, СЛЕДУЮЩЕГО ЗА последним */
    while( first < last ) { /* Если просматриваемый участок непустой, first < last */
        int mid = first + ( last - first ) / 2;
        if( /*x <= a[mid]*/ CmpFunc( key, array, mid ) <= 0 ) {
            last = mid;
        }
        else {
            first = mid + 1;
        }
    }
    *foundIndex = last;
    return (
#ifndef SEARCH_CHECK_BOUNDS
        /*last < n*/ ( last < arrayLen ) &&
#endif
        /*a[last] == x*/ ( CmpFunc( key, array, last ) == 0 ) );
}

//---------------------------------------------------------------------------
int ICACHE_FLASH_ATTR FindIndexByKeyInArray( const void* key,
                                             const void* array,
                                             const int arrayLen,
                                             SearchByKeyInArrayCmpFunc_t* CmpFunc ) {
#ifdef SEARCH_CHECK_ARGS
    if( ( arrayLen < 0 ) || ( key == NULL ) || ( array == NULL ) || ( CmpFunc == NULL ) ) {
        return -1;
    }
#endif  // #ifdef SEARCH_CHECK_ARGS
/* Начальная проверка, которую, в принципе, можно опустить — но тогда см. ниже!
 */
#ifdef SEARCH_CHECK_BOUNDS
    if( arrayLen == 0 ) { /* массив пуст */
        return -1;
    }
    else if( /*x<array[0]     */ CmpFunc( key, array, 0 ) < 0 ) { /* искомый элемент меньше всех в массиве */
        return -1;
    }
    else if( /*a[arrayLen-1]<x*/ CmpFunc( key, array, arrayLen - 1 ) > 0 ) { /* искомый элемент больше всех в массиве */
        return -1;
    }
#endif  // #ifdef SEARCH_CHECK_BOUNDS

    int first = 0;       /* Номер первого элемента в массиве */
    int last = arrayLen; /* Номер элемента в массиве, СЛЕДУЮЩЕГО ЗА последним */
    while( first < last ) { /* Если просматриваемый участок непустой, first < last */
        int mid = first + ( last - first ) / 2;
        if( /*x <= a[mid]*/ CmpFunc( key, array, mid ) <= 0 ) {
            last = mid;
        }
        else {
            first = mid + 1;
        }
    }
    return ( (
#ifndef SEARCH_CHECK_BOUNDS
                 /*last < n*/ ( last < arrayLen ) &&
#endif
                 /*a[last] == x*/ ( CmpFunc( key, array, last ) == 0 ) ) ?
                 last :
                 -1 );
}

//===========================================================================

//---------------------------------------------------------------------------
BOOL ICACHE_FLASH_ATTR SearchByKey( const void* key,
                                    const int arrayLen,
                                    SearchByKeyCmpFunc_t* CmpFunc,
                                    int* foundIndex ) {
#ifdef SEARCH_CHECK_ARGS
    if( foundIndex == NULL ) {
        return false;
    }
    if( ( arrayLen < 0 ) || ( key == NULL ) || ( CmpFunc == NULL ) ) {
        *foundIndex = -1;
        return false;
    }
#endif  // #ifdef SEARCH_CHECK_ARGS
/* Начальная проверка, которую, в принципе, можно опустить — но тогда см. ниже!
 */
#ifdef SEARCH_CHECK_BOUNDS
    if( arrayLen == 0 ) { /* массив пуст */
        *foundIndex = 0;
        return false;
    }
    else if( /*x<array[0]     */ CmpFunc( key, 0 ) < 0 ) { /* искомый элемент меньше всех в массиве */
        *foundIndex = 0;
        return false;
    }
    else if( /*a[arrayLen-1]<x*/ CmpFunc( key, arrayLen - 1 ) > 0 ) { /* искомый элемент больше всех в массиве */
        *foundIndex = arrayLen;
        return false;
    }
#endif  // #ifdef SEARCH_CHECK_BOUNDS

    int first = 0;       /* Номер первого элемента в массиве */
    int last = arrayLen; /* Номер элемента в массиве, СЛЕДУЮЩЕГО ЗА последним */
    while( first < last ) { /* Если просматриваемый участок непустой, first < last */
        int mid = first + ( last - first ) / 2;
        if( /*x <= a[mid]*/ CmpFunc( key, mid ) <= 0 ) {
            last = mid;
        }
        else {
            first = mid + 1;
        }
    }
    *foundIndex = last;
    return (
#ifndef SEARCH_CHECK_BOUNDS
        /*last < n*/ ( last < arrayLen ) &&
#endif
        /*a[last] == x*/ ( CmpFunc( key, last ) == 0 ) );
}

#include "logger.h"

//---------------------------------------------------------------------------
int ICACHE_FLASH_ATTR FindIndexByKey( const void* key, const int arrayLen, SearchByKeyCmpFunc_t* CmpFunc ) {
#ifdef SEARCH_CHECK_ARGS
    if( ( arrayLen < 0 ) || ( key == NULL ) || ( CmpFunc == NULL ) ) {
        return -1;
    }
#endif  // #ifdef SEARCH_CHECK_ARGS
/* Начальная проверка, которую, в принципе, можно опустить — но тогда см. ниже!
 */
#ifdef SEARCH_CHECK_BOUNDS
    if( arrayLen == 0 ) { /* массив пуст */
        return -1;
    }
    else if( /*x<array[0]     */ CmpFunc( key, 0 ) < 0 ) { /* искомый элемент меньше всех в массиве */
        return -1;
    }
    else if( /*a[arrayLen-1]<x*/ CmpFunc( key, arrayLen - 1 ) > 0 ) { /* искомый элемент больше всех в массиве */
        return -1;
    }
#endif  // #ifdef SEARCH_CHECK_BOUNDS

    int first = 0;       /* Номер первого элемента в массиве */
    int last = arrayLen; /* Номер элемента в массиве, СЛЕДУЮЩЕГО ЗА последним */
    while( first < last ) { /* Если просматриваемый участок непустой, first < last */
        int mid = first + ( last - first ) / 2;
        if( /*x <= a[mid]*/ CmpFunc( key, mid ) <= 0 ) {
            last = mid;
        }
        else {
            first = mid + 1;
        }
    }
    //  Log_Write("\r\nFindIndexByKey last=%d  first=%d arrayLen=%d \r\n",last,
    //  first,arrayLen );
    return ( (
#ifndef SEARCH_CHECK_BOUNDS
                 /*last < n*/ ( last < arrayLen ) &&
#endif
                 /*a[last] == x*/ ( CmpFunc( key, last ) == 0 ) ) ?
                 last :
                 -1 );
}

//===========================================================================

//---------------------------------------------------------------------------
BOOL ICACHE_FLASH_ATTR SearchInArray( const void* array,
                                      const int arrayLen,
                                      SearchInArrayCmpFunc_t* CmpFunc,
                                      int* foundIndex ) {
#ifdef SEARCH_CHECK_ARGS
    if( foundIndex == NULL ) {
        return false;
    }
    if( ( arrayLen < 0 ) || ( array == NULL ) || ( CmpFunc == NULL ) ) {
        *foundIndex = -1;
        return false;
    }
#endif  // #ifdef SEARCH_CHECK_ARGS
/* Начальная проверка, которую, в принципе, можно опустить — но тогда см. ниже!
 */
#ifdef SEARCH_CHECK_BOUNDS
    if( arrayLen == 0 ) { /* массив пуст */
        *foundIndex = 0;
        return false;
    }
    else if( /*x<array[0]     */ CmpFunc( array, 0 ) < 0 ) { /* искомый элемент меньше всех в массиве */
        *foundIndex = 0;
        return false;
    }
    else if( /*a[arrayLen-1]<x*/ CmpFunc( array, arrayLen - 1 ) > 0 ) { /* искомый элемент больше всех в массиве */
        *foundIndex = arrayLen;
        return false;
    }
#endif  // #ifdef SEARCH_CHECK_BOUNDS

    int first = 0;       /* Номер первого элемента в массиве */
    int last = arrayLen; /* Номер элемента в массиве, СЛЕДУЮЩЕГО ЗА последним */
    while( first < last ) { /* Если просматриваемый участок непустой, first < last */
        int mid = first + ( last - first ) / 2;
        if( /*x <= a[mid]*/ CmpFunc( array, mid ) <= 0 ) {
            last = mid;
        }
        else {
            first = mid + 1;
        }
    }
    *foundIndex = last;
    return (
#ifndef SEARCH_CHECK_BOUNDS
        /*last < n*/ ( last < arrayLen ) &&
#endif
        /*a[last] == x*/ ( CmpFunc( array, last ) == 0 ) );
}

//---------------------------------------------------------------------------
int ICACHE_FLASH_ATTR FindIndexInArray( const void* array, const int arrayLen, SearchInArrayCmpFunc_t* CmpFunc ) {
#ifdef SEARCH_CHECK_ARGS
    if( ( arrayLen < 0 ) || ( array == NULL ) || ( CmpFunc == NULL ) ) {
        return -1;
    }
#endif  // #ifdef SEARCH_CHECK_ARGS
/* Начальная проверка, которую, в принципе, можно опустить — но тогда см. ниже!
 */
#ifdef SEARCH_CHECK_BOUNDS
    if( arrayLen == 0 ) { /* массив пуст */
        return -1;
    }
    else if( /*x<array[0]     */ CmpFunc( array, 0 ) < 0 ) { /* искомый элемент меньше всех в массиве */
        return -1;
    }
    else if( /*a[arrayLen-1]<x*/ CmpFunc( array, arrayLen - 1 ) > 0 ) { /* искомый элемент больше всех в массиве */
        return -1;
    }
#endif  // #ifdef SEARCH_CHECK_BOUNDS

    int first = 0;       /* Номер первого элемента в массиве */
    int last = arrayLen; /* Номер элемента в массиве, СЛЕДУЮЩЕГО ЗА последним */
    while( first < last ) { /* Если просматриваемый участок непустой, first < last */
        int mid = first + ( last - first ) / 2;
        if( /*x <= a[mid]*/ CmpFunc( array, mid ) <= 0 ) {
            last = mid;
        }
        else {
            first = mid + 1;
        }
    }
    return ( (
#ifndef SEARCH_CHECK_BOUNDS
                 /*last < n*/ ( last < arrayLen ) &&
#endif
                 /*a[last] == x*/ ( CmpFunc( array, last ) == 0 ) ) ?
                 last :
                 -1 );
}
//---------------------------------------------------------------------------
BOOL ICACHE_FLASH_ATTR Search( const int arrayLen, SearchCmpFunc_t* CmpFunc, int* foundIndex ) {
#ifdef SEARCH_CHECK_ARGS
    if( foundIndex == NULL ) {
        return false;
    }
    if( ( arrayLen < 0 ) || ( CmpFunc == NULL ) ) {
        *foundIndex = -1;
        return false;
    }
#endif  // #ifdef SEARCH_CHECK_ARGS
/* Начальная проверка, которую, в принципе, можно опустить — но тогда см. ниже!
 */
#ifdef SEARCH_CHECK_BOUNDS
    if( arrayLen == 0 ) { /* массив пуст */
        *foundIndex = 0;
        return false;
    }
    else if( /*x<array[0]     */ CmpFunc( 0 ) < 0 ) { /* искомый элемент меньше всех в массиве */
        *foundIndex = 0;
        return false;
    }
    else if( /*a[arrayLen-1]<x*/ CmpFunc( arrayLen - 1 ) > 0 ) { /* искомый элемент больше всех в массиве */
        *foundIndex = arrayLen;
        return false;
    }
#endif  // #ifdef SEARCH_CHECK_BOUNDS

    int first = 0;       /* Номер первого элемента в массиве */
    int last = arrayLen; /* Номер элемента в массиве, СЛЕДУЮЩЕГО ЗА последним */
    while( first < last ) { /* Если просматриваемый участок непустой, first < last */
        int mid = first + ( last - first ) / 2;
        if( /*x <= a[mid]*/ CmpFunc( mid ) <= 0 ) {
            last = mid;
        }
        else {
            first = mid + 1;
        }
    }
    *foundIndex = last;
    return (
#ifndef SEARCH_CHECK_BOUNDS
        /*last < n*/ ( last < arrayLen ) &&
#endif
        /*a[last] == x*/ ( CmpFunc( last ) == 0 ) );
}

//---------------------------------------------------------------------------
int ICACHE_FLASH_ATTR FindIndex( const int arrayLen, SearchCmpFunc_t* CmpFunc ) {
#ifdef SEARCH_CHECK_ARGS
    if( ( arrayLen < 0 ) || ( CmpFunc == NULL ) ) {
        return -1;
    }
#endif  // #ifdef SEARCH_CHECK_ARGS
/* Начальная проверка, которую, в принципе, можно опустить — но тогда см. ниже!
 */
#ifdef SEARCH_CHECK_BOUNDS
    if( arrayLen == 0 ) { /* массив пуст */
        return -1;
    }
    else if( /*x<array[0]     */ CmpFunc( 0 ) < 0 ) { /* искомый элемент меньше всех в массиве */
        return -1;
    }
    else if( /*a[arrayLen-1]<x*/ CmpFunc( arrayLen - 1 ) > 0 ) { /* искомый элемент больше всех в массиве */
        return -1;
    }
#endif  // #ifdef SEARCH_CHECK_BOUNDS

    int first = 0;       /* Номер первого элемента в массиве */
    int last = arrayLen; /* Номер элемента в массиве, СЛЕДУЮЩЕГО ЗА последним */
    while( first < last ) { /* Если просматриваемый участок непустой, first < last */
        int mid = first + ( last - first ) / 2;
        if( /*x <= a[mid]*/ CmpFunc( mid ) <= 0 ) {
            last = mid;
        }
        else {
            first = mid + 1;
        }
    }
    return ( (
#ifndef SEARCH_CHECK_BOUNDS
                 /*last < n*/ ( last < arrayLen ) &&
#endif
                 /*a[last] == x*/ ( CmpFunc( last ) == 0 ) ) ?
                 last :
                 -1 );
}
