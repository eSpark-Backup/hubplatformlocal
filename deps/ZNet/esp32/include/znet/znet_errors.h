/**
 * @file znet_error.h
 * @date 04 July 2018
 * @brief Defines errors
 *
 *
 */

#ifndef ZNET_ERRORS_H
#define ZNET_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int znet_error_t;

#define ZNET_ERR_REQUEST_SENT_OK ( 1 )       /* No error, request sent */
#define ZNET_ERR_OK ( 0 )                    /* No error */
#define ZNET_ERR_FAULT ( -1 )                /* General error */
#define ZNET_ERR_INVAL ( -2 )                /* Invalid argument */
#define ZNET_ERR_NOMEM ( -3 )                /* Bad alloc */
#define ZNET_ERR_TIME ( -4 )                 /* Timer expired */
#define ZNET_ERR_BUSY ( -5 )                 /* Resource busy */
#define ZNET_ERR_COMM ( -6 )                 /* Communication error on send */
#define ZNET_ERR_UNREACH ( -7 )              /* End point is unreachable */
#define ZNET_ERR_NOTFOUND ( -8 )             /* Not found */
#define ZNET_ERR_INPROGR ( -9 )              /* Cannot be completed immediately */
#define ZNET_ERR_UNSUPPORT ( -10 )           /* Not supported for current version CC */
#define ZNET_ERR_NETWORK_BUSY ( -11 )        /* Not possible to transmit data because network is busy */
#define ZNET_ERR_CALLBACK ( -12 )            /* Invalid callback */
#define ZNET_ERR_INV_RES ( -13 )             /* Invalid result */
#define ZNET_ERR_NOT_SIS ( -14 )             /* The controller is primary but not a SIS */
#define ZNET_ERR_REPORT_EXPIRED ( -15 )      /* Report waiting has expired */
#define ZNET_ERR_VALUES_GETTING ( -16 )      /* Finish values getting due to reported fail */
#define ZNET_ERR_MVALUES_GETTING ( -17 )     /* CommandClassValuesSetter: request value error */
#define ZNET_ERR_DISCOVER_REQ ( -18 )        /* DiscoverHandler, request node info fail */
#define ZNET_ERR_TIME_REQ ( -19 )            /* TimeManager, request failed, setup default node time diff */
#define ZNET_ERR_NIF_REQ ( -20 )             /* Could not request node info */
#define ZNET_ERR_NIF_TIME ( -21 )            /* Request of NIF has been timed out */
#define ZNET_ERR_NIF_INVAL ( -22 )           /* Invalid Node Info */
#define ZNET_ERR_NODE_EXISTS ( -23 )         /* Node already exists */
#define ZNET_ERR_NODE_NOT_FOUND ( -24 )      /* Could not find node info */
#define ZNET_ERR_INTERVIEW ( -25 )           /* Finalize interview on cc processing due to reported fail */
#define ZNET_ERR_SMART_SECURITY ( -26 )      /* Security bootstrapping of a Smart Start node has failed */
#define ZNET_ERR_BKP_OPEN ( -27 )            /* Cannot open backup session */
#define ZNET_ERR_BKP_OPEN_STATUS ( -28 )     /* Wrong status of opening backup session */
#define ZNET_ERR_BKP_CLOSE ( -29 )           /* Backup session closing failed */
#define ZNET_ERR_BKP_CLOSE_STATUS ( -30 )    /* Backup session closing wrong status */
#define ZNET_ERR_BKP_CLOSE_REQ ( -31 )       /* Close session request failed */
#define ZNET_ERR_BKP_READ ( -32 )            /* Read backup data failed */
#define ZNET_ERR_BKP_READ_STATUS ( -33 )     /* Wrong backup reading status */
#define ZNET_ERR_BKP_READ_REQ ( -34 )        /* Cannot start reading backup data */
#define ZNET_ERR_BKP_WRITE ( -35 )           /* Write backup data request failed */
#define ZNET_ERR_BKP_WRITE_STATUS ( -36 )    /* Wrong backup writing status */
#define ZNET_ERR_BKP_WRITE_REQ ( -37 )       /* Cannot send writing data request to chip */
#define ZNET_ERR_BKP_WRITE_EOF ( -38 )       /* Backup writing error, EOF expected but N bytes have been read */
#define ZNET_ERR_BKP_RST_OPEN ( -39 )        /* Open backup restoring session failed */
#define ZNET_ERR_BKP_RST_OPEN_STATUS ( -40 ) /* Wrong status on backup restoring session opening */
#define ZNET_ERR_BKP_RST_SET_DEFAULT ( -41 ) /* Cannot start backup restoring: set default raw failed */
#define ZNET_ERR_INCLUDE_PERM ( -42 )        /* Controller has no permissions to start include */

#ifdef __cplusplus
}
#endif

#endif  // ZNET_ERRORS_H
