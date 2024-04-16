install(
    TARGETS pgen
    RUNTIME COMPONENT pgen_Runtime
)

# Include CPack
if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()