#pragma once

#include <string>

namespace Turbine
{

class PinData
{
public:
    PinData(const std::string& address);

private:
    std::string m_Address;
};

} // namespace Turbine