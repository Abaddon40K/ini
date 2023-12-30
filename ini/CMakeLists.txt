# CMakeList.txt: проект CMake для ini; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
set(INI_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32) 
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake)
    CONAN_CMAKE_RUN(CONANFILE conanfile.txt
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing 
    )
endif()

# Добавьте источник в исполняемый файл этого проекта.
add_library(ini STATIC "ini.cpp" "ini.h"  "mmap.cpp" "mmap.h")
add_library(ini::ini ALIAS ini)

target_compile_features(ini PUBLIC cxx_std_17)
target_include_directories(ini PUBLIC ${INI_ROOT_DIR})


add_executable (ini.exe main.cpp)
target_compile_features(ini.exe PUBLIC cxx_std_17)
target_link_libraries(ini.exe PRIVATE ini)
target_include_directories(ini.exe PUBLIC ${INI_ROOT_DIR})



if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
