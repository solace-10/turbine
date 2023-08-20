#include <functional>

#include "veilid/veilidnetwork.hpp"

#include "geolocation/geolocationservice.hpp"
#include "turbine.h"
#include "veilid/veilidserver.hpp"

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
    if (result.IsValid())
    {
        ServerSharedPtr pServer = std::make_shared<VeilidServer>();
        pServer->SetGeolocationData(result.Data.value());
        g_pTurbine->AddServer(pServer);
    }
}

} // namespace Turbine
