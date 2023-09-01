#ifndef __STATES_H__
#define __STATES_H__

//////////////////////////////////////////////////////////////////////////////

#include "typedefs.h"
#include "system_time.h"

//////////////////////////////////////////////////////////////////////////////

typedef SystemTime_t StateContextTime_t;
typedef uint8_t StateContextResult_t;

struct StateContext_s;
typedef struct StateContext_s* StateContext;

typedef void ( *StateContext_FreeFunc )( StateContext context );

#define STATES_CONTEXT_FIELDS       \
    void* _label;                   \
    StateContextResult_t _res;      \
    StateContextTime_t _finishTime; \
    StateContext _nested;           \
    StateContext_FreeFunc _freeFunc

typedef struct StateContext_s {
    STATES_CONTEXT_FIELDS;

} StateContext_t;
// typedef StateContext_t* StateContext;

//////////////////////////////////////////////////////////////////////////////

#define STATES_CONTEXT_ARG StateContext ___stateContext___
#define STATES_CONTEXT ___stateContext___

#define DECLARE_STATES_CONTEXT_FUNC( func, ... ) \
    StateContextResult_t ICACHE_FLASH_ATTR func( STATES_CONTEXT_ARG, __VA_ARGS__ )

//---------------------------------------------------------------------------
#define STATES_CONTEXT_RESET( c ) \
    do {                          \
        ( c )->_label = NULL;     \
        ( c )->_res = 0;          \
        ( c )->_finishTime = 0;   \
        ( c )->_nested = NULL;    \
        ( c )->_freeFunc = NULL;  \
    } while( 0 )

//---------------------------------------------------------------------------
#define STATES_BEGIN                       \
    do {                                   \
        if( STATES_CONTEXT->_label ) {     \
            goto * STATES_CONTEXT->_label; \
        }                                  \
    } while( 0 )

//---------------------------------------------------------------------------
#define STATES_END
///
///    STATES_RESET

//---------------------------------------------------------------------------
#define STATES_STATE                    \
    do {                                \
        __label__ _L_;                  \
        STATES_CONTEXT->_label = &&_L_; \
    _L_:;                               \
    } while( 0 )

//---------------------------------------------------------------------------
#define STATES_RESET STATES_CONTEXT->_label = NULL

//---------------------------------------------------------------------------
#define STATES_WAIT( condition )        \
    do {                                \
        __label__ _L_;                  \
        STATES_CONTEXT->_label = &&_L_; \
    _L_:                                \
        if( !( condition ) ) {          \
            return 1;                   \
        }                               \
    } while( 0 )

//---------------------------------------------------------------------------
#define STATES_WAIT_BLOCK( condition ) \
    do {                               \
        while( !( condition ) ) {      \
        }                              \
    } while( 0 )

//---------------------------------------------------------------------------
#define STATES_WAIT_TIMED( condition, time, _L_timeout_ )           \
    do {                                                            \
        __label__ _L_;                                              \
        STATES_CONTEXT->_finishTime = SystemTime_Now() + ( time );  \
        STATES_CONTEXT->_label = &&_L_;                             \
    _L_:                                                            \
        if( !( condition ) ) {                                      \
            if( SystemTime_Now() >= STATES_CONTEXT->_finishTime ) { \
                goto _L_timeout_;                                   \
            }                                                       \
            return 1;                                               \
        }                                                           \
    } while( 0 )

//---------------------------------------------------------------------------
#define STATES_WAIT_TIMED_BLOCK( condition, time, _L_timeout_ )      \
    do {                                                             \
        StateContextTime_t finishTime = SystemTime_Now() + ( time ); \
        while( !( condition ) ) {                                    \
            if( SystemTime_Now() >= finishTime ) {                   \
                goto _L_timeout_;                                    \
            }                                                        \
        }                                                            \
    } while( 0 )

//---------------------------------------------------------------------------
#define STATES_DELAY( time )                                                \
    do {                                                                    \
        STATES_CONTEXT->_finishTime = SystemTime_Now() + ( time );          \
        STATES_WAIT( ( SystemTime_Now() >= STATES_CONTEXT->_finishTime ) ); \
    } while( 0 )

//---------------------------------------------------------------------------
#define STATES_DELAY_BLOCK( time )                                   \
    do {                                                             \
        StateContextTime_t finishTime = SystemTime_Now() + ( time ); \
        while( SystemTime_Now() < finishTime ) {                     \
        }                                                            \
    } while( 0 )

//---------------------------------------------------------------------------
// <  0 ---> goto ErrorLabel
// == 0 ---> continue to next
// >  0 ---> return result to continue next time of scheduler call
#define STATES_FUNC( _L_error_, func, ... )                            \
    do {                                                               \
        __label__ _L_;                                                 \
        STATES_CONTEXT->_label = &&_L_;                                \
    _L_:                                                               \
        if( ( STATES_CONTEXT->_res = ( func )( __VA_ARGS__ ) ) > 0 ) { \
            return STATES_CONTEXT->_res;                               \
        }                                                              \
        if( STATES_CONTEXT->_res < 0 ) {                               \
            goto _L_error_;                                            \
        }                                                              \
    } while( 0 )

//---------------------------------------------------------------------------
// <  0 ---> goto ErrorLabel
// == 0 ---> continue
// >  0 ---> loop
#define STATES_FUNC_BLOCK( _L_error_, func, ... )                                           \
    do {                                                                                    \
    _L_:                                                                                    \
                if( (STATES_CONTEXT->_res = (func)( __VA_ARGS__ ) > 0 ) { goto _L_      ; } \
		if(  STATES_CONTEXT->_res                         < 0 ) { goto _L_error_; }         \
    } while( 0 )

//---------------------------------------------------------------------------
#define STATES_CONTEXT_FUNC( _L_error_, func, ... )                                            \
    do {                                                                                       \
        if( STATES_CONTEXT->_nested == NULL ) {                                                \
            if( ( STATES_CONTEXT->_nested = func##_CreateContext() ) == NULL ) {               \
                STATES_CONTEXT->_res = Error_BadAllocation;                                    \
                goto _L_error_                                                                 \
            }                                                                                  \
        }                                                                                      \
        if( STATES_CONTEXT->_res = ( func )( STATES_CONTEXT->_nested, __VA_ARGS__ ) > 0 ) {    \
            return STATES_CONTEXT->_res;                                                       \
        }                                                                                      \
        if( STATES_CONTEXT->_nested->_freeFunc )                                               \
            ( *STATES_CONTEXT->_nested->_freeFunc )( STATES_CONTEXT->_nested ) else Heap_Free( \
                STATES_CONTEXT->_nested );                                                     \
        STATES_CONTEXT->_nested = NULL;                                                        \
        if( STATES_CONTEXT->_res < 0 ) {                                                       \
            goto _L_error_                                                                     \
        }                                                                                      \
    } while( 0 )

#endif  // #ifndef __STATES_H__
