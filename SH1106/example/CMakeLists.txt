# Deprecated for v7.x.x
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES driver sh1106
)

# idf_component_register(
#     SRCS "main.c"
#     INCLUDE_DIRS "."
#     REQUIRES esp_driver_i2c sh1106
# )
