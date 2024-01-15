set(tests
        unit.cpp
)
set(TEST_LINK_LIBRARIES
    ini::ini
)

find_package(Threads REQUIRED)

if (WIN32)
	if (NOT TARGET CONAN_PKG::gtest)
		message(FATAL_ERROR "GTest not found!")
	endif()
    list(APPEND TEST_LINK_LIBRARIES CONAN_PKG::gtest)
else()
	find_package(GTest REQUIRED)
    list(APPEND TEST_LINK_LIBRARIES GTest::gtest_main)
endif()

foreach (i ${tests})
        add_executable(${i} ${i})
        target_compile_features(${i} PUBLIC cxx_std_17)
        target_compile_definitions(${i} PUBLIC TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
        target_include_directories(${i} PUBLIC ${INI_ROOT_DIR} ${GTest_INCLUDE_DIRS})
        target_link_libraries(${i} PRIVATE ${TEST_LINK_LIBRARIES})
        add_test(${i}.test ${i})
endforeach()








#target_compile_features(ini.exe PUBLIC cxx_std_20)
#target_link_libraries(ini.exe PRIVATE ini)
