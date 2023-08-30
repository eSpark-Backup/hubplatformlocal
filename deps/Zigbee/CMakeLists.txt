cmake_minimum_required( VERSION 3.5 )

message( STATUS "-- CMAKE is working in \"${CMAKE_CURRENT_SOURCE_DIR}\" " )
get_filename_component( PROJECT_ID ${CMAKE_CURRENT_SOURCE_DIR} NAME )
project ( "ember_znet" )

set( STACK_VERSION v2.7 )

set( PACKAGE_NAME firmware-common-${PROJECT_NAME} )

set( APP_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/app/builder/zigbeegw/znet-bookkeeping.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/app/builder/zigbeegw/call-command-handler.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/app/builder/zigbeegw/callback-stub.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/app/builder/zigbeegw/stack-handler-stub.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/app/builder/zigbeegw/znet-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/app/builder/zigbeegw/zigbeegw_callbacks.c
		)

set( STACK_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../util/common/library.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../util/serial/command-interpreter2.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../util/zigbee-framework/zigbee-device-common.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../util/zigbee-framework/zigbee-device-host.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../../stack/framework/event-control.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/generic/led-stub.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/generic/mem-util.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/plugin/antenna-stub/antenna-stub.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/plugin/buzzer-stub/buzzer-stub.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/address-table/address-table.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/address-table/address-table-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/basic/basic.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/concentrator/concentrator-support.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/concentrator/concentrator-support-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/counters/counters-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/counters/counters-ota-host.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/counters/counters-host.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/device-database/device-database.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/device-database/device-database-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/device-query-service/device-query-service.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/device-query-service/device-query-service-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/device-table/device-table-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/device-table/device-table.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/device-table/device-table-discovery.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/device-table/device-table-tracking.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/ezmode-commissioning/ez-mode.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/ezmode-commissioning/ez-mode-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/util/ezsp/ezsp-callbacks.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/util/ezsp/ezsp-enum-decode.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/util/ezsp/ezsp-frame-utilities.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/util/ezsp/ezsp.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/ezsp-host/ezsp-host-io.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/ezsp-host/ezsp-host-queues.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/ezsp-host/ezsp-host-ui.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/util/ezsp/serial-interface-uart.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/ezsp-host/ash/ash-host-ui.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/ezsp-host/ash/ash-host.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/generic/ash-common.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin-host/file-descriptor-dispatch/file-descriptor-dispatch.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/find-and-bind-target/find-and-bind-target.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/find-and-bind-target/find-and-bind-target-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/form-and-join/form-and-join-afv2.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/util/common/form-and-join.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/util/common/form-and-join-host-adapter.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin-host/gateway/gateway-support.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin-host/gateway/backchannel-support.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin-host/gateway/gateway-support-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/general-response-commands/general-response-commands.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/green-power-client/green-power-client.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/green-power-client/green-power-client-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/green-power-common/green-power-common.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/stack/gp/gp-util.c
    ${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/groups-client/groups-client.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/heartbeat/heartbeat.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/ias-zone-client/ias-zone-client.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/ias-zone-client/ias-zone-client-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/identify/identify.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/identify/identify-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/identify-feedback/identify-feedback.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/plugin/plugin-common/linked-list/linked-list.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/aes.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/aesni.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/arc4.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/asn1parse.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/asn1write.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/base64.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/bignum.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/blowfish.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/camellia.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ccm.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/certs.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/cipher.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/cipher_wrap.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/cmac.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ctr_drbg.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/debug.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/des.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/dhm.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ecdh.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ecdsa.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ecjpake.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ecp.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ecp_curves.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/entropy.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/entropy_poll.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/error.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/gcm.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/havege.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/hmac_drbg.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/md.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/md2.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/md4.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/md5.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/md_wrap.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/memory_buffer_alloc.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/net_sockets.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/oid.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/padlock.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/pem.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/pk.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/pk_wrap.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/pkcs11.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/pkcs12.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/pkcs5.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/pkparse.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/pkwrite.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ripemd160.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/rsa.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/rsa_internal.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/sha1.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/sha256.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/sha512.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ssl_cache.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ssl_ciphersuites.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ssl_cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ssl_cookie.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ssl_srv.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ssl_ticket.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/ssl_tls.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/threading.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/timing.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/version.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/version_features.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/x509.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/x509_create.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/x509_crl.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/x509_crt.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/x509_csr.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/x509write_crt.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/x509write_csr.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/library/xtea.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin-host/ncp-configuration/ncp-configuration.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/network-creator/network-creator.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/network-creator/network-creator-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/network-creator-security/network-creator-security.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/network-creator-security/network-creator-security-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/network-steering/network-steering.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/network-steering/network-steering-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/network-steering/network-steering-v2.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/network-steering/network-steering-host.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/poll-control-client/poll-control-client.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/poll-control-client/poll-control-client-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/reporting/reporting.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/reporting/reporting-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/reporting/reporting-default-configuration.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/scan-dispatch/scan-dispatch.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/scenes-client/scenes-client.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/util/ezsp/secure-ezsp-stub.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/simple-metering-client/simple-metering-client.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/simple-metering-client/simple-metering-client-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/stack-diagnostics/stack-diagnostics.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/test-harness/test-harness.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/test-harness/read-write-attributes.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/test-harness/test-harness-host.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/time-server/time-server.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/trust-center-nwk-key-update-broadcast/trust-center-nwk-key-update-broadcast.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/trust-center-nwk-key-update-periodic/trust-center-nwk-key-update-periodic.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/trust-center-nwk-key-update-unicast/trust-center-nwk-key-update-unicast.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/generic/crc.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/generic/endian.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/generic/random.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/generic/system-timer.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/unix/host/micro.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/unix/host/token-def-unix.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/unix/host/token.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/plugin/serial/ember-printf-convert.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/util/serial/linux-serial.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/update-ha-tc-link-key/update-ha-tc-link-key.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/update-tc-link-key/update-tc-link-key.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/update-tc-link-key/update-tc-link-key-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/xmodem-sender/xmodem-sender.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/cli/core-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/cli/network-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/cli/option-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/cli/plugin-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/cli/security-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/cli/zcl-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/cli/zdo-cli.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/security/af-node.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/security/af-security-common.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/security/af-trust-center.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/security/crypto-state.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/af-event.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/af-main-common.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/attribute-size.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/attribute-storage.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/attribute-table.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/client-api.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/message.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/multi-network.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/print.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/print-formatter.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/process-cluster-message.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/process-global-message.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/service-discovery-common.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/time-util.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/util.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/af-main-host.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/service-discovery-host.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/zll-identify-server/zll-identify-server.c
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/plugin/zll-scenes-client/zll-scenes-client.c
	)

set( SRC_FILES
	${APP_SRC_FILES}
	${STACK_SRC_FILES}
	)


set( LIBRARIES "" )

if( NOT DEFINED ARCHIVE_EXTENSION )
    set( ARCHIVE_EXTENSION ".a")
endif()

include_directories(
	${COMPILER_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../..
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../../stack
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../util
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../util/common
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../util/ezsp
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../util/serial
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/../util/zigbee-framework
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/cli
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/include
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/security
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/app/builder/zigbeegw
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/plugin
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/..
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/generic
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/unix/host
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/unix/host/board
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/plugin/plugin-common/mbedtls
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/include
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/include/mbedtls
	${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/util/third_party/mbedtls/sl_crypto/include
)

set( APP_BUILDER_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/app/builder/zigbeegw )
set( APP_BUILDER_CONFIG_HEADER ${APP_BUILDER_OUTPUT_DIRECTORY}/zigbeegw.h )
set( APP_BUILDER_STORAGE_FILE ${APP_BUILDER_OUTPUT_DIRECTORY}/zigbeegw_endpoint_config.h )
set( OUTPUT_DIR ${APP_BUILDER_OUTPUT_DIRECTORY}/build )
set( OUTPUT_DIR_CREATED ${OUTPUT_DIR}/created )
set( EXE_DIR ${OUTPUT_DIR}/exe )

if( NOT DEFINED PLATFORM_HEADER_FILE )
    set( PLATFORM_HEADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/platform/base/hal/micro/unix/compiler/gcc.h )
endif()

set( DEFINES "${COMPILER_DEFINES} \
-DUNIX \
-DUNIX_HOST \
-DPHY_NULL \
-DCONFIGURATION_HEADER=\\\"${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/config.h\\\" \
-DEZSP_HOST \
-DGATEWAY_APP \
-DZA_GENERATED_HEADER=\\\"${APP_BUILDER_CONFIG_HEADER}\\\" \
-DATTRIBUTE_STORAGE_CONFIGURATION=\\\"${APP_BUILDER_STORAGE_FILE}\\\" \
-DPLATFORM_HEADER=\\\"${PLATFORM_HEADER_FILE}\\\" \
-DBOARD_HOST \
-DBOARD_HEADER=\\\"${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/app/builder/zigbeegw/zigbeegw_board.h\\\" \
-DEM_AF_TEST_HARNESS_CODE \
-DEM_AF_LINK_M \
-DEM_AF_LINK_PTHREAD \
-DEMBER_AF_API_EMBER_TYPES=\\\"${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/stack/include/ember-types.h\\\" \
-DEMBER_AF_API_DEBUG_PRINT=\\\"${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/util/print.h\\\" \
-DEMBER_AF_API_AF_HEADER=\\\"${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/include/af.h\\\" \
-DEMBER_AF_API_AF_SECURITY_HEADER=\\\"${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/app/framework/security/af-security.h\\\" \
-DEMBER_AF_API_NEIGHBOR_HEADER=\\\"${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/protocol/zigbee/stack/include/stack-info.h\\\" \
-DEMBER_STACK_ZIGBEE \
-DEZSP_ASH \
-DNO_READLINE \
-DMBEDTLS_CONFIG_FILE=\\\"${CMAKE_CURRENT_SOURCE_DIR}/${STACK_VERSION}/app/builder/zigbeegw/mbedtls-config-generated.h\\\" \
-DEMBER_COMMAND_INTEPRETER_HAS_DESCRIPTION_FIELD"
		)

if( CMAKE_BUILD_TYPE MATCHES Debug )
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_ENABLE=1")
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_DEBUG=1")
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_CORE=1")
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_ON_OFF_CLUSTER=1")
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_BASIC_CLUSTER=1")
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_IAS_ZONE_CLUSTER=1")
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_SERVICE_DISCOVERY=1")
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_REGISTRATION=1")
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_ZDO=1")
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_ATTRIBUTES=1")
	set( DEBUG_DEFINES "${DEBUG_DEFINES} -DEMBER_AF_PRINT_REPORTING=1")
endif()

set( CMAKE_C_FLAGS_RELEASE "" )
set( CMAKE_C_FLAGS_MINSIZEREL "" )
set( CMAKE_C_FLAGS "${COMPILER_FLAGS} ${DEPENDENCY_FLAGS} ${DEFINES} ${DEBUG_DEFINES} -Wall -ggdb -O0" )
set( CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS}" )

if( NOT DEFINED STD )
	set( CMAKE_C_STANDARD 99 )
else()
	set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=${STD}" )
endif()

if( NOT "${DEFINES}" MATCHES "-DNO_READLINE" )
	set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lreadline -lncurses" )
	set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lreadline -lncurses" )
endif()

# Conditionally include the math library if EM_AF_LINK_M is defined.
if( "${DEFINES}" MATCHES "-DEM_AF_LINK_M" )
	set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm" )
endif()

# Conditionally include the POSIX threads library if EM_AF_LINK_PTHREAD is defined.
if( ${DEFINES} MATCHES "-DEM_AF_LINK_PTHREAD" )
	set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread" )
endif()

message( STATUS "-- CMAKE is working on \"${PROJECT_NAME}\" " )
message( STATUS "${COLOR_VAR_NAME}CMAKE_C_STANDARD${COLOR_DOTS}.............${COLOR_VAR_VAL}${CMAKE_C_STANDARD}${COLOR_RESET}" )
message( STATUS "${COLOR_VAR_NAME}CMAKE_C_FLAGS${COLOR_DOTS}................${COLOR_VAR_VAL}${CMAKE_C_FLAGS}${COLOR_RESET}" )
message( STATUS "The ${COLOR_VAR_NAME}CMAKE_C_FLAGS_RELEASE${COLOR_RESET} ----> ${COLOR_VAR_VAL}${CMAKE_C_FLAGS_RELEASE}${COLOR_RESET}" )
message( STATUS "The ${COLOR_VAR_NAME}CMAKE_C_FLAGS_DEBUG${COLOR_RESET} ----> ${COLOR_VAR_VAL}${CMAKE_C_FLAGS_DEBUG}${COLOR_RESET}" )
message( STATUS "The ${COLOR_VAR_NAME}CMAKE_C_FLAGS_MINSIZEREL${COLOR_RESET} ----> ${COLOR_VAR_VAL}${CMAKE_C_FLAGS_MINSIZEREL}${COLOR_RESET}" )
message( STATUS "${COLOR_VAR_NAME}CMAKE_EXE_LINKER_FLAGS${COLOR_DOTS}.......${COLOR_VAR_VAL}${CMAKE_EXE_LINKER_FLAGS}${COLOR_RESET}" )

get_property( dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES )
message( STATUS "${COLOR_VAR_NAME}INCLUDE_DIRECTORIES${COLOR_DOTS}..........${COLOR_VAR_VAL}${dirs}${COLOR_RESET}" )

add_library( ${PROJECT_NAME} ${SRC_FILES} )

CONFIGURE_ARCH_DEPENDENT_FILES( CURRENT_GENERATED_DIR definitions.h )
target_include_directories(${PROJECT_NAME} PRIVATE ${CURRENT_GENERATED_DIR})

message( STATUS "${COLOR_VAR_NAME}CURRENT_GENERATED_DIR${COLOR_DOTS}...... .${COLOR_VAR_VAL}${CURRENT_GENERATED_DIR}${COLOR_RESET}" )

install( TARGETS ${PROJECT_NAME} DESTINATION ${PACKAGE_NAME}/opt/firmware/lib )
