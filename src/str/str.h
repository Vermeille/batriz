#pragma once

#include <string>
#include <vector>

#include "code_points.h"
#include "grapheme_iterator.h"

namespace str {

extern const std::string ascii_lowercase;
extern const std::string ascii_uppercase;
extern const std::string ascii_letters;
extern const std::string digits;
extern const std::string hexdigits;
extern const std::string octdigits;
extern const std::string punctuation;
extern const std::string whitespace;
extern const std::string printable;

// https://docs.python.org/3/library/stdtypes.html#str.capitalize
std::string capitalize(const std::string& s);

// https://docs.python.org/3/library/stdtypes.html#str.casefold
std::string casefold(const std::string& s);

// https://docs.python.org/3/library/stdtypes.html#str.center
std::string center(const std::string& s,
                   int width,
                   const std::string& fill = " ");

// https://docs.python.org/3/library/stdtypes.html#str.count
int count(const std::string& s,
          const std::string& sub,
          int start = 0,
          std::string::size_type end = -1);

// https://docs.python.org/3/library/stdtypes.html#str.encode
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.endswith
bool endswith(const std::string& s,
              const std::string& suffix,
              int start = 0,
              std::string::size_type end = -1);

// https://docs.python.org/3/library/stdtypes.html#str.expandtabs
std::string expandtabs(const std::string& s, int tabsize = 8);

// https://docs.python.org/3/library/stdtypes.html#str.find
int find(const std::string& s,
         const std::string& sub,
         int start = 0,
         std::string::size_type end = -1);

// https://docs.python.org/3/library/stdtypes.html#str.format
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.format_map
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.index
int index(const std::string& s,
          const std::string& pattern,
          int start = 0,
          std::string::size_type end = -1);

// https://docs.python.org/3/library/stdtypes.html#str.isalnum
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.isalpha
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.isdecimal
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.isdigit
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.isidentifier
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.islower
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.isnumeric
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.isprintable
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.isspace
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.istitle
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.isupper<Paste>
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.join
template <class Ts>
std::string join(const std::string& joiner, Ts xs);

// https://docs.python.org/3/library/stdtypes.html#str.ljust
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.lower
std::string lower(const std::string& s);

// https://docs.python.org/3/library/stdtypes.html#str.lstrip
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.maketrans
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.partition
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.replace
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.rfind
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.rindex
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.rjust
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.rpartition
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.rsplit
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.rstrip
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.split
template <class T = std::vector<std::string>>
T split(const std::string& s, char c = ' ');

// https://docs.python.org/3/library/stdtypes.html#str.splitlines
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.startswith
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.strip
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.swapcase
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.title
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.translate
// TODO

// https://docs.python.org/3/library/stdtypes.html#str.upper
std::string upper(const std::string& s);

// https://docs.python.org/3/library/stdtypes.html#str.zfill
std::string zfill(const std::string& s, int width);

// ********* PYTHON GLUE **********
bool in(const std::string& c, const std::string& s);
bool in(char c, const std::string& s);

// ********* NON PYTHON FUNCTIONS **********
void append_code_point(std::string& s, int cp);
int countchars(const std::string& s);
int countcodepoints(const std::string& s);

}  // namespace str

#include "str-impl.h"
