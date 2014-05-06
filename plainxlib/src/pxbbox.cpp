#include "plainx.h"
#include "pxbbox.h"


CPXBBox::CPXBBox()
{
    m_bIsClean = TRUE;
    m_bIsCircle = FALSE;
    m_nRadius = 0;
    m_nRadQuad = 0;
    memset(&m_ptCenter, 0, sizeof(m_ptCenter));
    memset(&m_rcBox, 0, sizeof(m_rcBox));
}


CPXBBox::~CPXBBox()
{
}


void CPXBBox::Set(int nWLeft, int nWTop, int nWRight, int nWBottom)
{
    m_bIsCircle = FALSE;
    m_rcBox.left = nWLeft;
    m_rcBox.right = nWRight;
    m_rcBox.bottom = nWBottom;
    m_rcBox.top = nWTop;

    if (m_rcBox.left < 0) m_rcBox.left = 0;
    if (m_rcBox.bottom < 0) m_rcBox.bottom = 0;
    if (m_rcBox.right < m_rcBox.left) m_rcBox.right = m_rcBox.left;
    if (m_rcBox.top < m_rcBox.bottom) m_rcBox.top = m_rcBox.bottom;
}


void CPXBBox::Set(int nWX, int nWY, int nRadius)
{
    m_bIsCircle = TRUE;
    m_ptCenter.x = nWX;
    m_ptCenter.y = nWY;

    if (m_ptCenter.x < 0) m_ptCenter.x = 0;
    if (m_ptCenter.y < 0) m_ptCenter.y = 0;

    if (m_nRadius != nRadius) {
        m_nRadius = nRadius;
        if (m_nRadius < 0) m_nRadius = 0;
        m_nRadQuad = m_nRadius * m_nRadius;
    }
}


BOOL CPXBBox::TestPoint(int nWX, int nWY)
{
    if (m_bIsCircle) {
        int dx = nWX - m_ptCenter.x;
        int dy = nWY - m_ptCenter.y;
        BOOL bInt = ((dx * dx + dy * dy) <= m_nRadQuad) ? TRUE : FALSE;
        if (bInt) m_bIsClean = FALSE;
        return bInt;
    } else {
        int iInt = 0;
        if (nWX >= m_rcBox.left && nWX <= m_rcBox.right) iInt++;
        if (nWY >= m_rcBox.bottom && nWY <= m_rcBox.top) iInt++;
        if (iInt == 2) m_bIsClean = FALSE;
        return (iInt == 2) ? TRUE : FALSE;
    }
}


BOOL CPXBBox::TestBox(CPXBBox* pTest)
{
    int dx, dy;
    BOOL bRes = FALSE;

    if (!pTest)
        return FALSE;

    while (TRUE) {
        if (m_bIsCircle) {
            if (pTest->m_bIsCircle) {
                dx = m_ptCenter.x - pTest->m_ptCenter.x;
                dy = m_ptCenter.y - pTest->m_ptCenter.y;
                int nr = m_nRadQuad + pTest->m_nRadQuad + 2 * m_nRadius * pTest->m_nRadius;
                bRes = ((dx * dx + dy * dy) <= nr) ? TRUE : FALSE;
            } else {
                dx = m_ptCenter.x - pTest->m_rcBox.left;
                dy = m_ptCenter.y - pTest->m_rcBox.top;
                bRes = ((dx * dx + dy * dy) <= m_nRadQuad) ? TRUE : FALSE;
                if (bRes) break;
                dx = m_ptCenter.x - pTest->m_rcBox.right;
                bRes = ((dx * dx + dy * dy) <= m_nRadQuad) ? TRUE : FALSE;
                if (bRes) break;
                dx = m_ptCenter.x - pTest->m_rcBox.right;
                dy = m_ptCenter.y - pTest->m_rcBox.bottom;
                bRes = ((dx * dx + dy * dy) <= m_nRadQuad) ? TRUE : FALSE;
                if (bRes) break;
                dx = m_ptCenter.x - pTest->m_rcBox.left;
                bRes = ((dx * dx + dy * dy) <= m_nRadQuad) ? TRUE : FALSE;
            }
        } else {
            if (pTest->m_bIsCircle) {
                dx = pTest->m_ptCenter.x - m_rcBox.left;
                dy = pTest->m_ptCenter.y - m_rcBox.top;
                bRes = ((dx * dx + dy * dy) <= pTest->m_nRadQuad) ? TRUE : FALSE;
                if (bRes) break;
                dx = pTest->m_ptCenter.x - m_rcBox.right;
                bRes = ((dx * dx + dy * dy) <= pTest->m_nRadQuad) ? TRUE : FALSE;
                if (bRes) break;
                dx = pTest->m_ptCenter.x - m_rcBox.right;
                dy = pTest->m_ptCenter.y - m_rcBox.bottom;
                bRes = ((dx * dx + dy * dy) <= pTest->m_nRadQuad) ? TRUE : FALSE;
                if (bRes) break;
                dx = pTest->m_ptCenter.x - m_rcBox.left;
                bRes = ((dx * dx + dy * dy) <= pTest->m_nRadQuad) ? TRUE : FALSE;
            } else {
                bRes = TRUE;
                if (m_rcBox.left > pTest->m_rcBox.right ||
                    m_rcBox.left < pTest->m_rcBox.left &&
                    m_rcBox.right > pTest->m_rcBox.right ||
                    m_rcBox.right < pTest->m_rcBox.left)
                {
                    if (m_rcBox.top > pTest->m_rcBox.top ||
                        m_rcBox.top < pTest->m_rcBox.bottom &&
                        m_rcBox.bottom > pTest->m_rcBox.top ||
                        m_rcBox.bottom < pTest->m_rcBox.bottom)
                    {
                        bRes = FALSE;
                    }
                }
            }
        }
        break;
    }
    if (bRes) {
        m_bIsClean = FALSE;
    }
    return bRes;
}


void CPXBBox::Get(int* pnWLeft, int* pnWTop, int* pnWRight, int* pnWBottom)
{
    *pnWLeft = m_rcBox.left;
    *pnWTop = m_rcBox.top;
    *pnWRight = m_rcBox.right;
    *pnWBottom = m_rcBox.bottom;
}


void CPXBBox::Get(int* pnWX, int* pnWY, int* pnRadius)
{
    *pnWX = m_ptCenter.x;
    *pnWY = m_ptCenter.y;
    *pnRadius = m_nRadius;
}


void CPXBBox::Move(int nWDX, int nWDY)
{
    if (m_bIsCircle) {
        Set(m_ptCenter.x + nWDX, m_ptCenter.y + nWDY, m_nRadius);
    } else {
        Set(m_rcBox.left + nWDX, m_rcBox.top + nWDY, m_rcBox.right + nWDX, m_rcBox.bottom + nWDY);
    }
}
