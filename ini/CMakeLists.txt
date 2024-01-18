set(INI_HDR
    ini.h
)

set(INI_SRC
    ini.cpp
    mmap.h
    mmap.cpp
)

list(APPEND INI_SRC ${INI_HDR})

add_library(ini_object OBJECT)
target_sources(ini_object PRIVATE ${INI_SRC})

add_library(ini_shared SHARED $<TARGET_OBJECTS:ini_object>)
add_library(ini::shared ALIAS ini_shared)

add_library(ini_static STATIC $<TARGET_OBJECTS:ini_object>)
add_library(ini::static ALIAS ini_static)

target_include_directories(ini_object PUBLIC $<BUILD_INTERFACE:${INI_ROOT_DIR}>)
target_compile_features(ini_object PUBLIC cxx_std_17)

set_target_properties(ini_shared PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION}
    OUTPUT_NAME ini_shared
    SUFFIX ".dll"
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(ini_shared PUBLIC $<BUILD_INTERFACE:${INI_ROOT_DIR}>)
target_compile_features(ini_static PUBLIC cxx_std_17)

set_target_properties(ini_static PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION}
    OUTPUT_NAME ini_static
    SUFFIX ".lib"
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(ini_static PUBLIC $<BUILD_INTERFACE:${INI_ROOT_DIR}>)
target_compile_features(ini_static PUBLIC cxx_std_17)




add_executable(ini.exe main.cpp)
target_compile_features(ini.exe PUBLIC cxx_std_17)
target_link_libraries(ini.exe PRIVATE ini::static)
target_include_directories(ini.exe PUBLIC ${INI_ROOT_DIR})

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
