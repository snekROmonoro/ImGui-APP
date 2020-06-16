#pragma once
#include <d3d9.h>
#include <d3dx9.h>

namespace menu {
	void start( HWND window , IDirect3DDevice9* device );
	void main( );

	inline bool init_style = false;
}
