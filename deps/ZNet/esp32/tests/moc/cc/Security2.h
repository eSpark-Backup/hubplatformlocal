#pragma once

#include <cassert>

#include "../cc/CommanClass.h"

class Security2 : public CommandClass {
public:
    Security2();

    bool run( std::vector<std::vector<uint8_t>>& rev,
              std::vector<uint8_t>& ret,
              std::vector<uint8_t> const& data ) override;
};
