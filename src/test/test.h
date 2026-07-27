#pragma once
#include <iostream>

class Test
{
public:
    static void pass(const char *name) { std::cout << "  [PASS] " << name << "\n"; }
    static void fail(const char *name) { std::cout << "  [FAIL] " << name << "\n"; }
    static void testName(const char *name) { std::cout << "Test: " << name << "\n"; }
};