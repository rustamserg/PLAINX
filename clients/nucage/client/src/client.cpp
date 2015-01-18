//-----------------------------------------------------------------------------
// File: client.cpp
//
// Desc: Entry point
//
// Copyright (c) 2002 Sergei Rustamov
//-----------------------------------------------------------------------------

#include "plainx.h"
#include "pxapp.h"

#include "nucage.h"

// {BB0AF69E-8755-4c0e-8354-FF4277E6C808}
GUID PXAPP_GUID = 
{ 0xbb0af69e, 0x8755, 0x4c0e, { 0x83, 0x54, 0xff, 0x42, 0x77, 0xe6, 0xc8, 0x8 } };


//-----------------------------------------------------------------------------
// Defines class app
//-----------------------------------------------------------------------------
class CNucAgeApp: public CPlainXApp
{
};


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything and calls
//       UpdateFrame() when idle from the message pump.
//-----------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow)
{
    CNucAgeApp app;
    CNucAge* pEngine;

    // create engine
    if (!PX_CreateEngine(&pEngine))
        return -1;

    // analyze command line
    if (strcmp(app.GetArg(pCmdLine, 1), "-safe") == 0) {
        pEngine->SetSafeMode(TRUE);
    }

    app.SetTitle("NucAge");

    // init app
	// multiple instance run are allowed temporally
	// since we have problem with using named pipes on Win98
    if (!app.Init(hInst, nCmdShow, pEngine, TRUE))
        return -1;

    // run app
    return app.Run();
}

