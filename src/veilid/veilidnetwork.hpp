#pragma once

#include "geolocation/geolocationqueryresult.hpp"

namespace Turbine
{

class VeilidNetwork
{
public:
    VeilidNetwork();
    ~VeilidNetwork();

private:
    void OnGeolocationCompleted(const GeolocationQueryResult& result);
};

} // namespace Turbine
