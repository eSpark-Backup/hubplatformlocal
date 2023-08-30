/*
 *  stub
 */

#ifndef __USER_INTERFACE_H__
#define __USER_INTERFACE_H__

#include <stdio.h>
#ifdef ESP_PLATFORM
#include <esp_system.h>
#else
#define system_get_free_heap_size() ( 0 )
#endif

#define os_printf printf

#endif
