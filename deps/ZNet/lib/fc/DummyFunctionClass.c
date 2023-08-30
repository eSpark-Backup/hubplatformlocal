
#include "typedefs.h"

#include "ZErrors.h"
#include "znet.h"
//

#include "znet_fc_private.h"

//----------------------------------------------------------------------------
const ZFunctionClass_t fcDummyFunctionClass = {
    0x00,  // _id
    "UNKNOWN",
    NULL,  // _Init
    NULL,  // _ProcessACK
    NULL,  // _ProcessRESPONSE
    NULL,  // _ProcessCALLBACK
    // NULL, // _ProcessTimeout
    NULL  // _IsCallbackFrameMine
    /*
        "UNKNOWN",
        "Unknown function class",
        NULL,
        NULL,
        NULL,
        NULL,
        //FALSE,
        //FALSE,
        //FALSE,
        //FALSE,
        //FALSE,
        //FALSE,
        //FALSE,
        //0.0f,
        //0.0f,
        //0.0f
    */
};
