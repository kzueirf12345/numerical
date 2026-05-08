add_library(rialto STATIC
    src/rialto.cpp
)

target_include_directories(rialto
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rialto PRIVATE project_warnings)

if(TARGET project_sanitizers)
    target_link_libraries(rialto PRIVATE project_sanitizers)
endif()

target_link_libraries(rialto PRIVATE 
    sleef::sleef
    OpenMP::OpenMP_CXX 
    Threads::Threads 
    Gen
)

set_target_properties(rialto PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)