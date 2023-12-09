set(tests
	unit.cpp
)
foreach (i ${tests})
	add_executable(${i} ${i})
	target_compile_features(${i} PUBLIC cxx_std_20)
	target_link_libraries(${i} PRIVATE ini)
	add_test(${i}.test ${i})
endforeach()








#target_compile_features(ini.exe PUBLIC cxx_std_20)
#target_link_libraries(ini.exe PRIVATE ini)