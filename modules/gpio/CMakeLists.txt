SET(MODULE "gpio")

# Native Module
ADD_MODULE(${MODULE} NODE
  SRCS
    src/gpio.cc
  INCLUDES
    src
  DEPENDS
    capi-iotbus
)

# Copy Project
INSTALL(FILES package.json
  DESTINATION ${GLOBAL_NODE_MODULE_PATH}/${MODULE}
)

INSTALL(TARGETS ${MODULE}
  DESTINATION ${GLOBAL_NODE_MODULE_PATH}/${MODULE}/build/Release
)