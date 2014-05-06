#include "plainx.h"
#include "input.h"

#define KEYDOWN(name,key) (name[key] & 0x80) 


CInputManager::CInputManager()
{
  m_pDI = NULL;
  m_pDIKeyboard = NULL;
  m_pDIMouse = NULL;
  
  m_bMouse = FALSE;

  memset(m_bKeyState, 0, sizeof(m_bKeyState));
  memset(m_bKeyBuffer, 0, sizeof(m_bKeyBuffer));
  
  im_FillKeyMaps();
}


CInputManager::~CInputManager()
{
  RemoveDirectInput();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CInputManager::RemoveDirectInput()
{
  if (m_pDIMouse) {
    m_pDIMouse->Unacquire();
    m_pDIMouse->Release();
    m_pDIMouse=NULL;
  }

  if (m_pDIKeyboard) {
    m_pDIKeyboard->Unacquire();
    m_pDIKeyboard->Release();
    m_pDIKeyboard=NULL;
  }

  if (m_pDI) {
    m_pDI->Release();
    m_pDI = NULL;
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CInputManager::InitDirectInput ( HWND hWnd, BOOL bIsWindowed )
{
  HRESULT hRet;
  HINSTANCE hInst;
	
  memset(m_bKeyBuffer, 0, sizeof(m_bKeyBuffer));
	
  hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
    
  // Create DirectInput
  hRet = DirectInputCreateEx(hInst, 
                             DIRECTINPUT_VERSION, 
                             IID_IDirectInput7, 
                             (void**)&m_pDI, 
                             NULL); 
  if (FAILED(hRet)) {
    return FALSE;
  }
	
  //------------- keyboard -----------------------------------------

  // Create keyboard
  hRet = m_pDI->CreateDeviceEx(GUID_SysKeyboard, 
                               IID_IDirectInputDevice7, 
                               (void**)&m_pDIKeyboard, 
                               NULL);
  if (FAILED(hRet)) {
    return FALSE;
  }

  hRet = m_pDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
  if (FAILED(hRet)) {
    return FALSE;
  }

  hRet = m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard); 
  if (FAILED(hRet)) {
    return FALSE;
  }

  // capture keyboard
  m_pDIKeyboard->Acquire();

  //----------------- mouse ---------------------------------

  // Create mouse
  m_bMouse = FALSE;
  if (!bIsWindowed) {
    hRet = m_pDI->CreateDeviceEx(GUID_SysMouse, 
                                 IID_IDirectInputDevice7, 
                                 (void**)&m_pDIMouse, 
                                 NULL);
    if (FAILED(hRet)) {
      return FALSE;
    }
		
    // Set access level
    hRet = m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    if (FAILED(hRet)) {
      return FALSE;
    }
			
    // Set format
    hRet = m_pDIMouse->SetDataFormat(&c_dfDIMouse2); 
    if (FAILED(hRet)) {
      return FALSE;
    }

    // Capture mouse
    m_pDIMouse->Acquire();	
    m_bMouse = TRUE;
  }
  return TRUE;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CInputManager::AccessMouse(BOOL isActive)
{
  if (m_pDIMouse) {
    if (isActive) {
      m_pDIMouse->Acquire();	
    } else {
      m_pDIMouse->Unacquire();
    }
    m_bMouse = isActive;
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CInputManager::ProcessKeyboard()
{
  HRESULT hRet;

  if (!m_pDIKeyboard)
    return FALSE;

  memset(m_bKeyState, 0, sizeof(m_bKeyState));
	
  hRet = m_pDIKeyboard->GetDeviceState(sizeof(m_bKeyState), m_bKeyState);
  if (hRet==DIERR_INPUTLOST) {
    m_pDIKeyboard->Acquire();
    hRet = m_pDIKeyboard->GetDeviceState(sizeof(m_bKeyState), m_bKeyState);
  }
  
  if (FAILED(hRet)) return FALSE;
	
  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CInputManager::AccessKeyboard(BOOL isActive)
{
  if (m_pDIKeyboard) {
    if (isActive) {
      m_pDIKeyboard->Acquire();	
    } else {
      m_pDIKeyboard->Unacquire();
    }
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CInputManager::KeyPressed(BYTE key)
{
  if(KEYDOWN(m_bKeyState, key) && !m_bKeyBuffer[key]) {
    m_bKeyBuffer[key] = TRUE;
    return TRUE;
  } else if(!(KEYDOWN(m_bKeyState, key)) && m_bKeyBuffer[key]) {
    m_bKeyBuffer[key] = FALSE;
    return FALSE;
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CInputManager::KeyDown(BYTE key)
{
  return KEYDOWN(m_bKeyState, key);
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BYTE CInputManager::DIK2ASCII(BYTE nDIKCode, BOOL bIsShift)
{
  // map direct input key codes into ascii table codes
  // if not recognized return 0
  if (bIsShift) {
    return m_sKeyMapHR[nDIKCode];
  } else {
    return m_sKeyMapLR[nDIKCode];
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CInputManager::ProcessMouse() 
{
  HRESULT hr;

  if (!m_pDIMouse || !m_bMouse)
    return FALSE;
	    
  // Get the input's device state, and put the state in dims
  memset(&m_dims2, 0, sizeof(m_dims2));

  hr = m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_dims2);

  if (FAILED(hr)) {
    // DirectInput may be telling us that the input stream has been
    // interrupted.  We aren't tracking any state between polls, so
    // we don't have any special reset that needs to be done.
    // We just re-acquire and try again.
        
    // If input is lost then acquire and keep trying 
    hr = m_pDIMouse->Acquire();
    while (hr == DIERR_INPUTLOST) {
      hr = m_pDIMouse->Acquire();
    }

    // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
    // may occur when the app is minimized or in the process of 
    // switching, so just try again later 
    return FALSE; 
  }
  return TRUE;
}
	

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CInputManager::im_FillKeyMaps()
{
  // fill map with ascii codes for quick convert dik codes to ascii codes
  memset(m_sKeyMapLR, ASCII_NULL_CODE, sizeof(m_sKeyMapLR));
  memset(m_sKeyMapHR, ASCII_NULL_CODE, sizeof(m_sKeyMapHR));

  // alphaphit
  m_sKeyMapHR[DIK_A] = 'A'; m_sKeyMapLR[DIK_A] = 'a';
  m_sKeyMapHR[DIK_B] = 'B'; m_sKeyMapLR[DIK_B] = 'b';
  m_sKeyMapHR[DIK_C] = 'C'; m_sKeyMapLR[DIK_C] = 'c';
  m_sKeyMapHR[DIK_D] = 'D'; m_sKeyMapLR[DIK_D] = 'd';
  m_sKeyMapHR[DIK_E] = 'E'; m_sKeyMapLR[DIK_E] = 'e';
  m_sKeyMapHR[DIK_F] = 'F'; m_sKeyMapLR[DIK_F] = 'f';
  m_sKeyMapHR[DIK_G] = 'G'; m_sKeyMapLR[DIK_G] = 'g';
  m_sKeyMapHR[DIK_H] = 'H'; m_sKeyMapLR[DIK_H] = 'h';
  m_sKeyMapHR[DIK_I] = 'I'; m_sKeyMapLR[DIK_I] = 'i';
  m_sKeyMapHR[DIK_J] = 'J'; m_sKeyMapLR[DIK_J] = 'j';
  m_sKeyMapHR[DIK_K] = 'K'; m_sKeyMapLR[DIK_K] = 'k';
  m_sKeyMapHR[DIK_L] = 'L'; m_sKeyMapLR[DIK_L] = 'l';
  m_sKeyMapHR[DIK_M] = 'M'; m_sKeyMapLR[DIK_M] = 'm';
  m_sKeyMapHR[DIK_N] = 'N'; m_sKeyMapLR[DIK_N] = 'n';
  m_sKeyMapHR[DIK_O] = 'O'; m_sKeyMapLR[DIK_O] = 'o';
  m_sKeyMapHR[DIK_P] = 'P'; m_sKeyMapLR[DIK_P] = 'p';
  m_sKeyMapHR[DIK_Q] = 'Q'; m_sKeyMapLR[DIK_Q] = 'q';
  m_sKeyMapHR[DIK_R] = 'R'; m_sKeyMapLR[DIK_R] = 'r';
  m_sKeyMapHR[DIK_S] = 'S'; m_sKeyMapLR[DIK_S] = 's';
  m_sKeyMapHR[DIK_T] = 'T'; m_sKeyMapLR[DIK_T] = 't';
  m_sKeyMapHR[DIK_U] = 'U'; m_sKeyMapLR[DIK_U] = 'u';
  m_sKeyMapHR[DIK_V] = 'V'; m_sKeyMapLR[DIK_V] = 'v';
  m_sKeyMapHR[DIK_W] = 'W'; m_sKeyMapLR[DIK_W] = 'w';
  m_sKeyMapHR[DIK_X] = 'X'; m_sKeyMapLR[DIK_X] = 'x';
  m_sKeyMapHR[DIK_Y] = 'Y'; m_sKeyMapLR[DIK_Y] = 'y';
  m_sKeyMapHR[DIK_Z] = 'Z'; m_sKeyMapLR[DIK_Z] = 'z';
 
  // numbers
  m_sKeyMapLR[DIK_1] = '1'; m_sKeyMapHR[DIK_1] = '!';
  m_sKeyMapLR[DIK_2] = '2'; m_sKeyMapHR[DIK_2] = '@';
  m_sKeyMapLR[DIK_3] = '3'; m_sKeyMapHR[DIK_3] = '#';
  m_sKeyMapLR[DIK_4] = '4'; m_sKeyMapHR[DIK_4] = '$';
  m_sKeyMapLR[DIK_5] = '5'; m_sKeyMapHR[DIK_5] = '%';
  m_sKeyMapLR[DIK_6] = '6'; m_sKeyMapHR[DIK_6] = '^';
  m_sKeyMapLR[DIK_7] = '7'; m_sKeyMapHR[DIK_7] = '&';
  m_sKeyMapLR[DIK_8] = '8'; m_sKeyMapHR[DIK_8] = '*';
  m_sKeyMapLR[DIK_9] = '9'; m_sKeyMapHR[DIK_9] = '(';
  m_sKeyMapLR[DIK_0] = '0'; m_sKeyMapHR[DIK_0] = ')';

  // space, backspace, tab, and carriage return
  // returns as dik codes

  // others
  m_sKeyMapLR[DIK_MINUS] = '-'; m_sKeyMapHR[DIK_MINUS] = '_';
  m_sKeyMapLR[DIK_EQUALS] = '='; m_sKeyMapHR[DIK_EQUALS] = '+';
  m_sKeyMapLR[DIK_LBRACKET] = '['; m_sKeyMapHR[DIK_LBRACKET] = '{';
  m_sKeyMapLR[DIK_RBRACKET] = ']'; m_sKeyMapHR[DIK_RBRACKET] = '}';
  m_sKeyMapLR[DIK_SEMICOLON] = ';'; m_sKeyMapHR[DIK_SEMICOLON] = ':';
  m_sKeyMapLR[DIK_APOSTROPHE] = '\''; m_sKeyMapHR[DIK_APOSTROPHE] = '"';
  m_sKeyMapLR[DIK_GRAVE] = '`'; m_sKeyMapHR[DIK_GRAVE] = '~';
  m_sKeyMapLR[DIK_BACKSLASH] = '\\'; m_sKeyMapHR[DIK_BACKSLASH] = '|';
  m_sKeyMapLR[DIK_COMMA] = ','; m_sKeyMapHR[DIK_COMMA] = '<';
  m_sKeyMapLR[DIK_PERIOD] = '.'; m_sKeyMapHR[DIK_PERIOD] = '>';
  m_sKeyMapLR[DIK_SLASH] = '/'; m_sKeyMapHR[DIK_SLASH] = '?';
  m_sKeyMapLR[DIK_SPACE] = ' '; m_sKeyMapHR[DIK_SPACE] = ' ';
}