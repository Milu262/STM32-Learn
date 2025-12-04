cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(STM32F407_LCKFB)
enable_language(ASM)
# 交叉编译配置
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

# CPU 和编译选项
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(CMAKE_C_FLAGS "${CPU_FLAGS} -Wall -Wextra -O0 -g3 -std=c99 -ffunction-sections -fdata-sections -fno-builtin")
set(CMAKE_CXX_FLAGS "${CPU_FLAGS} -Wall -Wextra -O0 -g3 -fno-exceptions -ffunction-sections -fdata-sections -fno-builtin")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp -g3")

# 链接脚本
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/stm32f407vetx_flash.ld")
set(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT} -Wl,--gc-sections -Wl,-Map=${PROJECT_NAME}.map")

# 源文件收集（必须在 add_executable 之前）
file(GLOB_RECURSE SRC_FILES
  ${CMAKE_SOURCE_DIR}/app/*.c
  ${CMAKE_SOURCE_DIR}/board/*.c
  ${CMAKE_SOURCE_DIR}/module/*.c
  ${CMAKE_SOURCE_DIR}/bsp/I2C/*.c
  ${CMAKE_SOURCE_DIR}/bsp/Response/*.c
  ${CMAKE_SOURCE_DIR}/bsp/SPI/*.c
  ${CMAKE_SOURCE_DIR}/bsp/uart/*.c
  ${CMAKE_SOURCE_DIR}/bsp/DMA/*.c
  ${CMAKE_SOURCE_DIR}/bsp/NVIC/*.c
  ${CMAKE_SOURCE_DIR}/PeripheralModule/**/*.c 
  ${CMAKE_SOURCE_DIR}/protocol/*.c
  ${CMAKE_SOURCE_DIR}/libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
  ${CMAKE_SOURCE_DIR}/libraries/STM32F4xx_StdPeriph_Driver/src/*.c
)

# 启动文件
set(STARTUP_FILE ${CMAKE_SOURCE_DIR}/libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/arm/startup_stm32f407xx.s)
list(APPEND SRC_FILES ${STARTUP_FILE})

# 排除不需要的文件
list(FILTER SRC_FILES EXCLUDE REGEX ".*bsp_spi_flash.c$")
list(FILTER SRC_FILES EXCLUDE REGEX ".*Test_code.c$")
list(FILTER SRC_FILES EXCLUDE REGEX ".*stm32f4xx_fmc\\.c$")

# ✅ 第一步：创建目标
add_executable(${PROJECT_NAME}.elf ${SRC_FILES})

# ✅ 第二步：设置目标属性（顺序不能错！）

# 头文件路径（现代写法）
target_include_directories(${PROJECT_NAME}.elf PRIVATE
  ${CMAKE_SOURCE_DIR}/app
  ${CMAKE_SOURCE_DIR}/board
  ${CMAKE_SOURCE_DIR}/bsp
  ${CMAKE_SOURCE_DIR}/bsp/uart
  ${CMAKE_SOURCE_DIR}/bsp/I2C
  ${CMAKE_SOURCE_DIR}/bsp/Response
  ${CMAKE_SOURCE_DIR}/bsp/SPI
  ${CMAKE_SOURCE_DIR}/bsp/DMA
  ${CMAKE_SOURCE_DIR}/bsp/NVIC
  ${CMAKE_SOURCE_DIR}/bsp/DCMI
  ${CMAKE_SOURCE_DIR}/libraries/CMSIS/Include
  ${CMAKE_SOURCE_DIR}/libraries/CMSIS/Device/ST/STM32F4xx/Include
  ${CMAKE_SOURCE_DIR}/libraries/STM32F4xx_StdPeriph_Driver/inc
  ${CMAKE_SOURCE_DIR}/module
  ${CMAKE_SOURCE_DIR}/PeripheralModule/SPI_Screen
  ${CMAKE_SOURCE_DIR}/PeripheralModule/OV2640
  ${CMAKE_SOURCE_DIR}/protocol
)

# === ✅ 关键修改：自动检测 newlib 头文件路径（用于 clangd）===
find_program(ARM_GCC arm-none-eabi-gcc REQUIRED)

execute_process(
    COMMAND ${ARM_GCC} -print-sysroot
    OUTPUT_VARIABLE NEWLIB_SYSROOT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(NEWLIB_INCLUDE_DIR "${NEWLIB_SYSROOT}/include")

if(EXISTS "${NEWLIB_INCLUDE_DIR}/string.h")
    target_include_directories(${PROJECT_NAME}.elf SYSTEM PRIVATE "${NEWLIB_INCLUDE_DIR}")
    message(STATUS "✅ Using newlib headers from: ${NEWLIB_INCLUDE_DIR}")
else()
    message(WARNING "❌ newlib headers not found at ${NEWLIB_INCLUDE_DIR}. IDE may show false errors.")
endif()

message(STATUS "Source files:")
foreach(src ${SRC_FILES})
    message(STATUS "  ${src}")
endforeach()

# 宏定义
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
  STM32F40_41xxx
  USE_STDPERIPH_DRIVER
  HSE_VALUE=25000000
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
  -specs=nosys.specs
  -specs=nano.specs
  -lc
  -lm
)

# 生成 bin/hex
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
  COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
  COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
  COMMENT "Generating binary and hex files"
)