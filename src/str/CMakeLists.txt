add_executable(str_test
    string.cpp
    code_points.h
    grapheme_iterator.h
    combining_tables.cpp
    upper_tables.cpp
    lower_tables.cpp
    grapheme_cluster_break_tables.cpp)
target_link_libraries(str_test boost_locale)
