//-----------------------------------------------------------------------------
// File: client.cpp
//
// Desc: Entry point
//
// Copyright (c) 2002 Sergei Rustamov
//-----------------------------------------------------------------------------

#include "plainx.h"
#include "pxapp.h"

#include "masterdx.h"
#include "resource.h"


// {035C3AC9-5D21-474f-B829-838FF19F89F5}
GUID PXAPP_GUID = 
{ 0x35c3ac9, 0x5d21, 0x474f, { 0xb8, 0x29, 0x83, 0x8f, 0xf1, 0x9f, 0x89, 0xf5 } };



//-----------------------------------------------------------------------------
// Defines class app
//-----------------------------------------------------------------------------
class CMasterDXApp: public CPlainXApp
{
public:
  void OnCreateRegion(HWND hWnd);
  BOOL IsDragArea(int wx, int wy);
};


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CMasterDXApp::OnCreateRegion(HWND hWnd)
{
  //create window region
  DWORD dwFrameWidth    = GetSystemMetrics(SM_CXSIZEFRAME);
  DWORD dwFrameHeight   = GetSystemMetrics(SM_CYSIZEFRAME);
  DWORD dwCaptionHeight = GetSystemMetrics(SM_CYCAPTION);

  int nWWidth, nWHeight;
  GetEngine()->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_WIDTH, 0, &nWWidth);
  GetEngine()->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_HEIGHT, 0, &nWHeight);
  
  HRGN hRgn = CreateRoundRectRgn(dwFrameWidth + 2,
                              dwCaptionHeight + dwFrameHeight,
                              nWWidth + dwFrameWidth,
                              nWHeight + dwCaptionHeight + dwFrameHeight,
                              46,
                              46);

  SetRegion(hRgn);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
BOOL CMasterDXApp::IsDragArea(int xs, int ys)
{
	CSceneManager* pFrameScene = GetEngine()->GetScene("masterdx_frame");
	if (pFrameScene) {
		CSprite* pFrame = pFrameScene->GetSprite("frame");
		if (pFrame && pFrame->IsInSprite(xs, ys)) {
			return TRUE;
		}
		pFrame = pFrameScene->GetSprite("frame_r");
		if (pFrame && pFrame->IsInSprite(xs, ys)) {
			return TRUE;
		}
		pFrame = pFrameScene->GetSprite("frame_t");
		if (pFrame && pFrame->IsInSprite(xs, ys)) {
			return TRUE;
		}
		pFrame = pFrameScene->GetSprite("frame_b");
		if (pFrame && pFrame->IsInSprite(xs, ys)) {
			return TRUE;
		}
	}
	return FALSE;
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything and calls
//       UpdateFrame() when idle from the message pump.
//-----------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow)
{
  CMasterDXApp app;
  CMasterDX* pEngine;

  //create engine
  if (!PX_CreateEngine(&pEngine))
	  return -1;
  
  // analyze command line
  if (strcmp(app.GetArg(pCmdLine, 1), "-safe") == 0) {
    pEngine->SetSafeMode(TRUE);
  }

  app.SetTitle("MasterDX");

  //init app
  if (!app.Init(hInst, nCmdShow, pEngine))
    return -1;

  //run app
  return app.Run();
}

