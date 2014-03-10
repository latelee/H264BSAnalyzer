/////////////////////////////////////////////////////////////////////////////
// from: http://www.codeguru.com/cpp/controls/editctrl/article.php/c539/Hex-edit-control.htm
// modified by Late Lee 2014.3.3
// CHexEdit window

class CHexEdit : public CEdit
{
// Construction
public:
	CHexEdit();

	enum EDITMODE{	
			EDIT_NONE,
			EDIT_ASCII,
			EDIT_HIGH,
			EDIT_LOW
	} ;
// Attributes
public:
	LPBYTE		m_pData;			// pointer to data
	int			m_length;			// length of data
	int			m_topindex;			// offset of first visible byte on screen

	int			m_currentAddress;	// address under cursor,Ë÷Òý
	EDITMODE	m_currentMode;		// current editing mode: address/hex/ascii
	int			m_selStart;			// start address of selection
	int			m_selEnd;			// end address of selection

	int			m_bpr;				// byte per row 
	int			m_lpp;				// lines per page
	BOOL		m_bShowAddress;
	BOOL		m_bShowAscii;
	BOOL		m_bShowHex;
	BOOL		m_bAddressIsWide;
	
	BOOL		m_bNoAddressChange;	// internally used
	BOOL		m_bHalfPage;
	
	CFont		m_Font;
	int			m_lineHeight;
	int			m_nullWidth;
	BOOL		m_bUpdate;

	int			m_offHex,
				m_offAscii,
				m_offAddress;

	CPoint		m_editPos;

    int         m_myoffset;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHexEdit)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void ResetPos(BOOL b=TRUE);
	int GetSelLength();
	int GetData(LPBYTE p, int len);
	void SetData(LPBYTE p, int len);
    // add by latelee fixme
    void SetData(LPBYTE p, int offset, int len);
	CSize GetSel(void);
	void SetSel(int s, int e);
	void SetBPR(int bpr);
	void SetOptions(BOOL a, BOOL h, BOOL c, BOOL w);
	virtual ~CHexEdit();

	// Generated message map functions
protected:
	void		ScrollIntoView(int p);
	void		RepositionCaret(int p);
	void		Move(int x, int y);
	inline BOOL IsSelected(void);
	void		UpdateScrollbars(void);
	void		CreateEditCaret(void);
	void		CreateAddressCaret(void);
	CPoint		CalcPos(int x, int y);
	void		SelInsert(int s, int l);
	void		SelDelete(int s, int e);
	inline void NormalizeSel(void);
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	//{{AFX_MSG(CHexEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt);   // org: void...
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditClear();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditUndo();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
