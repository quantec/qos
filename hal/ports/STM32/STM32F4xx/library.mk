
# ChibiOS default path if not specified
CHIBIOS_DIR ?= $(ROOT_DIR)/submodules/chibios
CHIBIOS := $(CHIBIOS_DIR)
CHIBIOS_CONTRIB := $(CHIBIOS)/community

QHAL_STM32F4XX_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

CSRC += $(wildcard $(QHAL_STM32F4XX_DIR)/*.c)
CSRC += $(wildcard $(QHAL_STM32F4XX_DIR)/../LLD/FLASHv2/*.c)
CSRC += $(wildcard $(QHAL_STM32F4XX_DIR)/../LLD/USARTv1/*.c)
CSRC += $(wildcard $(QHAL_STM32F4XX_DIR)/../LLD/RTCv2/*.c)

EXTRAINCDIRS += $(QHAL_STM32F4XX_DIR)
EXTRAINCDIRS += $(QHAL_STM32F4XX_DIR)/../LLD/FLASHv2
EXTRAINCDIRS += $(QHAL_STM32F4XX_DIR)/../LLD/USARTv1
EXTRAINCDIRS += $(QHAL_STM32F4XX_DIR)/../LLD/RTCv2

# ChibiOS
include $(CHIBIOS_DIR)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk
include $(CHIBIOS_DIR)/community/os/hal/ports/STM32/STM32F4xx/platform.mk

include $(QHAL_STM32F4XX_DIR)/../library.mk
