add_executable(str_tests str.cpp)
target_link_libraries(str_tests str)

add_executable(grapheme_break grapheme_break.cpp)
target_link_libraries(grapheme_break str)

add_executable(int_tests int.cpp)
#target_link_libraries(int_tests int)
