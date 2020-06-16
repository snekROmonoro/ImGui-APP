#include "menu.hpp"

/* include imgui */
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/impl/imgui_impl_dx9.h"
#include "ImGui/impl/imgui_impl_win32.h"
#include "../main.hpp"

void menu::start( HWND window , IDirect3DDevice9* device ) {
	if ( init_style )
		return;

	/* Create Context */
	ImGui::CreateContext( );

	/* You'll need these to create your own style */
	ImGuiIO& io = ImGui::GetIO( );
	ImGuiStyle& style = ImGui::GetStyle( );

	/* Create your own style here */
	/* -------------------------- */

	/* Implement ImGui Renderer */
	ImGui_ImplWin32_Init( window );
	ImGui_ImplDX9_Init( device );

	init_style = true;
}

void menu::main( )
{
	/* TIP: We can't let ImGui move our window */
	const ImGuiWindowFlags m_window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

	ImGui::SetNextWindowPos( ImVec2( WINDOW_POS [ 0 ] , WINDOW_POS [ 1 ] ) , ImGuiCond_Appearing );
	ImGui::SetNextWindowSize( ImVec2( WINDOW_SIZE [ 0 ] , WINDOW_SIZE [ 1 ] ) , ImGuiCond_Always );
	static bool m_window_opened = true;
	if ( m_window_opened ) {
		ImGui::Begin( "ImGui Application" , &m_window_opened , m_window_flags ); {

		} ImGui::End( );
	}

	/* Open a new window */
	if ( !m_window_opened ) {
		ImGui::SetNextWindowPos( ImVec2( WINDOW_POS [ 0 ] , WINDOW_POS [ 1 ] ) , ImGuiCond_Appearing );
		ImGui::SetNextWindowSize( ImVec2( WINDOW_SIZE [ 0 ] , WINDOW_SIZE [ 1 ] ) , ImGuiCond_Always );

		static bool m_second_window_opened = true;
		ImGui::Begin( "ImGui Application - Secondary" , &m_second_window_opened , m_window_flags ); {

		} ImGui::End( );

		/* We wanted to close the window, so close process too */
		if ( !m_second_window_opened )
			ExitProcess( 0 );
	}

	/* Exit when released END key */ {
		const bool pressing_end_key = GetAsyncKeyState( VK_END );
		static bool pressed_end_key = false;
		if ( pressing_end_key )
			pressed_end_key = true;
		else if ( !pressing_end_key && pressed_end_key )
			ExitProcess( 0 );
	}
}
