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

#include "atlas/tile.h"
#include "textureloader.h"

namespace Turbine
{

Tile::Tile() :
m_X( -1 ),
m_Y( -1 ),
m_ZoomLevel( -1 ),
m_Texture( 0 )
{

}

Tile::Tile( int x, int y, int zoomLevel ) :
m_X( x ),
m_Y( y ),
m_ZoomLevel( zoomLevel ),
m_Texture( 0 )
{

}

Tile::~Tile()
{
	if ( m_Texture != 0 )
	{
		TextureLoader::UnloadTexture( m_Texture );
	}
}

int Tile::X() const
{
	return m_X;
}

int Tile::Y() const
{
	return m_Y;
}

int Tile::ZoomLevel() const
{
	return m_ZoomLevel;
}

GLuint Tile::Texture() const
{
	return m_Texture;
}

void Tile::AssignTexture( GLuint texture )
{
	m_Texture = texture;
}

} // namespace Watcher
