#include "main.hpp"
#include "menu/menu.hpp"

ATOM RegMyWindowClass( HINSTANCE hInstance , LPCTSTR lpzClassName );
BOOL InitWindowInstance( HINSTANCE hInstance , LPCTSTR lpzClassName , LPCTSTR lpzWindowName );
BOOL MakeWindowTransparent( HWND hWnd , unsigned char factor );
BOOL CreateDeviceD3D( HWND hWnd );
LRESULT CALLBACK WndProc( HWND hWnd , UINT message , WPARAM wParam , LPARAM lParam );
void ResetDevice( );

int main( HINSTANCE hInstance , HINSTANCE hPrevInstance , LPWSTR lpCmdLine , int nCmdShow )
{
	LPCTSTR lpzClass = ( "ImGui Application" );
	LPCTSTR lpzWindow = ( "ImGui Application" );

	/* Hide Console window */
	ShowWindow( GetConsoleWindow( ) , SW_HIDE );

	/* Register Window Class */
	if ( !RegMyWindowClass( hInstance , lpzClass ) )
		return 0;

	/* Create Window */
	if ( !InitWindowInstance( hInstance , lpzClass , lpzWindow ) )
		return 0;

	/* Create D3D Device */
	if ( !CreateDeviceD3D( g_pWindow ) ) {
		g_pD3D->Release( );
		UnregisterClass( lpzClass , hInstance );
		return 0;
	}

	/* Initialize ImGui */
	menu::start( g_pWindow , g_pd3dDevice );

	MSG msg;
	ZeroMemory( &msg , sizeof( msg ) );
	while ( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg , NULL , 0U , 0U , PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			continue;
		}

		/* ImGui New Frames */
		ImGui_ImplDX9_NewFrame( );
		ImGui_ImplWin32_NewFrame( );
		ImGui::NewFrame( );

		/* Create our Menu */
		menu::main( );

		/* End Frame */
		ImGui::EndFrame( );

		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE , false );
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE , false );
		g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE , false );
		g_pd3dDevice->Clear( 0 , NULL , D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_RGBA( 0 , 0 , 0 , 255 ) , 1.0f , 0 );
		if ( g_pd3dDevice->BeginScene( ) >= 0 )
		{
			/* Render ImGui DrawData */
			ImGui::Render( );
			ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );

			g_pd3dDevice->EndScene( );
		}
		HRESULT result = g_pd3dDevice->Present( NULL , NULL , NULL , NULL );

		// Handle loss of D3D9 device
		if ( result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel( ) == D3DERR_DEVICENOTRESET )
			ResetDevice( );
	}

	/* Shut ImGui down */
	ImGui_ImplDX9_Shutdown( );
	ImGui_ImplWin32_Shutdown( );

	/* Destroying Context creates an .ini file, which we won't need. */
	// ImGui::DestroyContext( );

	/* Release D3D devices */
	if ( g_pd3dDevice ) g_pd3dDevice->Release( );
	if ( g_pD3D ) g_pD3D->Release( );

	/* Destroy window */
	DestroyWindow( g_pWindow );

	/* Unregister class */
	UnregisterClass( lpzClass , hInstance );

	return 1;
}

ATOM RegMyWindowClass( HINSTANCE hInstance , LPCTSTR lpzClassName )
{
	WNDCLASS wcWindowClass = { 0 };
	wcWindowClass.lpfnWndProc = ( WNDPROC ) WndProc;
	wcWindowClass.style = CS_DROPSHADOW | CS_HREDRAW | CS_VREDRAW;
	wcWindowClass.hInstance = hInstance;
	wcWindowClass.lpszClassName = lpzClassName;
	wcWindowClass.hCursor = LoadCursor( NULL , IDC_ARROW );
	wcWindowClass.hbrBackground = ( HBRUSH ) COLOR_APPWORKSPACE;
	return RegisterClass( &wcWindowClass );
}

BOOL InitWindowInstance( HINSTANCE hInstance , LPCTSTR lpzClassName , LPCTSTR lpzWindowName )
{
	RECT screen_rect;
	GetWindowRect( GetDesktopWindow( ) , &screen_rect );

	WINDOW_POS [ 0 ] = ( screen_rect.right / 2 ) - ( WINDOW_SIZE [ 0 ] / 2 );
	WINDOW_POS [ 1 ] = ( screen_rect.bottom / 2 ) - ( WINDOW_SIZE [ 1 ] / 2 );

	/* Creating Window as a Popup, but a WindowApp too, so we can close/open it from taskbar for example. */
	g_pWindow = CreateWindowEx( WS_EX_APPWINDOW , lpzClassName , lpzWindowName , WS_POPUP , WINDOW_POS [ 0 ] , WINDOW_POS [ 1 ] , WINDOW_SIZE [ 0 ] , WINDOW_SIZE [ 1 ] , NULL , NULL , hInstance , NULL );

	if ( !g_pWindow ) {
		return FALSE;
	}

	/* If you want custom window alpha */
	// MakeWindowTransparent( g_pWindow , 255 );

	ShowWindow( g_pWindow , SW_SHOWDEFAULT );
	UpdateWindow( g_pWindow );

	return TRUE;
}

BOOL MakeWindowTransparent( HWND hWnd , unsigned char factor ) /* https://www.codeproject.com/Articles/5475/Quick-and-Dirty-Window-Transparency */
{
	/* Windows need to be layered to be made transparent. This is done
	 * by modifying the extended style bits to contain WS_EX_LAYARED. */
	SetLastError( 0 );

	SetWindowLong( hWnd ,
		GWL_EXSTYLE ,
		GetWindowLong( hWnd , GWL_EXSTYLE ) | WS_EX_LAYERED );

	if ( GetLastError( ) )
		return FALSE;

	/* Now, we need to set the 'layered window attributes'. This
	 * is where the alpha values get set. */
	return SetLayeredWindowAttributes( hWnd ,
		RGB( 255 , 255 , 255 ) ,
		factor ,
		LWA_COLORKEY | LWA_ALPHA );
}

BOOL CreateDeviceD3D( HWND hWnd )
{
	if ( ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) == NULL )
		return FALSE;

	ZeroMemory( &g_d3dpp , sizeof( g_d3dpp ) );
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; /* Present with vsync */
	if ( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd , D3DCREATE_HARDWARE_VERTEXPROCESSING , &g_d3dpp , &g_pd3dDevice ) < 0 )
		return FALSE;

	return TRUE;
}

void ResetDevice( )
{
	ImGui_ImplDX9_InvalidateDeviceObjects( );
	HRESULT hr = g_pd3dDevice->Reset( &g_d3dpp );
	if ( hr == D3DERR_INVALIDCALL )
		IM_ASSERT( 0 );
	ImGui_ImplDX9_CreateDeviceObjects( );
}

LRESULT CALLBACK WndProc( HWND hWnd , UINT message , WPARAM wParam , LPARAM lParam )
{
	/* Move Window ( ImGui Removed it a while ago ) */ {
		static POINTS m;
		switch ( message ) {
		case WM_LBUTTONDOWN:
			SetCapture( g_pWindow );
			m = MAKEPOINTS( lParam );
		case WM_LBUTTONUP:
			ReleaseCapture( );
		case WM_MOUSEMOVE:
			if ( wParam == MK_LBUTTON ) {
				POINTS p = MAKEPOINTS( lParam );
				RECT rect;
				GetWindowRect( g_pWindow , &rect );

				rect.left += p.x - m.x;
				rect.top += p.y - m.y;

				SetWindowPos( g_pWindow , HWND_TOPMOST , rect.left , rect.top , 0 , 0 , SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER );
			}
		}
	}

	if ( ImGui_ImplWin32_WndProcHandler( hWnd , message , wParam , lParam ) ) {
		return true;
	}

	switch ( message ) {
	case WM_SIZE:
		if ( g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED )
		{
			g_d3dpp.BackBufferWidth = LOWORD( lParam );
			g_d3dpp.BackBufferHeight = HIWORD( lParam );
			ResetDevice( );
		}

		return 0;

	case WM_SYSCOMMAND:
		if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd , message , wParam , lParam );
}
