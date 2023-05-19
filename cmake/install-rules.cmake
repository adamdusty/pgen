install(
    TARGETS pgen_exe
    RUNTIME COMPONENT pgen_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()