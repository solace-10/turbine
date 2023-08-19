// Copyright 2023 Pedro Nunes
//
// This file is part of Turbine.
//
// Turbine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Turbine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Turbine. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <optional>
#include <string>

#include "geolocation/geolocationdata.hpp"

namespace Turbine
{

class GeolocationQueryResult
{
public:
    bool IsValid() const;
    std::optional<GeolocationData> Data;
    std::optional<std::string> Error;
};

inline bool GeolocationQueryResult::IsValid() const
{
    return Data.has_value();
}

} // namespace Turbine