#include "resource.h"
class CMyBitmap : public CBitmap
{

public:
	BOOL LoadBitmap(CString szFilename);

};
// This is the main window class.
class CMainWin : public CFrameWnd
{
  
	CDC m_memDC; // virtual window device context
	CDC m_redoDC; // virtual window device context
	CDC m_undoDC; // virtual window device context
	
	CBitmap m_bmp; // virtual window bitmap
	CBitmap m_undobmp; // virtual window bitmap
	CBitmap m_redobmp; // virtual window bitmap
	CBrush m_bkbrush; // brush for virtual window
	CMyBitmap m_mybitmap;
	
	// For Background
	CBrush m_RedBrush, m_YellowBrush, m_GreenBrush, m_BlueBrush, m_BlackBrush;
	CBrush m_OldBrush;
	CBrush HollowBrush;
	CBrush m_CustomBK;
	CBrush m_PreviousBrush, m_CurrentBrush;
	// For Foreground
	CPen m_RedPen, m_YellowPen, m_GreenPen, m_BluePen;
	CPen m_OldPen, m_CustPen;
	CPen m_CurrentPen;

	CPoint oldPoint;

	COLORREF fillCol;
	COLORREF newCol;
	COLORREF bgCol;
	COLORREF corCol;

	int m_vpX, m_vpY; // viewport extents
	int m_orgX, m_orgY; // viewport origin

	CMenu m_FloatMenu; // will hold the floating menu

	CToolBar    m_wndToolBar;
	CToolBar    m_wndBrushBar;
	CToolBar    m_wndPenFillBar;
	CToolBar    m_wndPenSizeBar;
	CToolBar    m_wndTools;
	CStatusBar  m_wndStatusBar;
	CReBar      m_wndReBar;

	// for text
	LOGFONT m_logfont;          // font in use for the document
	CFont one;

public:
	CMainWin(LPCSTR ClassName); // includes class name parameter
	afx_msg void OnPaint();

	afx_msg void CMainWin::OnUndoRedoCheck(CCmdUI *UI);
	afx_msg void OnUndoRedoID();
	
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnFileOpen();
	afx_msg void OnPrintFile();
	afx_msg void OnFileSave();
	afx_msg void OnShowSplash();
	
	afx_msg void OnText();

	// Enable the Undo / Redo menu items
	afx_msg void OnWantUndoRedo();

	// CCmdUI for Tools
	afx_msg void CMainWin::OnToolCheck(CCmdUI *UI);
	afx_msg void OnToolID();
	
	// CCmdUI for Brush
	afx_msg void CMainWin::OnBrushCheck(CCmdUI *UI);
	afx_msg void OnBrushID();

	// CCmdUI for Pen Size
	afx_msg void CMainWin::OnPenSizeCheck(CCmdUI *UI);
	afx_msg void OnPenSizeID();

	// CCmdUI for Pen Colour
	afx_msg void CMainWin::OnPenFillCheck(CCmdUI *UI);
	afx_msg void OnPenFillID();

	afx_msg void OnFixFlicker();
	afx_msg void OnReset();
	afx_msg void OnShowToolbar();
	afx_msg void OnHideToolbar();
	afx_msg void OnShowFillbar();
	afx_msg void OnHideFillbar();
	afx_msg void OnExit();
	afx_msg void OnHelp();
	afx_msg void OnTerminate();

	// Drawing Methods That We Need
	afx_msg void OnFill();
	afx_msg void OnEraser();

	// Size
	afx_msg void OnOne();
	afx_msg void OnTwo();
	afx_msg void OnThree();
	afx_msg void OnFour();
	
	// Colours
	afx_msg void OnRedPen();
	afx_msg void OnGreenPen();
	afx_msg void OnBluePen();
	afx_msg void OnYellowPen();
	afx_msg void OnBlackPen();
	
	// Brush
	afx_msg void OnRedBrush();
	afx_msg void OnGreenBrush();
	afx_msg void OnBlueBrush();
	afx_msg void OnYellowBrush();
	afx_msg void OnBlackBrush();
	afx_msg void OnWhiteBrush();
	
	afx_msg void OnLButtonDown(UINT flags, CPoint loc);
	afx_msg void OnLButtonUp(UINT flags, CPoint loc);
	afx_msg void OnRButtonDown(UINT flags, CPoint loc);
	afx_msg void OnMouseMove(UINT nFlags, CPoint loc);

	afx_msg void OnCustomSize();
	afx_msg void OnCustomBrush();
	afx_msg void OnCustomPen();
  
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnViewReset();

	void checkPen();
	void checkBrush();
	
	void setSize();
	void InitToolBars();

	// For Bitmap Writing
	BOOL WriteWindowToDIB( CString szFile, CWnd *pWnd );
	HBITMAP DIBToDDB( HANDLE hDIB );
	HANDLE DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal ) ;
	BOOL WriteDIB( CString szFile, HANDLE hDIB);
	
	DECLARE_MESSAGE_MAP()

};

// This is the application class.
class CApp : public CWinApp
{

public: 
	BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

};

class CSampleDialog : public CDialog
{

public:
	CSampleDialog(char *DialogName, CWnd *Owner) : CDialog(DialogName, Owner) {}
	
	afx_msg void OnCredits();
	
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

};

class CSampleDialog2 : public CDialog
{

public:
	CSampleDialog2(char *DialogName, CWnd *Owner) : CDialog(DialogName, Owner) {}

	BOOL OnInitDialog();

	afx_msg void OnCBLeft();
	afx_msg void OnCBRight();

	DECLARE_MESSAGE_MAP()

};

class CSampleDialog3 : public CDialog
{

	CSliderCtrl m_Slider;

public:
	CSampleDialog3(char *DialogName, CWnd *Owner) : CDialog(DialogName, Owner) {}

	BOOL OnInitDialog();

	afx_msg void OnCancel();
	afx_msg void OnOk();
	afx_msg void OnHScroll(UINT SBCode, UINT Pos, CScrollBar *SB);
	DECLARE_MESSAGE_MAP()

};

class CSampleDialog4 : public CDialog
{

public:
	CSampleDialog4(char *DialogName, CWnd *Owner) : CDialog(DialogName, Owner) {}

	BOOL OnInitDialog();

	afx_msg void OnCancel();
	afx_msg void OnOk();
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

class CSplashWnd : public CWnd
{
// Construction
protected:
	CSplashWnd();

// Attributes:
public:
	CBitmap m_bitmap;

// Operations
public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	~CSplashWnd();
	virtual void PostNcDestroy();

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;

// Generated message map functions
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
