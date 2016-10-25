# ADD_MODULE
function(ADD_MODULE)
  set(OV NODE XWALK)
  set(SV JSAPI)
  set(MV SRCS INCLUDES DEPENDS)
  cmake_parse_arguments(ADD_MODULE "${OV}" "${SV}" "${MV}" ${ARGN})
  set(_TARGET ${ARGV0})
  if(ADD_MODULE_XWALK)
    add_custom_command(
      OUTPUT __generated_api.cc
      COMMAND python ${PROJECT_SOURCE_DIR}/build/generate_api.py
                ${CMAKE_CURRENT_SOURCE_DIR}/${ADD_MODULE_JSAPI}
                kSourceJSAPI __generated_api.cc
      DEPENDS ${ADD_MODULE_JSAPI})
    pkg_check_modules(
      ${_TARGET}_DEPS REQUIRED xwalk-extensions-common ${ADD_MODULE_DEPENDS})
    include_directories(${ADD_MODULE_INCLUDES} ${${_TARGET}_DEPS_INCLUDE_DIRS})
    add_library(${_TARGET} MODULE ${ADD_MODULE_SRCS} __generated_api.cc)
    set_target_properties(${_TARGET} PROPERTIES PREFIX "" SUFFIX ".xwalk")
    target_link_libraries(${_TARGET} ${${_TARGET}_DEPS_LIBRARIES})
  endif(ADD_MODULE_XWALK)
  if(ADD_MODULE_NODE)
    pkg_check_modules(
      ${_TARGET}_DEPS REQUIRED nodejs ${ADD_MODULE_DEPENDS})
    include_directories(${ADD_MODULE_INCLUDES} ${${_TARGET}_DEPS_INCLUDE_DIRS})
    add_library(${_TARGET} MODULE ${ADD_MODULE_SRCS})
    set_target_properties(${_TARGET} PROPERTIES PREFIX "" SUFFIX ".node")
    target_link_libraries(${_TARGET} ${${_TARGET}_DEPS_LIBRARIES})
  endif(ADD_MODULE_NODE)
endfunction()
