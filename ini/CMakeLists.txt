# CMakeList.txt: проект CMake для ini; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
set(INI_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Добавьте источник в исполняемый файл этого проекта.
add_library(ini STATIC "ini.cpp" "ini.h"  "mmap.cpp" "mmap.h")
add_library(ini::ini ALIAS ini)

target_compile_features(ini PUBLIC cxx_std_20)
target_include_directories(ini PUBLIC ${INI_ROOT_DIR})


add_executable (ini.exe main.cpp)
target_compile_features(ini.exe PUBLIC cxx_std_20)
target_link_libraries(ini.exe PRIVATE ini)
target_include_directories(ini.exe PUBLIC ${INI_ROOT_DIR})

if (BUILD_TESTS AND UNIX)
    enable_testing()
    add_subdirectory(tests)
endif()
