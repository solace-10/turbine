/*
MIT License

Copyright (c) 2022 Pedro Nunes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "providers/digitalocean/dropletinfo.h"

namespace Turbine
{

DropletInfo::DropletInfo(const std::string& name, float memory, int cpus, float disk, float transfer, float priceMonthly, const Regions& regions) :
m_Name(name),
m_Memory(memory),
m_CPUs(cpus),
m_Disk(disk),
m_Transfer(transfer),
m_PriceMonthly(priceMonthly),
m_Regions(regions)
{

}

const std::string& DropletInfo::GetName() const
{
	return m_Name;
}

float DropletInfo::GetMemory() const
{
	return m_Memory;
}

int DropletInfo::GetCPUs() const
{
	return m_CPUs;
}

float DropletInfo::GetDisk() const
{
	return m_Disk;
}

float DropletInfo::GetTransfer() const
{
	return m_Transfer;
}

float DropletInfo::GetPrice() const
{
	return m_PriceMonthly;
}

const Regions& DropletInfo::GetRegions() const
{
	return m_Regions;
}

} // namespace Turbine
