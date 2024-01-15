set(tests
        unit.cpp
)
set(TEST_LINK_LIBRARIES
    ini::shared
)

if(WIN32)
    include(${INI_ROOT_DIR}/cmake/conan.cmake)
    CONAN_CMAKE_RUN(CONANFILE conanfile.txt
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing
    )
    list(APPEND TEST_LINK_LIBRARIES CONAN_PKG::gtest)

else()
    find_package(GTest REQUIRED)
    list(APPEND TEST_LINK_LIBRARIES GTest::gtest_main)
endif()

find_package(Threads REQUIRED)

foreach(i ${tests})
        add_executable(${i} ${i})
        target_compile_features(${i} PUBLIC cxx_std_17)
        target_compile_definitions(${i} PUBLIC TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
        target_include_directories(${i} PUBLIC ${INI_ROOT_DIR} ${GTest_INCLUDE_DIRS})
        target_link_libraries(${i} PRIVATE ${TEST_LINK_LIBRARIES})
        add_test(NAME ${i}.test COMMAND ${i})
        add_custom_target(${i}.test ALL COMMAND ${i})
endforeach()




