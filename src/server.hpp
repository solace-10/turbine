#pragma once

#include "geolocation/geolocationdata.hpp"

#include <optional>

namespace Turbine
{

class Server
{
public:
    Server() {}
    virtual ~Server() {}

    std::optional<GeolocationData> GetGeolocationData() const;
    void SetGeolocationData(const GeolocationData& geolocationData);

private:
    std::optional<GeolocationData> m_GeolocationData;
};

inline std::optional<GeolocationData> Server::GetGeolocationData() const
{
    return m_GeolocationData;
}

inline void Server::SetGeolocationData(const GeolocationData& geolocationData)
{
    m_GeolocationData = geolocationData;
}

} // namespace Turbine