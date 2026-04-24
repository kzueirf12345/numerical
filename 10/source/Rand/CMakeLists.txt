add_library(Rand STATIC
    src/Rand.cpp
)

target_include_directories(Rand
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Rand PRIVATE project_warnings)

if(TARGET project_sanitizers)
    target_link_libraries(Rand PRIVATE project_sanitizers)
endif()

set_target_properties(Rand PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)