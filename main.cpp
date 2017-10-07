#pragma once
#include <Windows.h>
#include "Game/Game.h"
#include <dxgidebug.h>

int main()
{
	//Track memory leaks
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Enable run-time memory leak check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	typedef HRESULT(__stdcall *fPtr)(const IID&, void**);
	HMODULE hDll = LoadLibrary(L"dxgidebug.dll");
	fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

	IDXGIDebug* pDXGIDebug;
	DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&pDXGIDebug);
	//_CrtSetBreakAlloc(206); //Filereader memory leak
#endif

	//For restarting, get filepath
	//Read warning below!
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	wstring wpath(buffer);
	string path = string(wpath.begin(), wpath.end());

	//Start game
#if defined(DEBUG) | defined(_DEBUG) 
	Game::GetInstance()->Start(true);
#else 
	Game::GetInstance()->Start(false);
#endif
	//End game if it exits the gameloop
	Game::Destroy();

	bool keepPlaying = true;
	OUTPUT->Log("\n\nX to exit, any other key to play again.\n\n", OUTPUT_COLOR::YELLOW);
	if (cin.get() == 'x') keepPlaying = false;

	if (keepPlaying)
	{
		//WARNING
		//I use system for cls and this
		//It's generally a super bad idea, as it's resource heavy
		//Starting programs like this is also bad, it will inherit the privileges from this program (so admin => new program also as admin)
		//Not to mention a lot of virus scanners will pick this up and treat it as malicious
		//So avoid using this, but for this i'm going to use it as a quick replay button of sorts
		//I know the risks and that it's not really a good idea, but for this it's a one-time okay as it is ONLY to make it easier to test the game
		system(path.c_str());
	}

	//When the game eventually closes, close this too
	return 0;
}