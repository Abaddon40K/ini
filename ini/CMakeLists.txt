# CMakeList.txt: проект CMake для ini; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Добавьте источник в исполняемый файл этого проекта.
add_library (ini STATIC "ini.cpp" "ini.h")

add_executable (ini.exe main.cpp)

target_compile_features(ini PUBLIC cxx_std_20)
target_compile_features(ini.exe PUBLIC cxx_std_20)
target_link_libraries(ini.exe PRIVATE ini)

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
