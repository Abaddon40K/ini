set(INI_HDR
    ini.h
)

set(INI_SRC
    ini.cpp
    mmap.h
    mmap.cpp 
)

set(MY_COMPILE_FLAGS )
set(MY_LINK_FLAGS )

list(APPEND INI_SRC ${INI_HDR})


add_library(ini_object OBJECT)
target_sources(ini_object PRIVATE ${INI_SRC})

if(BUILD_RELEASE)
    if(WIN32)
        #list(APPEND MY_COMPILE_FLAGS "/O2") 
    elseif(UNIX)
        list(APPEND MY_COMPILE_FLAGS -O3)
    endif()
else() # DEBUG BUILD
    if(WIN32)
        #list(APPEND MY_COMPILE_FLAGS "/Zi /MDd")
    elseif(UNIX)
        list(APPEND MY_COMPILE_FLAGS
            -fPIC
            -Og
            -g3
        )
    endif()
endif()

if(BUILD_WITH_ASAN AND UNIX)
    set(ASAN_FLAGS
        -fsanitize=address,undefined,leak,signed-integer-overflow,pointer-subtract,pointer-compare
        -fsanitize-address-use-after-scope
    )
    list(APPEND MY_COMPILE_FLAGS
        -fPIC
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
        ${ASAN_FLAGS}
    )
    list(APPEND MY_LINK_FLAGS ${ASAN_FLAGS})
endif()

add_library(ini_shared SHARED $<TARGET_OBJECTS:ini_object>)
add_library(ini::shared ALIAS ini_shared)

add_library(ini_static STATIC $<TARGET_OBJECTS:ini_object>)
add_library(ini::static ALIAS ini_static)

add_executable(ini.exe main.cpp)

foreach(i ini_object ini_static ini_shared)
    set_target_properties(${i} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION}
        POSITION_INDEPENDENT_CODE ON
    )
    if(UNIX)
        set_target_properties(${i} PROPERTIES OUTPUT_NAME ini)
    endif()
    target_include_directories(${i} PUBLIC $<BUILD_INTERFACE:${INI_ROOT_DIR}>)
    target_compile_features(${i} PUBLIC cxx_std_17)
    target_compile_options(${i} PUBLIC ${MY_COMPILE_FLAGS})
    target_link_options(${i} PUBLIC ${MY_LINK_FLAGS})
endforeach()

if(WIN32)
    set_target_properties(ini_static PROPERTIES OUTPUT_NAME ini)
endif()

if(UNIX)
    set_target_properties(ini.exe PROPERTIES OUTPUT_NAME ini)
endif()


target_compile_features(ini.exe PUBLIC cxx_std_17)
target_link_libraries(ini.exe PRIVATE ini::static ${MY_LINK_FLAGS})
target_compile_options(ini.exe PUBLIC ${MY_COMPILE_FLAGS})
target_include_directories(ini.exe PUBLIC ${INI_ROOT_DIR})

message(STATUS "MY_LINK_FLAGS: ${MY_LINK_FLAGS}")
message(STATUS "MY_COMPILE_FLAGS: ${MY_COMPILE_FLAGS}")

include(GNUInstallDirs)


# install lib
install(
    TARGETS ini_shared ini_static
    EXPORT ini_Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# install lib headers
install(
    FILES ${INI_HDR}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# create ${PROJECT_NAME}Targets.cmake
install(EXPORT ini_Targets
    FILE iniTargets.cmake
    NAMESPACE ini::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/ini
)

# create ${PROJECT_NAME}ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    iniConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# create ${PROJECT_NAME}Config.cmake file
configure_package_config_file(
    "${INI_ROOT_DIR}/cmake/iniConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/iniConfig.cmake"
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_DATAROOTDIR}/cmake/ini
)

# create pkgconfig file
configure_file(
    "${INI_ROOT_DIR}/cmake/ini.pc.in"
    "${PROJECT_BINARY_DIR}/ini.pc"
    @ONLY
)
# install cmake find files
install(
    FILES
    ${CMAKE_BINARY_DIR}/iniConfig.cmake
    ${CMAKE_BINARY_DIR}/ini/iniConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/ini
)
# install pkgconfig file
install(
    FILES ${CMAKE_BINARY_DIR}/ini.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig
)

# install binary file
install(
    TARGETS ini.exe
    EXPORT ini.exe
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


if(BUILD_TESTS)
    enable_testing()
    include(CTest)
    add_subdirectory(tests)
endif()
