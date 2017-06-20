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

    std::string a = "aßtotolae\u0301e\u0301 \r\nu";
    for (auto g : str::make_graphemes(a)) {
        std::cout << "-"
                  << std::string(g.first.str_begin(), g.second.str_begin())
                  << "-\n";
    }

    while (std::cin) {
        std::string s;
        std::cin >> s;
        std::cout << str::capitalize(s) << "\n";
    }

    return 0;
}
