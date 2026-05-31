# Deprecated for v7.x.x
idf_component_register(
    SRCS "src/sh1106.c"
    INCLUDE_DIRS "include"
    REQUIRES driver
)

# idf_component_register(
#     SRCS "src/sh1106.c"
#     INCLUDE_DIRS "include"
#     REQUIRES esp_driver_i2c
# )
