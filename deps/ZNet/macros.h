#ifndef __MACROS_H__
#define __MACROS_H__

#define MAX( first, second ) ( ( ( first ) > ( second ) ) ? ( first ) : ( second ) )
#define MIN( first, second ) ( ( ( first ) < ( second ) ) ? ( first ) : ( second ) )

#define ARRAY_LEN( arr ) ( sizeof( arr ) / sizeof( ( arr )[0] ) )

#endif  // #ifndef __MACROS_H__
