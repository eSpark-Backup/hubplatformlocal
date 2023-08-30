#ifndef __ZNET_RELEASE_H__
#define __ZNET_RELEASE_H__

/*
  if [ "$(SKIP_GIT_PREPARE)" != "yes" ]; then
    (git log
         --no-color
         --format='#define ZNET_RELEASE_DATE "%ad"'
         -1
         --date=iso;
     git log
         --no-color
         --format='#define ZNET_RELEASE_COMMIT "%H"'
         -1;
         echo "#define ZNET_RELEASE_VERSION \"`git describe --tags`\"") >
  ZRelease.h; fi
*/

#define ZNET_RELEASE_DATE "ZNET_RELEASE_DATE"
#define ZNET_RELEASE_COMMIT "ZNET_RELEASE_COMMIT"
#define ZNET_RELEASE_VERSION "ZNET_RELEASE_VERSION"

//////////////////////////////////////////////////////////////////////////////

#define LIB_VERSION_ID 1  // Deprecated, use API calls below

#ifdef __cplusplus
extern "C" {
#endif

const char* znetRevisionTag();

const char* znetRevisionId();

int znetVersion( char* buff, int bufSize );

const char* znetCommitId();

const char* znetCommitDate();

#ifdef __cplusplus
}
#endif

#endif  // #ifndef __ZNET_RELEASE_H__
