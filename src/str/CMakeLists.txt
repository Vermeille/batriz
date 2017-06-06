add_executable(str_test string.cpp code_points.h combining_tables.cpp upper_tables.cpp)
target_link_libraries(str_test boost_locale)
