#include <int/int.h>

#include <cassert>
#include <iostream>
#include <vector>

int stupid_len(int x) {
    if (x == 0) {
        return 0;
    }
    if (x == 1) {
        return 1;
    }
    if (x < 4) {
        return 2;
    }
    if (x < 8) {
        return 3;
    }
    if (x < 16) {
        return 4;
    }
    if (x < 32) {
        return 5;
    }
    if (x < 64) {
        return 6;
    }
    if (x < 128) {
        return 7;
    }
    if (x < 256) {
        return 8;
    }
    if (x < 512) {
        return 9;
    }
    return -1;
}

int main() {
    using namespace batriz::integer;
    for (int i = 0; i < 260; ++i) {
        assert(bit_length(i) == stupid_len(i));
    }

    assert(to_bytes(0x1234, 3, ByteOrder::Big) ==
           std::string("\x00\x12\x34", 3));

    assert(to_bytes(0x1234, 3, ByteOrder::Little) ==
           std::string("\x34\x12\x00", 3));

    assert(bit_length(1024) == 11);
    assert(bit_length(-1024) == 11);

    assert(to_bytes(1024, 10, ByteOrder::Big, true) ==
           std::string("\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00", 10));

    assert(to_bytes(-1025, 10, ByteOrder::Big, true) ==
           std::string("\xff\xff\xff\xff\xff\xff\xff\xff\xfb\xff", 10));

    assert(from_bytes(to_bytes(0x1234, 3, ByteOrder::Big), ByteOrder::Big) ==
           0x1234);

    assert(from_bytes(to_bytes(0x1234, 3, ByteOrder::Little),
                      ByteOrder::Little) == 0x1234);

    assert(from_bytes(std::string("\xfc\x00", 2), ByteOrder::Big, true) ==
           -1024);

    assert(from_bytes(std::string("\xfc\x00", 2), ByteOrder::Big, false) ==
           64512);
    return 0;
}
