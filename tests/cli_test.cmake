execute_process(COMMAND "${EXECUTABLE}" "${INPUT}"
  RESULT_VARIABLE stdout_result OUTPUT_VARIABLE expected ERROR_VARIABLE error)
if(NOT stdout_result EQUAL 0)
  message(FATAL_ERROR "Printing combined YAML failed: ${error}")
endif()
if(expected MATCHES "!include" OR NOT expected MATCHES "username: example")
  message(FATAL_ERROR "Output does not contain resolved nested includes")
endif()

file(WRITE "${OUTPUT}" "${expected}")
execute_process(COMMAND "${EXECUTABLE}" "${OUTPUT}"
  RESULT_VARIABLE parse_result OUTPUT_VARIABLE parsed ERROR_VARIABLE error)
file(REMOVE "${OUTPUT}")
if(NOT parse_result EQUAL 0 OR NOT parsed STREQUAL expected)
  message(FATAL_ERROR "Output is not valid round-trip YAML: ${error}")
endif()

execute_process(COMMAND "${EXECUTABLE}" "${INPUT}" extra
  RESULT_VARIABLE usage_result ERROR_VARIABLE error)
if(usage_result EQUAL 0 OR NOT error MATCHES "Usage:")
  message(FATAL_ERROR "Unexpected arguments were not rejected")
endif()
