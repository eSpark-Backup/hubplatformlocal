BUILD_DIR=./build
VERSION=
COMPILE=
TESTS_FILTER=
TESTS_REGEX_FILTER=
TESTS_LOG_LEVEL=
TESTS_PLATFORM=
TESTS_HARDWARE_REVISION=

HUB_PLUGINS_DIR=$(PWD)

all: packages

packages: devices
	./utils/build_openwrt_packages.py --openwrt_build_dir="$(BUILD_DIR)/openwrt" --build=$(VERSION) $(if $(COMPILE),--compile_lua,)

clean:
	rm -rf "$(BUILD_DIR)"
	rm -rf gateways/zwave/scripts/model/devices/inclusion/generated
	rm -rf gateways/zigbee/scripts/devices/generated/
	rm -rf gateways/bluetooth/scripts/devices/generated/

devices:
	python3 gateways/dev_utils/device_generation.py

compile_tests_runner:
	mkdir -p "$(BUILD_DIR)/tests" && \
	cd "$(BUILD_DIR)/tests" && \
	cmake -DCMAKE_BUILD_TYPE=Release $(HUB_PLUGINS_DIR)/tests_runner && \
	$(MAKE) $(ARGS)

run_tests:
	"$(BUILD_DIR)"/tests/tests_runner \
		--filter=$(TESTS_FILTER) \
		--regex_filter=$(TESTS_REGEX_FILTER) \
		--log_level=$(TESTS_LOG_LEVEL) \
		--platform=$(TESTS_PLATFORM) \
		--hardware_revision=$(TESTS_HARDWARE_REVISION)

prepare_and_run_tests: devices compile_tests_runner run_tests


