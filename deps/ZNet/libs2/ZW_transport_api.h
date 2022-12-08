/****************************************************************************
 *
 * Copyright (c) 2001-2013
 * Sigma Designs, Inc.
 * All Rights Reserved
 *
 *---------------------------------------------------------------------------
 *
 * Description: Z-Wave Transport Application layer interface
 *
 * Author:   Ivar Jeppesen
 *
 * Last Changed By:  $Author: jbu $
 * Revision:         $Revision: 29557 $
 * Last Changed:     $Date: 2014-09-30 15:25:45 +0200 (Tue, 30 Sep 2014) $
 *
 ****************************************************************************/
/**
 * \file ZW_transport_api.h
 * \brief Z-Wave Transport Application layer interface.
 *
 * The Z Wave transport layer controls transfer of data between Z Wave nodes
 * including retransmission, frame check and acknowledgement. The Z Wave
 * transport interface includes functions for transfer of data to other Z Wave
 * nodes. Application data received from other nodes is handed over to the
 * application via the \ref ApplicationCommandHandler function. The ZW_MAX_NODES
 * define defines the maximum of nodes possible in a Z Wave network.
 */
#ifndef _ZW_TRANSPORT_API_H_
#define _ZW_TRANSPORT_API_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "znet_fc_private.h"

/**
 * A response route is locked by the application
 */
#define RECEIVE_STATUS_ROUTED_BUSY 0x01
/**
 * Received at low output power level, this must
 * have the same value as TRANSMIT_OPTION_LOW_POWER
 */
#define RECEIVE_STATUS_LOW_POWER 0x02
/**
 * Mask for masking out the received frametype bits
 */
#define RECEIVE_STATUS_TYPE_MASK 0x0C
/**
 * Received frame is singlecast frame (rxOptions == xxxx00xx)
 */
#define RECEIVE_STATUS_TYPE_SINGLE 0x00
/**
 * Received frame is broadcast frame  (rxOptions == xxxx01xx)
 */
#define RECEIVE_STATUS_TYPE_BROAD 0x04
/**
 * Received frame is multicast frame (rxOptions == xxxx10xx)
 * Only TYPE_BROAD can be active at the same time as TYPE_EXPLORE
 */
#define RECEIVE_STATUS_TYPE_MULTI 0x08
/**
 * Received frame is an explore frame (rxOptions == xxx1xxxx)
 * Only TYPE_BROAD can be active at the same time as TYPE_EXPLORE
 */
#define RECEIVE_STATUS_TYPE_EXPLORE 0x10
/**
 * Received frame is not send to me (useful only in promiscuous mode)
 */
#define RECEIVE_STATUS_FOREIGN_FRAME 0x40

/* Max hops in route */
#define MAX_REPEATERS 4

typedef struct {
    BYTE bRepeaters; /* Repeaters in route, zero for direct range */
    /* rssi_values per hop for direct and routed frames.
     * Contains repeaters + 1 value pairs. Each pair contains an
     * incoming and an outgoing rssi value. */
    uint8_t rssi_values[MAX_REPEATERS + 1];
    uint16_t transmitTicks;
    uint8_t ackChannel;
    uint8_t lastTxChannel;
    uint8_t routeSchemeState;
    uint8_t repeaters[4];
    uint8_t routeSpeed;
    uint8_t routeTries;
    uint8_t lastFailedLinkFrom;
    uint8_t lastFailedLinkTo;
} TX_STATUS_TYPE;

/* Max number of nodes in a Z-wave system */
#define ZW_MAX_NODES 232

#endif /* _ZW_TRANSPORT_API_H_ */
