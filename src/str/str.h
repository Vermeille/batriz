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

std::string center(const std::string& s,
                   int width,
                   const std::string& fill = " ");

int count(const std::string& s,
          const std::string& sub,
          int start = 0,
          std::string::size_type end = -1);

bool endswith(const std::string& s,
              const std::string& suffix,
              int start = 0,
              std::string::size_type end = -1);

std::string expandtabs(const std::string& s, int tabsize = 8);

int find(const std::string& s,
         const std::string& sub,
         int start = 0,
         std::string::size_type end = -1);

int index(const std::string& s,
          const std::string& pattern,
          int start = 0,
          std::string::size_type end = -1);
std::string upper(const std::string& s);
std::string lower(const std::string& s);

template <class T = std::vector<std::string>>
T split(const std::string& s, char c = ' ');

// ********* PYTHON GLUE **********
bool in(const std::string& c, const std::string& s);
bool in(char c, const std::string& s);

// ********* NON PYTHON FUNCTIONS **********
void append_code_point(std::string& s, int cp);
int countchars(const std::string& s);
int countcodepoints(const std::string& s);

}  // namespace str

#include "str-impl.h"
