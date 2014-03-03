// HexEdit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HexEditCtrl.h"
#include <ctype.h>
#include <afxole.h>
#include <afxdisp.h>
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


char hextable[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
#define TOHEX(a, b)	{*b++ = hextable[a >> 4];*b++ = hextable[a&0xf];}
/////////////////////////////////////////////////////////////////////////////
// CHexEdit

CHexEdit::CHexEdit()
{
#if !defined(DEMO)
	m_pData			= NULL;		// pointer to data
	m_length		= 0;		// length of data
#else
	m_pData			= (LPBYTE)malloc(0xff);
	for(int i = 0; i < 0xff; i++)
		m_pData[i] = i;
#endif
	m_length		= 0xff;
	m_topindex		= 0;
	m_bpr			= 8;		// byte per row 
	m_lpp			= 1;
	
	m_bShowHex		= TRUE;
	m_bShowAscii	= TRUE;
	m_bShowAddress	= TRUE;
	m_bAddressIsWide= TRUE;		// 4/8 byte address
	
	m_offAddress	= 0;
	m_offHex		= 0;
	m_offAscii		= 0;
	
	m_bUpdate = TRUE;			// update font info
	m_bNoAddressChange = FALSE;
	m_currentMode = EDIT_NONE;
	
	m_editPos.x = m_editPos.y = 0;
	m_currentAddress = 0;
	m_bHalfPage = TRUE;
	
	m_selStart	= 0xffffffff;
	m_selEnd	= 0xffffffff;
	
	m_Font.CreateFont(-12, 0,0,0,0,0,0,0,0,0,0,0,0, "Courier New");
	
	AfxOleInit();
}

CHexEdit::~CHexEdit()
{
	free(m_pData);
}


BEGIN_MESSAGE_MAP(CHexEdit, CEdit)
ON_WM_CONTEXTMENU()
//{{AFX_MSG_MAP(CHexEdit)
ON_WM_CHAR()
ON_WM_KILLFOCUS()
ON_WM_PAINT()
ON_WM_SETFOCUS()
ON_WM_SIZE()
ON_WM_VSCROLL()
ON_WM_GETDLGCODE()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_KEYDOWN()
ON_WM_MOUSEWHEEL()
#if 0
ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
ON_COMMAND(ID_EDIT_CUT, OnEditCut)
ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
#endif
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHexEdit message handlers

// 重载OnPaint，处理每一行
void CHexEdit::OnPaint() 
{
	CPaintDC pdc(this); // device context for painting
	
	CRect rc;
	GetClientRect(rc);
	
	CDC	dc;
	dc.CreateCompatibleDC(CDC::FromHandle(pdc.m_ps.hdc));
	CBitmap bm;
	
	bm.CreateCompatibleBitmap(CDC::FromHandle(pdc.m_ps.hdc), rc.Width(), rc.Height());
	dc.SelectObject(bm);
	
	CBrush b;
	b.CreateSolidBrush(RGB(0xff,0xff,0xff));
	dc.FillRect(rc, &b);
	
	ASSERT(m_currentAddress >= 0);
	ASSERT(m_topindex >= 0);
	
	dc.SelectObject(m_Font);
	int		height		= 0;
	int		x,y;
	char	buf[256];
	
	x = rc.TopLeft().x;
	y = rc.TopLeft().y;
	
	dc.SetBoundsRect(&rc, DCB_DISABLE);
	
	if(m_pData)
	{
//				MessageBox("dsjf");
		//
		// get char dimensions
		//
		if(m_bUpdate)
		{
			dc.GetCharWidth('0', '0', &m_nullWidth);
			CSize sz = dc.GetTextExtent("0", 1);
			m_lineHeight = sz.cy;
			
            // 每一种类型的间隔
			m_offHex	= m_bShowAddress ? (m_bAddressIsWide ? m_nullWidth * 10 : m_nullWidth * 5) : 0;
			m_offAscii	= m_bShowAddress ? (m_bAddressIsWide ? m_nullWidth * 13 : m_nullWidth * 5) : 0;
			m_offAscii += m_bShowHex	 ? (m_bpr * 3 * m_nullWidth) : 0;
			
			m_lpp = rc.Height() / m_lineHeight;
			m_bHalfPage = FALSE;
			if(m_lpp * m_bpr > m_length)
			{
				m_lpp = (m_length + (m_bpr/2)) / m_bpr ;
				if(m_length % m_bpr != 0)
				{
					m_bHalfPage = TRUE;
					m_lpp++;
				}
			}
			m_bUpdate = FALSE;
			UpdateScrollbars();
		}
		
		TRACE("%i %i\n", m_topindex, m_selStart);
		
		height = rc.Height() / m_lineHeight;
		height *= m_lineHeight;
		
		if(m_bShowAddress)
		{
			char fmt[8] = {'%','0','8','l','X'};
			fmt[2] = m_bAddressIsWide ? '8' : '4';
			int w = m_bAddressIsWide ? 8 : 4;
			y = 0;
			CRect rcd = rc;
			rcd.TopLeft().x = m_offAddress;
            // 使用用户自定义偏移
            int tmp = m_myoffset;
			for(int	 i = m_topindex; (i < m_length) && (rcd.TopLeft().y < height); i+= m_bpr)
			{
                if (m_myoffset)
                {
                    sprintf(buf, fmt, tmp);
                    tmp += m_bpr;
                }
                else
                {
                    sprintf(buf, fmt, i);
                }
				dc.DrawText(buf, w, rcd, DT_LEFT|DT_TOP|DT_SINGLELINE|DT_NOPREFIX);
				rcd.TopLeft().y += m_lineHeight;
			}
		}
		if(m_bShowHex)
		{
			y = 0;
			CRect rcd = rc;
			rcd.TopLeft().x = x = m_offHex;
			
			if(m_selStart != 0xffffffff)
			{
				int	 i;
				int	 n = 0;
				int	 selStart = m_selStart, selEnd = m_selEnd;
				if(selStart > selEnd)
					selStart ^= selEnd ^= selStart ^= selEnd;
				
				char* p;
				for(i = m_topindex; (i < selStart) && (y < height); i++)
				{
					p = &buf[0];
					TOHEX(m_pData[i], p);
					*p++ = ' ';
					dc.TextOut(x, y, buf, 3);
					x += m_nullWidth * 3;
					n++;
					if(n == m_bpr)
					{
						n = 0;
						x = m_offHex;
						y += m_lineHeight;
					}
				}
				dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				dc.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
				for(; i <= selEnd && 
					i < m_length && y < height ; i++)
				{
					p = &buf[0];
					TOHEX(m_pData[i], p);
					*p++ = ' ';
					dc.TextOut(x, y, buf, (i==selEnd || (i+1)%m_bpr==0 || i==m_length-1) ? 2:3);
					x += m_nullWidth * 3;
					n++;
					if(n == m_bpr)
					{
						n = 0;
						x = m_offHex;
						y += m_lineHeight;
					}
				}
				dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				dc.SetBkColor(GetSysColor(COLOR_WINDOW));
				for(; (i < m_length) && (y < height); i++)
				{
					char* p = &buf[0];
					TOHEX(m_pData[i], p);
					*p++ = ' ';
					dc.TextOut(x, y, buf, 3);
					x += m_nullWidth * 3;
					n++;
					if(n == m_bpr)
					{
						n = 0;
						x = m_offHex;
						y += m_lineHeight;
					}
				}
			}
			else
			{
				for(int	 i = m_topindex; (i < m_length) && (rcd.TopLeft().y < height);)
				{
                    int n = 0;
					char* p = &buf[0];
					for(n = 0; (n < m_bpr) && (i < m_length); n++)
					{
						TOHEX(m_pData[i], p);
						*p++ = ' ';
						i++;
					}
					while(n < m_bpr)
					{
						*p++ = ' ';	*p++ = ' ';	*p++ = ' ';
						n++;
					}
					
					dc.DrawText(buf, m_bpr*3, rcd, DT_LEFT|DT_TOP|DT_SINGLELINE|DT_NOPREFIX);
					rcd.TopLeft().y += m_lineHeight;
				}
			}
		}
		if(m_bShowAscii)
		{
			y = 0;
			CRect rcd = rc;
			rcd.TopLeft().x = x = m_offAscii;
			if(m_selStart != 0xffffffff)
			{
				int	 i;
				int	 n = 0;
				int	 selStart = m_selStart, selEnd = m_selEnd;
				if(selStart > selEnd)
					selStart ^= selEnd ^= selStart ^= selEnd;
				
				for(i = m_topindex; (i < selStart) && (y < height); i++)
				{
					buf[0] = isprint(m_pData[i]) ? m_pData[i] : '.';
					dc.TextOut(x, y, buf, 1);
					x += m_nullWidth;
					n++;
					if(n == m_bpr)
					{
						n = 0;
						x = m_offAscii;
						y += m_lineHeight;
					}
				}
				dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				dc.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
				for(; (i <=selEnd) && i<m_length && (y < height); i++)
				{
					buf[0] = isprint(m_pData[i]) ? m_pData[i] : '.';
					dc.TextOut(x, y, buf, 1);
					x += m_nullWidth;
					n++;
					if(n == m_bpr)
					{
						n = 0;
						x = m_offAscii;
						y += m_lineHeight;
					}
				}
				dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				dc.SetBkColor(GetSysColor(COLOR_WINDOW));
				for(; (i < m_length) && y < height; i++)
				{
					buf[0] = isprint(m_pData[i]) ? m_pData[i] : '.';
					dc.TextOut(x, y, buf, 1);
					x += m_nullWidth;
					n++;
					if(n == m_bpr)
					{
						n = 0;
						x = m_offAscii;
						y += m_lineHeight;
					}
				}
			}
			else
			{
				for(int	 i = m_topindex; (i < m_length) && (rcd.TopLeft().y < height);)
				{
                    int n = 0;
					char* p = &buf[0];
					for(n = 0; (n < m_bpr) && (i < m_length); n++)
					{
						*p++ = isprint(m_pData[i]) ? m_pData[i] : '.';
						i++;
					}
					dc.DrawText(buf, n, rcd, DT_LEFT|DT_TOP|DT_SINGLELINE|DT_NOPREFIX);
					rcd.TopLeft().y += m_lineHeight;
				}
			}
		}
	}
	pdc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, 0, 0, SRCCOPY);

	//char ss[40];
	//sprintf(ss,"m_length:%d  m_currentAddress:%d",m_length,m_currentAddress);
	//GetParent()->SetWindowText(ss); // no need to do this

}

void CHexEdit::OnSetFocus(CWnd* pOldWnd) 
{
	if(m_pData && !IsSelected())
	{
		if(m_editPos.x == 0 && m_bShowAddress)
			CreateAddressCaret();
		else
			CreateEditCaret();
		SetCaretPos(m_editPos);
		ShowCaret();
	}
	CWnd::OnSetFocus(pOldWnd);
}

void CHexEdit::OnKillFocus(CWnd* pNewWnd) 
{
	DestroyCaret();
	CWnd::OnKillFocus(pNewWnd);
}

void CHexEdit::OnSize(UINT nType, int cx, int cy) 
{
	CEdit::OnSize(nType, cx, cy);
}
BOOL CHexEdit::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt)
{
	if(zDelta <0)
		OnVScroll(SB_LINEDOWN,m_topindex / m_bpr,0);
	else
		OnVScroll(SB_LINEUP,m_topindex / m_bpr,0);

    // new add
    return TRUE;
}

void CHexEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	pScrollBar;
	if(!m_pData)
		return;
	
	int oa = m_topindex;
	switch(nSBCode)
	{
	case SB_LINEDOWN:
		if(m_topindex < m_length - m_lpp*m_bpr)
		{
			m_topindex += m_bpr;
			Invalidate(FALSE);
		}
		break;
		
	case SB_LINEUP:
		if(m_topindex > 0)
		{
			m_topindex -= m_bpr;
			Invalidate(FALSE);
		}
		break;
		
	case SB_PAGEDOWN:
		if(m_topindex < m_length - m_lpp*m_bpr)
		{
			m_topindex += m_bpr * m_lpp;
			if(m_topindex > m_length - m_lpp*m_bpr)
				m_topindex = m_length - m_lpp*m_bpr;
			Invalidate(FALSE);
		}
		break;
		
	case SB_PAGEUP:
		if(m_topindex > 0)
		{
			m_topindex -= m_bpr * m_lpp;
			if(m_topindex < 0)
				m_topindex = 0;
			Invalidate(FALSE);
		}
		break;
		
	case SB_THUMBTRACK:
		m_topindex = nPos * m_bpr;
		Invalidate(FALSE);
		break;
	}
	
	
	
	::SetScrollPos(this->m_hWnd, SB_VERT, m_topindex / m_bpr, TRUE);
	RepositionCaret(m_currentAddress);
	
}


UINT CHexEdit::OnGetDlgCode() 
{
	return DLGC_WANTALLKEYS;
}
void CHexEdit::SetOptions(BOOL a, BOOL h, BOOL c, BOOL w)
{
	m_bShowHex		= h;
	m_bShowAscii	= c;
	m_bShowAddress	= a;
	m_bAddressIsWide= w;		// 4/8 byte address
	m_bUpdate		= TRUE;
}
void CHexEdit::SetBPR(int bpr=8)
{
	if(bpr==1 || bpr==2 || bpr==4 || bpr==8 || bpr==16)
		m_bpr = bpr;
	else
		return;
	m_bUpdate= TRUE;
}

void CHexEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	if(!m_pData)
		return;
	CPoint pt = CalcPos(point.x, point.y);
	if(pt.x > -1)//点合法
	{
		m_editPos = pt;
		pt.x *= m_nullWidth;
		pt.y *= m_lineHeight;
		
		if(pt.x == 0 && m_bShowAddress)
			CreateAddressCaret();
		else
			CreateEditCaret();
		
		
		if(nFlags & MK_SHIFT)//按下SHIFT
			m_selEnd = m_currentAddress;
		else
		{
			m_selStart= m_currentAddress;
			m_selEnd = m_selStart;
			//if(DragDetect(m_hWnd,point))//按下左键时拖动
            if(DragDetect(point))//按下左键时拖动
				SetCapture();
		}
		SetCaretPos(pt);
		ShowCaret();
		Invalidate(FALSE);
	}


}
CPoint CHexEdit::CalcPos(int x, int y)
{
	int xp,yp;
	y /= m_lineHeight;
	yp=(m_length-m_topindex)/m_bpr;
	yp=yp>m_lpp?m_lpp:yp;//当前行数

	if(y < 0 || y >yp)
		return CPoint(-1, -1);
	
	x += m_nullWidth;
	x /= m_nullWidth;
	
	
	if(m_bShowAddress && x <= (m_bAddressIsWide ? 8 : 4))
	{
		m_currentAddress = m_topindex + (m_bpr * y);
		m_currentMode = EDIT_NONE;
		return CPoint(0, y);
	}
	
	xp = (m_offHex  / m_nullWidth) + m_bpr * 3;//HEX区的右边界
	int x0=xp;

	if(y==yp && m_length%m_bpr)//在最后一行且最后一行未满
		xp-=(m_bpr-m_length%m_bpr)*3;
	if(m_bShowHex && x < xp)//当鼠标落在HEX区时计算m_currentAddress并返回CPoint(x, y)
	{
		if(x%3)
			x--;
		m_currentAddress = m_topindex + (m_bpr * y) + (x - (m_offHex  / m_nullWidth)) / 3;
		m_currentMode =  ((x%3) & 0x01) ? EDIT_LOW : EDIT_HIGH;
		return CPoint(x, y);
	}
	xp = (m_offAscii  / m_nullWidth) + m_bpr;//Ascii区右边界字符距+1

	if(y==yp && m_length%m_bpr)//在最后一行且最后一行未满
		xp-=m_bpr-(m_length%m_bpr);

	if(m_bShowAscii && --x < xp && x>=x0)//在Ascii区，x0为Ascii区左边界字符距
	{
		m_currentAddress = m_topindex + (m_bpr * y) + (x - (m_offAscii  / m_nullWidth));
		m_currentMode = EDIT_ASCII;
		return CPoint(x, y);
	}
	return CPoint(-1,-1);
}

void CHexEdit::CreateAddressCaret()
{
	DestroyCaret();
	CreateSolidCaret(m_nullWidth * (m_bAddressIsWide ? 8 : 4), m_lineHeight);
}

void CHexEdit::CreateEditCaret()
{
	DestroyCaret();
	CreateSolidCaret(m_nullWidth, m_lineHeight);
}

void CHexEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(!m_pData)
		return;
	
	if(nFlags & MK_LBUTTON && m_selStart != 0xffffffff)
	{
		CRect rc;
		GetClientRect(&rc);
		if(!rc.PtInRect(point))
		{
			if(point.y < 0)
			{
				OnVScroll(SB_LINEUP, 0, NULL);
				point.y = 0;
			}
			else if(point.y > rc.Height())
			{
				OnVScroll(SB_LINEDOWN, 0, NULL);
				point.y = rc.Height() -1;
			}
		}
		
		//
		// we are selecting
		//
		int	 seo = m_selEnd;
		CPoint pt = CalcPos(point.x, point.y);
		if(pt.x > -1)
		{
			m_selEnd = m_currentAddress;
		}
		if(IsSelected())
			DestroyCaret();
		
		if(seo!=m_selEnd)
			Invalidate(FALSE);
	}


}

void CHexEdit::UpdateScrollbars()
{
	SCROLLINFO si;
	
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = (m_length / m_bpr) - 1;
	si.nPage = m_lpp;
	si.nPos = m_topindex / m_bpr;
	
	::SetScrollInfo(this->m_hWnd, SB_VERT, &si, TRUE);
	if(si.nMax > (int)si.nPage)
		::EnableScrollBar(this->m_hWnd, SB_VERT, ESB_ENABLE_BOTH);
}


inline BOOL CHexEdit::IsSelected()
{
	return m_selStart != 0xffffffff;
}

void CHexEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(IsSelected())
		ReleaseCapture();
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CHexEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	nFlags;nRepCnt;
	if(!m_pData)
		return;
	if(nChar == '\t')
		return;
	if(GetKeyState(VK_CONTROL) & 0x80000000)
	{
		switch(nChar)
		{
		case 0x03:
			if(IsSelected())
				OnEditCopy();
			return;
		case 0x16:
			OnEditPaste();
			return;
		case 0x18:
			if(IsSelected())
				OnEditCut();
			return;
		case 0x1a:
			OnEditUndo();
			return;
		}
	}
	
	if(nChar == 0x08)
	{
		if(m_currentAddress > 0)
		{
			m_currentAddress--;
			SelDelete(m_currentAddress, m_currentAddress+1);
			RepositionCaret(m_currentAddress);
			Invalidate(FALSE);
		}
		return;
	}
	
	SetSel(-1, -1);
	switch(m_currentMode)
	{
	case EDIT_NONE:
		return;
	case EDIT_HIGH:
	case EDIT_LOW:
		if((nChar >= '0' && nChar <= '9') || (nChar >= 'a' && nChar <= 'f'))
		{
			UINT b = nChar - '0';
			if(b > 9) 
				b = 10 + nChar - 'a';
			
			if(m_currentMode == EDIT_HIGH)
			{
				m_pData[m_currentAddress] = (unsigned char)((m_pData[m_currentAddress] & 0x0f) | (b << 4));
			}
			else
			{
				m_pData[m_currentAddress] = (unsigned char)((m_pData[m_currentAddress] & 0xf0) | b);
			}
			Move(1,0);
		}
		break;
	case EDIT_ASCII:
		m_pData[m_currentAddress] = (unsigned char)nChar;
		Move(1,0);
		break;
	}
	Invalidate(FALSE);
}

void CHexEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	
	BOOL bShift = GetKeyState(VK_SHIFT) & 0x80000000;
	BOOL bCtrl=GetKeyState(VK_CONTROL) & 0x80000000;
	BOOL bac = m_bNoAddressChange;
	m_bNoAddressChange = TRUE;
	switch(nChar)
	{
	case VK_DOWN:
		Move(0,1);
		break;
	case VK_UP:
		Move(0,-1);
		break;
	case VK_LEFT:
		Move(-1,0);
		break;
	case VK_RIGHT:
		Move(1,0);
		break;
	case VK_PRIOR:
		OnVScroll(SB_PAGEUP, 0, NULL);
		break;
	case VK_NEXT:
		OnVScroll(SB_PAGEDOWN, 0, NULL);
		break;
	case VK_HOME:
		if(bCtrl)
		{
			OnVScroll(SB_THUMBTRACK, 0, NULL);
			m_currentAddress=0;
		}
		else
		{
			m_currentAddress /= m_bpr;
			m_currentAddress *= m_bpr;
		}
		Move(0,0);
		break;
	case VK_END:
		if(bCtrl)
		{
			OnVScroll(SB_THUMBTRACK, ((m_length+(m_bpr/2)) / m_bpr) - m_lpp, NULL);
			m_currentAddress = m_length-1;
		}
		else
		{
			m_currentAddress /= m_bpr;
			m_currentAddress *= m_bpr;
			m_currentAddress += m_bpr - 1;
			if(m_currentAddress > m_length)
				m_currentAddress = m_length-1;
		}
		Move(0,0);
		break;
	case VK_INSERT:
		SelInsert(m_currentAddress, max(1, m_selEnd-m_selStart));
		break;
	case VK_DELETE:
		if(IsSelected())
		{
			OnEditClear();
			Move(0,0);
		}
		break;
	case '\t':
		switch(m_currentMode)
		{
		case EDIT_NONE:
			m_currentMode = EDIT_HIGH;
			break;
		case EDIT_HIGH:
		case EDIT_LOW:
			m_currentMode = EDIT_ASCII;
			break;
		case EDIT_ASCII:
			m_currentMode = EDIT_HIGH;
			break;
		}
		Move(0,0);
		break;
	}
	if(!bShift && !bCtrl)
	{
		if(bShift)
			m_selEnd= m_currentAddress;
		else
			m_selEnd =m_selStart= m_currentAddress;
		Invalidate(FALSE);
	}
	m_bNoAddressChange = bac;
}

void CHexEdit::Move(int x, int y)
{
	switch(m_currentMode)
	{
	case EDIT_NONE:
		return;
	case EDIT_HIGH:
		if(x != 0)
			m_currentMode = EDIT_LOW;
		if(x == -1)
			m_currentAddress --;
		m_currentAddress += y* m_bpr;
		break;
	case EDIT_LOW:
		if(x != 0)
			m_currentMode = EDIT_HIGH;
		if(x == 1)
			m_currentAddress++;
		m_currentAddress += y* m_bpr;
		break;
	case EDIT_ASCII:
		{
			m_currentAddress += x;
			m_currentAddress += y*m_bpr;
		}
		break;
	}
	if(m_currentAddress < 0)
		m_currentAddress = 0;
	
	if(m_currentAddress >= m_length)
	{
		m_currentAddress -= x;
		m_currentAddress -= y*m_bpr;
	}
	m_bNoAddressChange = TRUE;
	if(m_currentAddress < m_topindex)
		OnVScroll(SB_LINEUP, 0, NULL);
	if(m_currentAddress >= m_topindex + m_lpp*m_bpr)
		OnVScroll(SB_LINEDOWN, 0, NULL);
	m_bNoAddressChange = FALSE;
	ScrollIntoView(m_currentAddress);
	
	RepositionCaret(m_currentAddress);
}

void CHexEdit::SetSel(int s, int e)
{
	DestroyCaret();
	m_selStart = s;
	m_selEnd = e;
	Invalidate(FALSE);
	if(m_editPos.x == 0 && m_bShowAddress)
		CreateAddressCaret();
	else
		CreateEditCaret();
	SetCaretPos(m_editPos);
	ShowCaret();
}

void CHexEdit::RepositionCaret(int	 p)
{
	int x, y;
	
	y =(p - m_topindex) / m_bpr;
	x = (p - m_topindex) % m_bpr;
	
	switch(m_currentMode)
	{
	case EDIT_NONE:
		CreateAddressCaret();
		x = 0;
		break;
	case EDIT_HIGH:
		CreateEditCaret();
		x *= m_nullWidth*3;
		x += m_offHex;
		break;
	case EDIT_LOW:
		CreateEditCaret();
		x *= m_nullWidth*3;
		x += m_nullWidth;
		x += m_offHex;
		break;
	case EDIT_ASCII:
		CreateEditCaret();
		x *= m_nullWidth;
		x += m_offAscii;
		break;
	}
	m_editPos.x = x;
	m_editPos.y = y*m_lineHeight;//m_lineHeight为每行的高
	CRect rc;
	GetClientRect(&rc);
	rc.top+=m_lineHeight;
	rc.bottom-=m_lineHeight;
	if(rc.PtInRect(m_editPos))
	{
		rc.bottom+=m_lineHeight;
		rc.top-=m_lineHeight;
		SetCaretPos(m_editPos);
		ShowCaret();
	}
}

void CHexEdit::ScrollIntoView(int p)
{
	if(p < m_topindex || p > m_topindex + m_lpp*m_bpr)
	{
		m_topindex = (p/m_bpr) * m_bpr;
		m_topindex -= (m_lpp / 3) * m_bpr;
		if(m_topindex < 0)
			m_topindex = 0;
		
		UpdateScrollbars();
		Invalidate(FALSE);
	}
}


void CHexEdit::OnContextMenu(CWnd*, CPoint point)
{
    // by latelee
	#if 0
	// CG: This block was added by the Pop-up Menu component
	{
		if (point.x == -1 && point.y == -1)
		{
			//keystroke invocation
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);
			
			point = rect.TopLeft();
			point.Offset(5, 5);
		}
		
		CMenu menu;
		VERIFY(menu.LoadMenu(CG_IDR_POPUP_HEX_EDIT));
		
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		
		pPopup->EnableMenuItem(ID_EDIT_UNDO, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		if(!IsSelected())
		{
			pPopup->EnableMenuItem(ID_EDIT_CLEAR, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDIT_CUT, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			pPopup->EnableMenuItem(ID_EDIT_COPY, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
		}
		{
			COleDataObject	obj;	
			if (obj.AttachClipboard()) 
			{
				if(!obj.IsDataAvailable(CF_TEXT) && !obj.IsDataAvailable(RegisterClipboardFormat("BinaryData")))
					pPopup->EnableMenuItem(ID_EDIT_PASTE, MF_GRAYED|MF_DISABLED|MF_BYCOMMAND);
			}
		}
		
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
			this);
	}
    #endif
}


void CHexEdit::OnEditClear() 
{
	SelDelete(m_selStart, m_selEnd);
}

void CHexEdit::OnEditCopy() 
{
	COleDataSource*		pSource = new COleDataSource();
	EmptyClipboard();
	int	dwLen = GetSelLength();
	HGLOBAL		hMemb = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT, dwLen);
	HGLOBAL		hMema = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT, (dwLen) * 3);
	if (!hMemb || !hMema) 
		return;
	LPBYTE	p = (BYTE*)::GlobalLock(hMemb);
	// copy binary
	memcpy(p, m_pData+m_selStart, dwLen);
	::GlobalUnlock(hMemb);
	p = (BYTE*)::GlobalLock(hMema);
	// copy ascii
	memcpy(p, m_pData+m_selStart, dwLen);
	::GlobalUnlock(hMema);
	for(int	 i = 0; i < dwLen;i++)
	{
		if(m_currentMode != EDIT_ASCII)
		{
			TOHEX(m_pData[m_selStart+i], p);
			*p++ = ' ';
		}
		else
			if(!isprint(*p))
				*p++ = '.';
	}
	pSource->CacheGlobalData(RegisterClipboardFormat("BinaryData"), hMemb);	
	pSource->CacheGlobalData(CF_TEXT, hMema);	
	pSource->SetClipboard();
}

void CHexEdit::OnEditCut() 
{
	OnEditCopy();
	SelDelete(m_selStart, m_selEnd);
	m_selStart=m_selEnd=m_currentAddress;
	Invalidate(FALSE);
}

void CHexEdit::OnEditPaste() 
{
	COleDataObject	obj;	
	if (obj.AttachClipboard()) 
	{
		HGLOBAL hmem = NULL;
		if (obj.IsDataAvailable(RegisterClipboardFormat("BinaryData"))) 
		{
			hmem = obj.GetGlobalData(RegisterClipboardFormat("BinaryData"));
		}	
		else if (obj.IsDataAvailable(CF_TEXT)) 
		{
			hmem = obj.GetGlobalData(CF_TEXT);
		}
		if(hmem)
		{
			LPBYTE	p = (BYTE*)::GlobalLock(hmem);
			DWORD	dwSizeMem=::GlobalSize(hmem);
			DWORD	dwSizeSel=GetSelLength();
			DWORD	dwLen =dwSizeMem>dwSizeSel?dwSizeSel:dwSizeMem;
			int		insert;
			
			NormalizeSel();
			if(m_selStart == 0xffffffff)
			{
				if(m_currentMode == EDIT_LOW)
					m_currentAddress++;
				insert = m_currentAddress;
				SelInsert(m_currentAddress, dwLen);
			}
			else
			{
				insert = m_selStart;
				SelDelete(m_selStart,m_selStart+dwLen-1);
				SelInsert(insert, dwLen);
			}
			memcpy(m_pData+insert, p, dwLen);
			
			m_currentAddress = insert+dwLen;
//			RepositionCaret(m_currentAddress);
			ResetPos();
			Invalidate(FALSE);
			::GlobalUnlock(hmem);
		}
	}
}

void CHexEdit::OnEditSelectAll() 
{
	m_selStart = 0;
	m_selEnd = m_length;
	DestroyCaret();
	Invalidate(FALSE);
}

void CHexEdit::OnEditUndo() 
{
	// TODO: Add your command handler code here
}

void CHexEdit::NormalizeSel()
{
	if(m_selStart > m_selEnd)
		m_selStart ^= m_selEnd ^= m_selStart ^= m_selEnd;
}

void CHexEdit::SelDelete(int s, int e)
{
	int temp;
	int l=e-s+1;
	if(e<s)
	{
		temp=s;
		s=e;
		e=temp;
	}

	LPBYTE p = (LPBYTE) malloc(m_length - l);
	memcpy(p, m_pData, s);

	if(s < m_length-l) 
		memcpy(p+s, m_pData+e+1, m_length -e);
	
	free(m_pData);
	m_pData = p;
	m_length-=l;
	m_currentAddress = s;
	if(m_currentAddress >= m_length)
		m_currentAddress--;
	m_bUpdate = TRUE;
}

void CHexEdit::SelInsert(int s, int l)
{
	LPBYTE p = (LPBYTE) malloc(m_length + l);
	memcpy(p, m_pData, s);
	memcpy(p+s+l, m_pData+s, m_length-s);
	free(m_pData);
	SetSel(-1, -1);
	m_pData = p;
	m_length = m_length+l;
	m_bUpdate = TRUE;
}

CSize CHexEdit::GetSel()
{
	return CSize(m_selStart, m_selEnd);
}

void CHexEdit::SetData(LPBYTE p, int len)
{
	free(m_pData);
	
	m_pData = (LPBYTE) malloc(len);
	memcpy(m_pData, p, len);
	
	SetSel(-1, -1);
	m_length = len;
	m_currentAddress = 0;
	m_editPos.x = m_editPos.y = 0;
	m_currentMode = EDIT_HIGH;
	m_topindex = 0;
    m_myoffset = 0;
	m_bUpdate = TRUE;
}

void CHexEdit::SetData(LPBYTE p, int offset, int len)
{
	free(m_pData);
	
	m_pData = (LPBYTE) malloc(len);
	memcpy(m_pData, p, len);
	
	SetSel(-1, -1);
	m_length = len;
	m_currentAddress = 0;
	m_editPos.x = m_editPos.y = 0;
	m_currentMode = EDIT_HIGH;
	m_topindex = 0;
    m_myoffset = offset;
	m_bUpdate = TRUE;
}

int CHexEdit::GetData(LPBYTE p, int len)
{
	memcpy(p, m_pData, min(len, m_length));
	return m_length;
}

int CHexEdit::GetSelLength()
{
	if(m_selStart==-1 || m_selEnd==-1)
		return 0;
	return m_selEnd>m_selStart ? m_selEnd-m_selStart+1:m_selStart-m_selEnd+1;
}

void CHexEdit::ResetPos(BOOL b)
{
	if(b)
	{
		m_selStart=m_selEnd=m_currentAddress;
	}
	else
	{
		m_currentAddress=m_selEnd;
	}
	RepositionCaret(m_currentAddress);
}
