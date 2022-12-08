# Zigbee Gateway stack 

This is a part of Silicon Labs Ember ZNet Zigbee stack v6.7.2.0

Gateway application used to work with NCP ( network co-processor ) firmware via
serial API interface and UART ( software flow control ). Before gateway started an
ncp-image must be flashed into ncp-chip. NCP images are located:
```
...Silabs/SimplicityStudio_v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/ncp-images
```
Files are presented in two formats:
  * *.ebl/*.gbl - update image in bootloader ( for update via bootloader )
  * *.s37 - full file image ( for flashing with debugger )
Not all debuggers support _*.s37_ ( Motorola ) binary format. For format conversion into
_intel hex_ format under linux you may use _srec_cat_ utility:
```
srec_cat ncp-uart-sw.s37 -Motorola -o ncp-uart-sw.hex -Intel
```


According to current workflow to build Zigbee gateway application you need:

  * generate and configure zigbee gateway application using SimplicityStudio and recent Ember ZNet stack
  * copy gateway and stack sources into hubPlatform build system
  * modify files - see below

Initially stack uses make build system.

To build under make you need to copy _project-specific.mak_ to _hubPlatform/deps/Zigbee/v2.7/app/builder/zigbeegw/_,
change folder to _.../zigbeegw/_ and run _make INCLUDE_MAKEFILE=project-specific.mak_. I assume all variables like CC,
LD, AR, etc are set ( review _Makefile_ if build fails ) 


To build under cmake some moves must be done:
  1. Modify hubPlatform/deps/Zigbee/CmakeLists.txt. You should rely on generated Makefile located at
  _hubPlatform/deps/Zigbee/v2.7/app/builder/zigbeegw/Makefile_ - update source files, include path's and defines
  involved in make build.
  2. Review variables mentioned in _DEFINES_ and update configuration source file _definitions.h.in_

According to hubPlatform services approach several fixes must be done with zigbee stack after it was generated:
  * ha-zigbeed uses global static variable _ncpNeedsResetAndInit_ from af-main-host.c file which already has getter
  function but lacks setter support

```
void emberAfNcpNeedsResetSet( bool value )
{
    ncpNeedsResetAndInit = value;
}
```
  * callback.h must be corrected for easier callback implementation in host application. Add _weak_ attribute to
  necessary callback definitions

```
change void emberAfMainTickCallback(void); to -> void emberAfMainTickCallback(void) __attribute__((weak));
```
There could meet possible build error:
```
ezsp-host-io.c:254:25: error: 'CSTART' undeclared (first use in this function); did you mean 'VSTART'?
     tios.c_cc[VSTART] = CSTART;           // define XON and XOFF
                         ^~~~~~
                         VSTART
```
  * can be fixed by adding explicit include of  header file _<sys/ttydefaults.h>_ in
  _hubPlatform/deps/Zigbee/v2.7/protocol/zigbee/app/ezsp-host/ezsp-host-io.c_

Sometimes happens that we need to make changes in the stack plugins all significant changes
are marked with comment _//hubPlatform_ or similar.