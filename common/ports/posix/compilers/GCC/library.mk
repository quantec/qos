
QOS_COMMON_PORTS_ARMCMX_GCC_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

LDFLAGS += -T$(QOS_COMMON_PORTS_ARMCMX_GCC_DIR)/sections.ld
