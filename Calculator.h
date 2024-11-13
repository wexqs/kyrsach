#pragma once
#include <vector>
#include <cstdint>

class Calculator {
    uint16_t results;
public:
    Calculator(std::vector<uint16_t> input_data);
    uint16_t send_res();
};
