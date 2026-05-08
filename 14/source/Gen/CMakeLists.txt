add_library(Gen STATIC
    src/Rand.cpp
)

target_include_directories(Gen
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Gen PRIVATE project_warnings)

if(TARGET project_sanitizers)
    target_link_libraries(Gen PRIVATE project_sanitizers)
endif()

target_link_libraries(rialto PRIVATE 
    sleef::sleef
)

set_target_properties(Gen PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)