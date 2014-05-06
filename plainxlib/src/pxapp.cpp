#include "plainx.h"
#include "pxapp.h"

#define PXA_CLASSNAME "PXAppClass"
#define PXA_WINDOWNAME "PlainXApp"


static CPlainXApp* g_pApp = NULL;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern GUID PXAPP_GUID;


CPlainXApp::CPlainXApp()
{
  g_pApp = this;

  strcpy(m_szClassName, PXA_CLASSNAME);
  strcpy(m_szWindowName, PXA_WINDOWNAME);

  m_hWnd = NULL;
  m_hInst = NULL;
  m_pEngine = NULL;
  m_bActive = FALSE;
  m_hAccel = NULL;
  m_hRgn = NULL;
  m_bIsDrawCursor = TRUE;
  m_bIsCustomDragArea = TRUE;
  m_hGuardPipe = INVALID_HANDLE_VALUE;
}


CPlainXApp::~CPlainXApp()
{
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  return g_pApp->Babylon5(hWnd, msg, wParam, lParam);
}


/*
 * UnicodeToAnsi converts the Unicode string pszW to an ANSI string
 * and returns the ANSI string through ppszA. Space for the
 * the converted string is allocated by UnicodeToAnsi.
 */ 

HRESULT __fastcall UnicodeToAnsi(LPCOLESTR pszW, LPSTR* ppszA)
{

    ULONG cbAnsi, cCharacters;
    DWORD dwError;

    // If input is null then just return the same.
    if (pszW == NULL)
    {
        *ppszA = NULL;
        return NOERROR;
    }

    cCharacters = wcslen(pszW)+1;
    // Determine number of bytes to be allocated for ANSI string. An
    // ANSI string can have at most 2 bytes per character (for Double
    // Byte Character Strings.)
    cbAnsi = cCharacters*2;

    // Use of the OLE allocator is not required because the resultant
    // ANSI  string will never be passed to another COM component. You
    // can use your own allocator.
    *ppszA = (LPSTR) malloc(cbAnsi);
    if (NULL == *ppszA)
        return E_OUTOFMEMORY;

    // Convert to ANSI.
    if (0 == WideCharToMultiByte(CP_ACP, 0, pszW, cCharacters, *ppszA,
                  cbAnsi, NULL, NULL))
    {
        dwError = GetLastError();
        free(*ppszA);
        *ppszA = NULL;
        return HRESULT_FROM_WIN32(dwError);
    }
    return NOERROR;

} 


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
BOOL CPlainXApp::CreateGuardPipe()
{
  LPOLESTR lpszGuid = (LPOLESTR)malloc(1024);
  StringFromGUID2(PXAPP_GUID, lpszGuid, 1024);
        
  LPSTR lpszPipe = (LPSTR)malloc(1024);
  LPSTR lpszGuidA;
        
  strcpy(lpszPipe, "\\\\.\\pipe\\");
  UnicodeToAnsi(lpszGuid, &lpszGuidA);
  strcat(lpszPipe, lpszGuidA);
        
  m_hGuardPipe = CreateNamedPipe(lpszPipe, 
                                 PIPE_ACCESS_OUTBOUND,
                                 PIPE_TYPE_BYTE | PIPE_WAIT,
                                 1,
                                 0,
                                 0,
                                 NMPWAIT_WAIT_FOREVER,
                                 NULL);

  free(lpszGuid);
  free(lpszGuidA);
  free(lpszPipe);

  if (m_hGuardPipe == INVALID_HANDLE_VALUE) {
    return FALSE;
  }
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
char* CPlainXApp::GetArg(const char* pcszCmdLine, int iPos)
{
  static char pszCmdLine[MAX_BUFFER];
  char* pszRes = 0;

  strncpy(pszCmdLine, pcszCmdLine, MAX_BUFFER);
  
  pszRes = &pszCmdLine[0];
  if (iPos > 0) {
    for (size_t i = 0; i < strlen(pcszCmdLine); i++) {
      if (pszCmdLine[i] == ' ') {
        pszCmdLine[i] = 0;
        iPos--;
        if (iPos == 0) break;
        pszRes = &pszCmdLine[i + 1];
      }
    }
  }
  return pszRes;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
BOOL CPlainXApp::Init(HINSTANCE hInst,
                      int nCmdShow,
                      CEngine* pEngine,
                      BOOL bIsRunMoreThanOnce)
{
  HWND hWnd;
  HACCEL hAccel;
  int nValue;
  OSVERSIONINFO osvi;
  BOOL bIsWindows98orLater;


	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);

	bIsWindows98orLater = 
	(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
	( (osvi.dwMajorVersion > 4) ||
	( (osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0) ) );

	// FIX: for Win98 guard pipe is turned off since
	// named pipes are not supported by Win98/95
	if (bIsWindows98orLater)
		bIsRunMoreThanOnce = TRUE;

	if (!bIsRunMoreThanOnce) {
		if (!CreateGuardPipe()) {
			PX_ErrorHandle("Can't run more than one instance of application");
			return FALSE;
		}
	}

  srand(timeGetTime());

  //init com library
  if (FAILED(CoInitialize(NULL))) {
    PX_ErrorHandle("Failed Initialize COM library");
    return FALSE;
  }

  char buffer[_MAX_PATH];

  //remember engine
  m_pEngine = pEngine;

  //Get the current working directory
  _getcwd(buffer, _MAX_PATH);
	
  //init engine by default windowed mode
  if (!m_pEngine->InitEngine(buffer, TRUE)) {
    PX_ErrorHandle("Failed init engine");
    return FALSE;
  }
  
  //read real window mode and if exist so set up
  m_pEngine->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_FULLSCREEN, 0, &nValue);
  if (nValue) {
    m_pEngine->SetWindowed(FALSE);
  }
  
  // mark debug mode
#ifdef _DEBUG
  strcat(m_szWindowName, " - Developer version");
#endif
  
  // mark safe mode
  if (m_pEngine->GetSafeMode()) {
    strcat(m_szWindowName, " (Safe mode)");
  }

  //create window
  if (FAILED(WinInit(hInst, nCmdShow, &hWnd, &hAccel))) {
    PX_ErrorHandle("Failed create window");
    return FALSE;
  }
  
  //Note: Always set HWND first!!!
  m_pEngine->SetHWnd(hWnd);

  //set up engine components
  if (!m_pEngine->InitVideo()) {
    PX_ErrorHandle("Failed init video");
    return FALSE;
  }

  if (!m_pEngine->InitInput()) {
    PX_ErrorHandle("Failed init input");
    return FALSE;
  }

  if (!m_pEngine->InitAudio()) {
    PX_ErrorHandle("Failed init audio");
    return FALSE;
  }

  if (!m_pEngine->InitNetwork()) {
    PX_ErrorHandle("Failed init network");
    return FALSE;
  }
  
  //remember params
  m_hWnd = hWnd;
  m_hInst = hInst;
  m_hAccel = hAccel;

  //all ok
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CPlainXApp::SetTitle(const char* pcszTitle)
{
	strncpy(m_szWindowName, pcszTitle, sizeof(m_szWindowName) - 1);
	m_szWindowName[sizeof(m_szWindowName) - 1] = 0;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
int CPlainXApp::Run()
{
  MSG msg;

  while(TRUE) {
    // Look for messages, if none are found then 
    // update the state and display it
    if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
      if (GetMessage(&msg, NULL, 0, 0) == 0) {
        // WM_QUIT was posted, so exit
        return (int)msg.wParam;
      }

      // Translate and dispatch the message
      if (TranslateAccelerator(m_hWnd, m_hAccel, &msg) == 0) {
        TranslateMessage(&msg); 
        DispatchMessage(&msg);
      }
    } else {
      if (m_bActive) {
        //Update game world
		DX_OnIdle(m_hWnd);
      } else {
        // Go to sleep if we have nothing else to do
        WaitMessage();

        // Ignore time spent inactive 
        if (m_pEngine) {
          m_pEngine->SetLastTick(timeGetTime());
        }
      }
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
// Name: WinInit()
// Desc: Init the window
//-----------------------------------------------------------------------------
HRESULT CPlainXApp::WinInit(HINSTANCE hInst, int nCmdShow, HWND* phWnd, HACCEL* phAccel)
{
  WNDCLASSEX wc;
  HWND       hWnd;
  HACCEL     hAccel;
  int nWWidth, nWHeight;

  // Register the window class
  wc.cbSize        = sizeof(wc);
  wc.lpszClassName = TEXT(m_szClassName);
  wc.lpfnWndProc   = MainWndProc;
  wc.style         = CS_VREDRAW | CS_HREDRAW;
  wc.hInstance     = hInst;
  wc.hIcon         = NULL; //LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAIN));
  wc.hIconSm       = NULL; //LoadIcon(hInst, MAKEINTRESOURCE(IDI_MAIN));
  wc.hCursor       = NULL;
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wc.lpszMenuName  = NULL;
  wc.cbClsExtra    = NULL;
  wc.cbWndExtra    = NULL;

  if (RegisterClassEx(&wc) == 0) {
    return E_FAIL;
  }

  //Load keyboard accelerators
  hAccel = NULL;
  //hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MAIN_ACCEL));
  
  // Calculate the proper size for the window given a client of 800x600
  DWORD dwFrameWidth    = GetSystemMetrics(SM_CXSIZEFRAME);
  DWORD dwFrameHeight   = GetSystemMetrics(SM_CYSIZEFRAME);
  DWORD dwCaptionHeight = GetSystemMetrics(SM_CYCAPTION);

  int nValue;
  m_pEngine->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_WIDTH, 0, &nWWidth);
  DWORD dwWindowWidth = nWWidth + dwFrameWidth * 2;
    
  m_pEngine->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_HEIGHT, 0, &nWHeight);
  DWORD dwWindowHeight = nWHeight + dwFrameHeight * 2 + dwCaptionHeight;
	
  DWORD dwXPos = GetSystemMetrics(SM_CXSCREEN);
  DWORD dwYPos = GetSystemMetrics(SM_CYSCREEN);
	
  if (dwXPos < dwWindowWidth) {
    dwXPos = 0;
  } else {
    dwXPos = (dwXPos - dwWindowWidth)/2;
  }
  if (dwYPos < dwWindowHeight) {
    dwYPos = 0;
  } else {
    dwYPos = (dwYPos - dwWindowHeight)/2;
  }
        
  m_pEngine->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_SCREENPOS, 0, &nValue);
  if (nValue != -1) {
    dwXPos = nValue;
  }
  m_pEngine->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_SCREENPOS, 1, &nValue);
  if (nValue != -1) {
    dwYPos = nValue;
  }

  // Create and show the main window
  DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX;
  hWnd = CreateWindowEx(0, TEXT(m_szClassName), 
                           TEXT(m_szWindowName),
                           dwStyle, 
                           dwXPos, 
                           dwYPos,
                           dwWindowWidth, 
                           dwWindowHeight, 
                           NULL, 
                           NULL, 
                           hInst, 
                           NULL);

  if (hWnd == NULL) {
    return E_FAIL;
  }
 
  //set region if present
  if (m_pEngine->GetWindowed()) {
    OnCreateRegion(hWnd);
    SetWindowRgn(hWnd, m_hRgn, TRUE);
  }
    
  //show window
  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  // Save the window size/pos for switching modes
  GetWindowRect(hWnd, &m_rcWindow);

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	DrawSystemCursor(FALSE);

  *phWnd   = hWnd;
  *phAccel = hAccel;

  return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MainWndProc()
// Desc: The main window procedure
//-----------------------------------------------------------------------------
LRESULT CPlainXApp::Babylon5(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  int x,y;

  switch (msg) 
  {
    HANDLE_MSG(hWnd, WM_ACTIVATE, DX_OnActivate);
    HANDLE_MSG(hWnd, WM_DESTROY, DX_OnDestroy);

  case WM_SYSKEYDOWN:
	  if (wParam == VK_RETURN) {

		  // check for ALT code (29 bit set 1)
		  //if (!(lParam & 0x40000000))
		//		break;

		// Toggle the fullscreen/window mode
		if (m_pEngine) {
          if (!m_pEngine->GetWindowed()) {
            //if we fs
			int nBPP;
			BOOL bIsCanBeW;
	      
            SetWindowRgn(hWnd, NULL, TRUE);
            m_hRgn = NULL;
            OnCreateRegion(hWnd);
            SetWindowRgn(hWnd, m_hRgn, TRUE);

            m_pEngine->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_BPP, 0, &nBPP);
			bIsCanBeW = m_pEngine->GetVideo()->IsCanBeWindowed(nBPP);
            m_pEngine->GetVideo()->ToggleScreenMode(hWnd, !bIsCanBeW, &m_rcWindow);
			m_pEngine->SetWindowed(bIsCanBeW);
          } else {
			m_pEngine->GetVideo()->ToggleScreenMode(hWnd, TRUE, &m_rcWindow);
			m_pEngine->SetWindowed(FALSE);
            
            SetWindowRgn(hWnd, NULL, TRUE);
            m_hRgn = NULL;
          }
		  m_pEngine->RecreateAll();
          m_pEngine->ReloadInput();
		  
		  // ignore time spend for toggle screen mode
		  m_pEngine->SetLastTick(timeGetTime());
        }
		return 0;		  
	  }
	  break;

  case WM_KEYDOWN:
	  if (wParam == VK_ESCAPE) {
		// Received command to exit app
        PostMessage(hWnd, WM_CLOSE, 0, 0);
		return 0;
	  }
	  break;

    case WM_GETMINMAXINFO:
      if (m_pEngine) {
        // Don't allow resizing in windowed mode.  
        // Fix the size of the window to (client size)
        MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;

        DWORD dwFrameWidth    = GetSystemMetrics(SM_CXSIZEFRAME);
        DWORD dwFrameHeight   = GetSystemMetrics(SM_CYSIZEFRAME);
        DWORD dwCaptionHeight = GetSystemMetrics(SM_CYCAPTION);

        int nValue;
        m_pEngine->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_WIDTH, 0, &nValue);
        pMinMax->ptMinTrackSize.x = nValue + dwFrameWidth * 2;
        m_pEngine->GetVars()->GetInt(L_GROUP, L_VID_VIDEO, L_VID_HEIGHT, 0, &nValue);
        pMinMax->ptMinTrackSize.y = nValue + dwFrameHeight * 2 + dwCaptionHeight;

        pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
        pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;
      }
      return 0;

    case WM_MOVE:
      // Retrieve the window position after a move.
      if (m_pEngine && m_pEngine->GetVideo()) {
        m_pEngine->GetVideo()->RetrieveWindowPosition();
      }
      return 0;

    case WM_SIZE:
      // Check to see if we are losing our window...
      if (SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam) {
        m_bActive = FALSE;
      } else {
        m_bActive = TRUE;
      }

      if (m_pEngine && m_pEngine->GetVideo()) {
        m_pEngine->GetVideo()->RetrieveWindowPosition();
        m_pEngine->SetActive(m_bActive);
      }
      break;

    case WM_SETCURSOR:
		if (m_pEngine && m_pEngine->GetWindowed() && !m_bIsCustomDragArea) {
			WORD wHit = LOWORD(lParam);
			if (wHit == HTCAPTION ||
				wHit == HTCLOSE ||
				wHit == HTMINBUTTON ||
				wHit == HTMAXBUTTON ||
				wHit == HTTOP)
			{
				m_pEngine->SetUseMouse(FALSE);
				DrawSystemCursor(TRUE);
			} else {
				m_pEngine->SetUseMouse(TRUE);
				DrawSystemCursor(FALSE);
			}
		}
		break;

    case WM_LBUTTONDOWN:
      if (m_pEngine && m_pEngine->GetWindowed()) {
        x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		if (m_pEngine->GetUseMouse())
			m_pEngine->PostMessage(EM_LMOUSEDOWN, x, y);
      }
      return 0;

   case WM_LBUTTONUP:
     if (m_pEngine && m_pEngine->GetWindowed()) {
       x = GET_X_LPARAM(lParam);
       y = GET_Y_LPARAM(lParam);
	   if (m_pEngine->GetUseMouse())
			m_pEngine->PostMessage(EM_LMOUSEUP, x, y);
     }
     return 0;
			
   case WM_RBUTTONDOWN:
     if (m_pEngine && m_pEngine->GetWindowed()) {
       x = GET_X_LPARAM(lParam);
       y = GET_Y_LPARAM(lParam);
	   if (m_pEngine->GetUseMouse())
			m_pEngine->PostMessage(EM_RMOUSEDOWN, x, y); 
     }
     return 0;

   case WM_RBUTTONUP:
     if (m_pEngine && m_pEngine->GetWindowed()) {
       x = GET_X_LPARAM(lParam);
       y = GET_Y_LPARAM(lParam);
	   if (m_pEngine->GetUseMouse())
			m_pEngine->PostMessage(EM_RMOUSEUP, x, y);
     }
     return 0;

   case WM_MOUSEMOVE:
     if (m_pEngine && m_pEngine->GetWindowed()) {
       x = GET_X_LPARAM(lParam);
       y = GET_Y_LPARAM(lParam);
	   if (m_pEngine->GetUseMouse())
			m_pEngine->PostMessage(EM_MOUSEMOVE, x, y);
     }
     return 0;

   case WM_NCMOUSEMOVE:
     if (m_pEngine && m_pEngine->GetWindowed() && m_bIsCustomDragArea) {
       x = GET_X_LPARAM(lParam);
       y = GET_Y_LPARAM(lParam);
       
       RECT rcW;
       DWORD dwFrameWidth    = GetSystemMetrics(SM_CXSIZEFRAME);
       DWORD dwFrameHeight   = GetSystemMetrics(SM_CYSIZEFRAME);
       DWORD dwCaptionHeight = GetSystemMetrics(SM_CYCAPTION);

       GetWindowRect(hWnd, &rcW);
	   
	   if (m_pEngine->GetUseMouse())
			m_pEngine->PostMessage(EM_MOUSEMOVE, x - rcW.left - dwFrameWidth, 
									y - rcW.top - dwFrameHeight * 2 - dwCaptionHeight);
     }
     return 0;

   case WM_NCHITTEST:
		if (m_pEngine && m_pEngine->GetWindowed()) {
			x = GET_X_LPARAM(lParam);
			y = GET_Y_LPARAM(lParam);

			RECT rcW;
			DWORD dwFrameWidth    = GetSystemMetrics(SM_CXSIZEFRAME);
			DWORD dwFrameHeight   = GetSystemMetrics(SM_CYSIZEFRAME);
			DWORD dwCaptionHeight = GetSystemMetrics(SM_CYCAPTION);

			GetWindowRect(hWnd, &rcW);

			if (IsDragArea(x - rcW.left - dwFrameWidth,
							y - rcW.top - dwFrameHeight * 2 - dwCaptionHeight))
			{
				return HTCAPTION;
			}
		}
		break;

   case WM_EXITMENULOOP:
     // Ignore time spent in menu
     if (m_pEngine) {
       m_pEngine->SetLastTick(timeGetTime());
     }
     break;
     
   case WM_EXITSIZEMOVE:
     // Ignore time spent resizing
     if (m_pEngine) {
       m_pEngine->SetLastTick(timeGetTime());
     }
     break;
		
   case WM_SYSCOMMAND:
     // Prevent moving/sizing and power loss in fullscreen mode
     switch (wParam) {
       case SC_MOVE:
       case SC_SIZE:
       case SC_MAXIMIZE:
       case SC_MONITORPOWER:
         if (m_pEngine && !m_pEngine->GetWindowed()) {
           return TRUE;
         }
     }
     break;
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CPlainXApp::DX_OnIdle(HWND hwnd)
{
  //update engine
  if (m_pEngine) {
    m_pEngine->UpdateEngine();
  }
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CPlainXApp::DX_OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
{
  if (state == WA_INACTIVE) {
    m_bActive = FALSE;
  } else {
    m_bActive = TRUE;
  }
  if (m_pEngine) {
    m_pEngine->SetActive(m_bActive);
  }
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CPlainXApp::DX_OnDestroy(HWND hwnd)
{
  WINDOWPLACEMENT wp;

  // Cleanup and close the app
  if (m_pEngine) {
    memset(&wp, 0, sizeof(WINDOWPLACEMENT));
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hwnd, &wp);
    
    m_pEngine->GetVars()->SetInt(L_GROUP, L_VID_VIDEO, L_VID_SCREENPOS, 0, wp.rcNormalPosition.left);
    m_pEngine->GetVars()->SetInt(L_GROUP, L_VID_VIDEO, L_VID_SCREENPOS, 1, wp.rcNormalPosition.top);
    m_pEngine->GetVars()->SetInt(L_GROUP, L_VID_VIDEO, L_VID_FULLSCREEN, 0, !m_pEngine->GetWindowed());

    m_pEngine->FreeEngine();
    delete m_pEngine;
  }
  m_pEngine = NULL;
	
  // Uninitialize the COM library
  CoUninitialize();

  // close guard pipe
  if (m_hGuardPipe != INVALID_HANDLE_VALUE) {
    CloseHandle(m_hGuardPipe);
  }

  PostQuitMessage(0);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------
BOOL CPlainXApp::IsDragArea(int xs, int ys)
{
	// set custom drag area to false for release cursor on HTCAPTION
	m_bIsCustomDragArea = FALSE;

	// custom code here
	return FALSE;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------
void CPlainXApp::OnCreateRegion(HWND hWnd)
{
	m_hRgn = NULL;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------
void CPlainXApp::DrawSystemCursor(BOOL bIsDraw)
{
	if (bIsDraw == m_bIsDrawCursor)
		return;

	m_bIsDrawCursor = bIsDraw;
	ShowCursor(m_bIsDrawCursor);
}
