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

#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <SDL_opengl.h>

#include "atlas/tile.h"

namespace Turbine
{

class Atlas;
class TileStreamer;
using AtlasUniquePtr = std::unique_ptr< Atlas >;

static const int sTileSize = 256;
static const int sMaxZoomLevels = 11;

class Atlas
{
public:
	Atlas( int windowWidth, int windowHeight );
	~Atlas();

	void Update(float delta);
	void Render();
	void GetScreenCoordinates( float longitude, float latitude, float& x, float& y ) const;

	void OnWindowSizeChanged( int width, int height );
	void OnMouseDrag( int deltaX, int deltaY );
	void OnZoomIn();
	void OnZoomOut();

private:
	void ClampOffset();
	void CalculateVisibleTiles( TileVector& visibleTiles );

	using TileTextureIdVector = std::vector< GLuint >;
	using TileMaps = std::vector< TileTextureIdVector >;
	TileMaps m_TileMaps;
	int m_MinimumZoomLevel;
	int m_CurrentZoomLevel;
	int m_MaxVisibleTilesX;
	int m_MaxVisibleTilesY;
	int m_OffsetX;
	int m_OffsetY;
	int m_WindowWidth;
	int m_WindowHeight;

	std::vector< int > m_TilesToDraw;
	std::unique_ptr< TileStreamer > m_pTileStreamer;
};

} // namespace Watcher
