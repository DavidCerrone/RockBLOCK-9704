# Zephyr module CMakeLists.txt for RockBLOCK 9704

if(CONFIG_IRIDIUM)

# Create iridium library
zephyr_library_named(iridium)

# Add include directories
zephyr_library_include_directories(
    ../src
    ../src/third_party/base64
    ../src/third_party/cJSON
    ../src/serial_presets/serial_zephyr
)

# Add source files from the library
zephyr_library_sources(
    ../src/rockblock_9704.c
    ../src/jspr.c
    ../src/jspr_command.c
    ../src/imt_queue.c
    ../src/third_party/base64/base64.c
    ../src/third_party/cJSON/cJSON.c
    ../src/serial_presets/serial_zephyr/serial_zephyr.c
)

# Add Zephyr-specific crossplatform support
zephyr_library_sources(
    crossplatform_zephyr.c
)

# Conditionally add Kermit support
if(CONFIG_IRIDIUM_ENABLE_KERMIT)
    zephyr_library_sources(
        ../src/kermit/kermit_io.c
        ../src/third_party/ekermit/kermit.c
    )
    zephyr_library_compile_definitions(KERMIT)
endif()

# Conditionally add GPIO support
if(CONFIG_IRIDIUM_ENABLE_GPIO)
    zephyr_library_sources(
        gpio_zephyr.c
    )
    zephyr_library_compile_definitions(RB_GPIO)
endif()

# Add compile definitions
zephyr_library_compile_definitions(
    IMT_QUEUE_SIZE=${CONFIG_IRIDIUM_QUEUE_SIZE}
    __ZEPHYR__
)

endif() # CONFIG_IRIDIUM
