#include "plainx.h"
#include "mapman.h"


CMapManager::CMapManager()
{
  m_nCellSizeH = DEF_CELLSIZE;
  m_nCellSizeW = DEF_CELLSIZE;
  m_nBorder = DEF_BORDER;
  m_nWWidth = 0;
  m_nWHeight = 0;
  m_nCWidth = 0;
  m_nCHeight= 0;
  m_pParent = NULL;
  m_pMap = NULL;
}


CMapManager::~CMapManager()
{
  Free();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CMapManager::Init(CSceneManager* pParent)
{
  m_pParent = pParent;
  
  Free();
  
  m_nCellSizeH = m_pParent->GetCellSizeH();
  m_nCellSizeW = m_pParent->GetCellSizeW();
  m_nBorder = m_pParent->GetBorder();
  m_nWWidth = m_pParent->GetWorldWidth();
  m_nWHeight = m_pParent->GetWorldHeight();
  m_nCWidth = mm_WorldXToCell(m_nWWidth);
  m_nCHeight = mm_WorldYToCell(m_nWHeight);

  m_pMap = (MAP_CELL**)malloc(m_nCWidth*m_nCHeight*sizeof(MAP_CELL*));
  for (int y=0; y<m_nCHeight; y++) {
    for (int x=0; x<m_nCWidth; x++) {
      m_pMap[y*m_nCWidth + x] = NULL;
    }
  }
  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMapManager::Free()
{
  for (SPRITES_HASH_MAP_ITER i=m_hmMap.begin(); i != m_hmMap.end(); i++) {
    CellSprite* pcs = (CellSprite*)(*i).second;
    if (pcs != 0) {
      free(pcs);
    }
  }
  m_hmMap.clear();

  //clear map
  if (m_pMap == NULL)
    return;

  for (int y=0; y<m_nCHeight; y++) {
    for (int x=0; x<m_nCWidth; x++) {
      MAP_CELL* pcell = m_pMap[y*m_nCWidth + x];
      if (pcell) {
        for (int z=0; z<L_DEPTH; z++) {
          SPRITES_LIST* plist = pcell->plist[z];
          if (plist) {
            plist->clear();
            delete plist;
          }
          pcell->plist[z] = NULL;
        }
        free(pcell);
      }
      m_pMap[y*m_nCWidth + x] = NULL;
    }
  }
  free(m_pMap);
  m_pMap = NULL;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSprite* CMapManager::GetFirstSprite(BOOL bUseCamera)
{
  int nCameraCX, nCameraCY;
  RECT rcView;
  CSprite* pSprite = NULL;

  if (bUseCamera) {
    nCameraCX = mm_WorldXToCell(m_pParent->GetCameraX());
    nCameraCY = mm_WorldYToCell(m_pParent->GetCameraY());
    rcView = m_pParent->GetViewRect();
  } else {
    nCameraCX = 0;
    nCameraCY = 0;
    rcView.left = 0;
    rcView.top = 0;
    rcView.right = m_nWWidth;
    rcView.bottom = m_nWHeight;
  }

  m_rcIter.left = nCameraCX;
  m_rcIter.bottom = nCameraCY;
  m_rcIter.right = m_rcIter.left + (rcView.right - rcView.left)/m_nCellSizeW;
  m_rcIter.top = m_rcIter.bottom + (rcView.bottom - rcView.top)/m_nCellSizeH;
  
  m_rcIter.left -= m_nBorder;
  m_rcIter.right += m_nBorder;
  m_rcIter.bottom -= m_nBorder;
  m_rcIter.top += m_nBorder;

  if (m_rcIter.left < 0) m_rcIter.left = 0;
  if (m_rcIter.bottom < 0) m_rcIter.bottom = 0;
  if (m_rcIter.right > m_nCWidth) m_rcIter.right = m_nCWidth;
  if (m_rcIter.top > m_nCHeight) m_rcIter.top = m_nCHeight;

  m_nXIterPos = m_rcIter.left;
  m_nYIterPos = m_rcIter.bottom;
  m_nZIterPos = 0;

  MAP_CELL* pcell = m_pMap[m_nYIterPos*m_nCWidth + m_nXIterPos];
  if (pcell) {
    SPRITES_LIST* plist = pcell->plist[m_nZIterPos];
    if (plist) {
      m_pIter = plist->begin();
      if (m_pIter != plist->end()) {
        pSprite = (CSprite*)(*m_pIter);
      }
    }
  }
  return pSprite;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSprite* CMapManager::GetNextSprite(BOOL* pbIsEnd)
{
  CSprite* pSprite = NULL;

  *pbIsEnd = FALSE;
  MAP_CELL* pcell = m_pMap[m_nYIterPos*m_nCWidth + m_nXIterPos];
  if (pcell) {
    SPRITES_LIST* plist = pcell->plist[m_nZIterPos];
    if (plist) {
      if (m_pIter != plist->end()) {
        m_pIter++;
        if (m_pIter != plist->end()) {
          pSprite = (CSprite*)(*m_pIter);
          return pSprite;
        }
      }
    }
  }
  if (pSprite == NULL || pcell == NULL) {
    m_nXIterPos++;
    if (m_nXIterPos >= m_rcIter.right) {
      m_nXIterPos = m_rcIter.left;
      m_nYIterPos++;
      if (m_nYIterPos >= m_rcIter.top) {
        m_nYIterPos = m_rcIter.bottom;
        m_nZIterPos++;
        if (m_nZIterPos >= L_DEPTH) {
          *pbIsEnd = TRUE;
          return NULL;
        }
      }
    }
  }
  
  pcell = m_pMap[m_nYIterPos*m_nCWidth + m_nXIterPos];
  if (pcell) {
    SPRITES_LIST* plist = pcell->plist[m_nZIterPos];
    if (plist != NULL) {
      m_pIter = plist->begin();
      if (m_pIter != plist->end()) {
        pSprite = (CSprite*)(*m_pIter);
      }
    }
  }
  return pSprite;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CSprite* CMapManager::GetSprite(const char* pcszName)
{
  CSprite* pSprite = NULL;

  CellSprite* pcs = m_hmMap[pcszName];
  if (pcs != NULL) {
    pSprite = pcs->pSprite;
  }
  return pSprite;
}
  
//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CMapManager::AddSprite(CSprite* pSprite)
{
  CellSprite* pcs = (CellSprite*)malloc(sizeof(CellSprite));

  pcs->pSprite = pSprite;
  pcs->nCXPos = mm_WorldXToCell(pSprite->GetWXPos());
  pcs->nCYPos = mm_WorldYToCell(pSprite->GetWYPos());
  pcs->nCZPos = pSprite->GetZPos();

  m_hmMap[pSprite->GetName()] = pcs;
  mm_AddToList(pcs);
  
  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
BOOL CMapManager::DelSprite(const char* pcszName)
{
  CellSprite* pcs;

  pcs = m_hmMap[pcszName];
  if (pcs != NULL) {
    mm_RemoveFromList(pcs);
    m_hmMap.erase(pcszName);
  }
  return TRUE;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMapManager::OnChangedSpritePos(const char* pcszName)
{
  CellSprite* pcs;
  int nCXPos, nCYPos, nCZPos;

  pcs = m_hmMap[pcszName];
  if (pcs == NULL)
    return;

  nCXPos = mm_WorldXToCell(pcs->pSprite->GetWXPos());
  nCYPos = mm_WorldYToCell(pcs->pSprite->GetWYPos());
  nCZPos = pcs->pSprite->GetZPos();
  
  if (nCXPos != pcs->nCXPos ||
      nCYPos != pcs->nCYPos ||
      nCZPos != pcs->nCZPos) 
  {
    mm_RemoveFromList(pcs);
    pcs->nCXPos = nCXPos;
    pcs->nCYPos = nCYPos;
    pcs->nCZPos = nCZPos;
    mm_AddToList(pcs);
  }
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
int CMapManager::mm_WorldXToCell(int nWXPos)
{
  return (int)(nWXPos/m_nCellSizeW);
}

int CMapManager::mm_WorldYToCell(int nWYPos)
{
  return (int)(nWYPos/m_nCellSizeH);
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMapManager::mm_CellToWorld(int nCX, int nCY, RECT* prcWorld)
{
  prcWorld->bottom = nCY*m_nCellSizeH;
  prcWorld->left = nCX*m_nCellSizeW;
  prcWorld->right = prcWorld->left + m_nCellSizeW;
  prcWorld->top = prcWorld->bottom + m_nCellSizeH;
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMapManager::mm_RemoveFromList(CellSprite* pcs)
{
  SPRITES_LIST_ITER iter;
  MAP_CELL* pcell = m_pMap[pcs->nCYPos*m_nCWidth + pcs->nCXPos];
  if (pcell == NULL)
    return;

  CSprite* pSprite = NULL;
  SPRITES_LIST* plist = pcell->plist[pcs->nCZPos];
  for (iter = plist->begin(); iter != plist->end(); iter++) {
    pSprite = (CSprite*)(*iter);
    if (pSprite) {
      if (strcmp(pSprite->GetName(), pcs->pSprite->GetName()) == 0) {
        plist->erase(iter);
        break;
      }
    }
  }
  BOOL bEmptyCell = TRUE;
  for (int j=0; j<L_DEPTH && bEmptyCell; j++) {
    plist = pcell->plist[j];
    if (plist != NULL) {
      bEmptyCell = bEmptyCell && plist->empty();
    }
  }
  if (bEmptyCell) {
    for (int j=0; j<L_DEPTH; j++) {
      plist = pcell->plist[j];
      if (plist != NULL) {
        delete plist;
      }
    }
    free(pcell);
    m_pMap[pcs->nCYPos*m_nCWidth + pcs->nCXPos] = NULL;
  }
}

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMapManager::mm_AddToList(CellSprite* pcs)
{
  MAP_CELL* pcell = m_pMap[pcs->nCYPos*m_nCWidth + pcs->nCXPos];
  if (pcell == NULL) {
    pcell = (MAP_CELL*)malloc(sizeof(MAP_CELL));
    for (int z=0; z<L_DEPTH; z++) {
      pcell->plist[z] = NULL;
    }
    m_pMap[pcs->nCYPos*m_nCWidth + pcs->nCXPos] = pcell;
  }
  SPRITES_LIST* plist = pcell->plist[pcs->nCZPos];
  if (plist == NULL) {
    plist = new SPRITES_LIST;
    pcell->plist[pcs->nCZPos] = plist;
  }
  plist->push_back(pcs->pSprite);
}

