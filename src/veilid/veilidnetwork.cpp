#include <functional>

#include "veilid/veilidnetwork.hpp"

#include "geolocation/geolocationservice.hpp"
#include "turbine.h"

namespace Turbine
{

void OnGeolocationCompleted2(const GeolocationQueryResult& result)
{

}

VeilidNetwork::VeilidNetwork()
{

    GeolocationService* pGeolocationService = g_pTurbine->GetGeolocationService();
    GeolocationQueryResultCallback callback = std::bind(&VeilidNetwork::OnGeolocationCompleted, this, std::placeholders::_1);
    pGeolocationService->Query("170.64.186.46", callback);
}

VeilidNetwork::~VeilidNetwork()
{

}

void VeilidNetwork::OnGeolocationCompleted(const GeolocationQueryResult& result)
{
    int a = 0;
}

} // namespace Turbine
