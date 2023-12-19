set(tests
        unit.cpp
)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

foreach (i ${tests})
        add_executable(${i} ${i})
        target_compile_features(${i} PUBLIC cxx_std_20)
        target_compile_definitions(${i} PUBLIC TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
        target_include_directories(${i} PUBLIC ${INI_ROOT_DIR} ${GTest_INCLUDE_DIRS})
        target_link_libraries(${i} PRIVATE
            ini::ini
            GTest::gtest_main
        )
        add_test(${i}.test ${i})
endforeach()








#target_compile_features(ini.exe PUBLIC cxx_std_20)
#target_link_libraries(ini.exe PRIVATE ini)
