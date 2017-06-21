#include <str/str.h>

#include <cassert>
#include <iostream>

int main() {
    assert(str::upper("ßtoto") == "ßTOTO");
    assert(str::countchars("ßtoto") == 5);
    assert(str::center("e\u0301", 3) == " e\u0301 ");
    assert(str::center("e\u0301", 4) == " e\u0301  ");
    assert(str::center("+", 8) == "   +    ");
    assert(str::center("-e\u0301e\u0301-", 8) == "  -e\u0301e\u0301-  ");
    assert(!str::endswith("le caca", "cac"));
    assert(str::endswith("le caca", "cac", 0, 6));
    assert(str::expandtabs("01\t012\t0123\t01234") ==
           "01      012     0123    01234");
    assert(str::expandtabs("01\t012\t0123\t01234", 4) ==
           "01  012 0123    01234");
    assert(str::count("ddddcacacccaddd", "ca") == 3);
    assert((str::split("le caca est beau") ==
            std::vector<std::string>{"le", "caca", "est", "beau"}));
    assert(str::make_code_points("$¢€𐍈").size() == 4);
    assert(str::upper("le caca e\u0301toile\u0301 σ") == "LE CACA ÉTOILÉ Σ");
    assert(str::lower(str::upper("le caca e\u0301toile\u0301 σ")) ==
           "le caca e\u0301toile\u0301 σ");
    assert(str::zfill("42", 5) == "00042");
    assert(str::zfill("+42", 5) == "+0042");
    assert(str::zfill("-42", 5) == "-0042");
    assert(str::join(", ", std::vector<int>({1})) == "1");
    assert(str::join(", ", std::vector<int>({1, 2, 3})) == "1, 2, 3");

    std::string a = "aßtotolae\u0301e\u0301 \r\nu";
    auto graphemes = str::make_graphemes(a);
    std::vector<std::string> parsed({"a",
                                     "ß",
                                     "t",
                                     "o",
                                     "t",
                                     "o",
                                     "l",
                                     "a",
                                     "e\u0301",
                                     "e\u0301",
                                     " ",
                                     "\r\n",
                                     "u"});
    assert(std::equal(graphemes.begin(),
                      graphemes.end(),
                      parsed.begin(),
                      [](auto cp, auto ref) {
                          return std::string(cp.first.str_begin(),
                                             cp.second.str_begin()) == ref;
                      }));

    return 0;
}
