//**************************************************************************
// Paint.cpp : implementation file for Paint 2000
//**************************************************************************
#include "StdAfx.h"
#include "paint.h"
#include "resource.h"
#include "resrc1.h"
#include "credits.h"

//**************************************************************************
// Variables needed for two or more classes
//**************************************************************************
char globalTEXT[1025];
int maxX, maxY; // screen dimensions
int rect1, rect2; // LButtonDown point.x/y
int globalBRUSH, globalPEN, globalSIZE; // remember which one is selected
int dothis, cbcheck, firsterase, fromlup = 0;
int retval, redraw, fixflicker, i_saveDC;
int DlgActive, redo, undo, doundoredo;
int timer = 0; // simple timer for our splash screen
UINT ui_tool = IDM_LINE; // stores current ID
UINT ui_brush = IDM_WHITEBRUSH; // stores current ID
UINT ui_pensize = IDM_ONE; // stores current ID
UINT ui_penfill = IDM_BLACKPEN; // stores current ID
UINT ui_undoredo; // check none
CPen eraserPEN; // used for our pen
HCURSOR hCursor; // recycled for our cursor
CWinApp *pApp = AfxGetApp(); // to get the appname
// for our filter in the two FileDialog boxes
static char BASED_CODE szFilter[] = "Bmp Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"; 
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,		// capslock indicator
	ID_INDICATOR_NUM,		// numlock indicator
	ID_INDICATOR_SCRL,		// scrolllock indicator
};
CDC printDC;

//**************************************************************************
// This is the Frame's message map.
//**************************************************************************
BEGIN_MESSAGE_MAP(CMainWin, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()

	ON_COMMAND(ID_EDIT_UNDO, OnUndoRedoID)
	ON_COMMAND(ID_EDIT_REDO, OnUndoRedoID)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUndoRedoCheck)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUndoRedoCheck)

	ON_COMMAND(ID_FILE_NEW, OnReset)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT, OnPrintFile)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_OPTIONS_FIXFLICKERING, OnFixFlicker)
	ON_COMMAND(ID_OPTIONS_SHOWSPLASHSCREEN, OnShowSplash)
	ON_COMMAND(IDM_RESET, OnReset)
	ON_COMMAND(IDM_OPTION_SHOWTOOLBAR, OnShowToolbar)
	ON_COMMAND(IDM_OPTION_SHOWFILLBAR, OnShowFillbar)
	ON_COMMAND(IDM_OPTION_HIDETOOLBAR, OnHideToolbar)
	ON_COMMAND(IDM_OPTION_HIDEFILLBAR, OnHideFillbar)
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_COMMAND(IDM_HELP, OnHelp)
	ON_COMMAND(IDM_TERMINATE, OnTerminate)
	ON_COMMAND(IDM_VIEW_ZOOMIN, OnZoomIn)
	ON_COMMAND(IDM_VIEW_ZOOMOUT, OnZoomOut)
	ON_COMMAND(IDM_VIEW_RESET, OnViewReset)

	ON_COMMAND(ID_OPTIONS_ENABLEUNDOREDO, OnWantUndoRedo)

	// For tools
	ON_COMMAND(IDM_LINE, OnToolID)
	ON_COMMAND(IDM_RECTANGLE, OnToolID)
	ON_COMMAND(IDM_HALLOW_RECTANGLE, OnToolID)
	ON_COMMAND(IDM_ROUNDRECT, OnToolID)
	ON_COMMAND(IDM_HALLOW_ROUNDRECT, OnToolID)
	ON_COMMAND(IDM_ELLIPSE, OnToolID)
	ON_COMMAND(IDM_HALLOW_ELLIPSE, OnToolID)
	ON_COMMAND(IDM_OVAL, OnToolID)
	ON_COMMAND(IDM_HALLOW_OVAL, OnToolID)
	ON_COMMAND(IDM_ARC, OnToolID)
	ON_COMMAND(IDM_PIE, OnToolID)
	ON_COMMAND(IDM_HALLOW_PIE, OnToolID)
	ON_COMMAND(IDM_FREEHAND, OnToolID)
	ON_COMMAND(IDM_FILL, OnToolID)
	ON_COMMAND(IDM_TEXT, OnToolID)
	ON_COMMAND(IDM_ERASER, OnToolID)

	
	ON_UPDATE_COMMAND_UI(IDM_LINE, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_RECTANGLE, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_HALLOW_RECTANGLE, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_ROUNDRECT, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_HALLOW_ROUNDRECT, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_ELLIPSE, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_HALLOW_ELLIPSE, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_OVAL, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_HALLOW_OVAL, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_ARC, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_PIE, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_HALLOW_PIE, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_FREEHAND, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_FILL, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_TEXT, OnToolCheck)
	ON_UPDATE_COMMAND_UI(IDM_ERASER, OnToolCheck)
	
	// For brush
	ON_COMMAND(IDM_REDBRUSH, OnBrushID)
	ON_COMMAND(IDM_GREENBRUSH, OnBrushID)
	ON_COMMAND(IDM_BLUEBRUSH, OnBrushID)
	ON_COMMAND(IDM_YELLOWBRUSH, OnBrushID)
	ON_COMMAND(IDM_BLACKBRUSH, OnBrushID)
	ON_COMMAND(IDM_WHITEBRUSH, OnBrushID)
	ON_COMMAND(IDM_CUSTOM_BRUSH, OnBrushID)

	ON_UPDATE_COMMAND_UI(IDM_REDBRUSH, OnBrushCheck)
	ON_UPDATE_COMMAND_UI(IDM_GREENBRUSH, OnBrushCheck)
	ON_UPDATE_COMMAND_UI(IDM_BLUEBRUSH, OnBrushCheck)
	ON_UPDATE_COMMAND_UI(IDM_YELLOWBRUSH, OnBrushCheck)
	ON_UPDATE_COMMAND_UI(IDM_BLACKBRUSH, OnBrushCheck)
	ON_UPDATE_COMMAND_UI(IDM_WHITEBRUSH, OnBrushCheck)
	ON_UPDATE_COMMAND_UI(IDM_CUSTOM_BRUSH, OnBrushCheck)

	// For pen size
	ON_COMMAND(IDM_ONE, OnPenSizeID)
	ON_COMMAND(IDM_TWO, OnPenSizeID)
	ON_COMMAND(IDM_THREE, OnPenSizeID)
	ON_COMMAND(IDM_FOUR, OnPenSizeID)
	ON_COMMAND(IDM_CUSTOM_SIZE, OnPenSizeID)

	ON_UPDATE_COMMAND_UI(IDM_ONE, OnPenSizeCheck)
	ON_UPDATE_COMMAND_UI(IDM_TWO, OnPenSizeCheck)
	ON_UPDATE_COMMAND_UI(IDM_THREE, OnPenSizeCheck)
	ON_UPDATE_COMMAND_UI(IDM_FOUR, OnPenSizeCheck)
	ON_UPDATE_COMMAND_UI(IDM_CUSTOM_SIZE, OnPenSizeCheck)

	// For pen colour
	ON_COMMAND(IDM_REDPEN, OnPenFillID)
	ON_COMMAND(IDM_GREENPEN, OnPenFillID)
	ON_COMMAND(IDM_BLUEPEN, OnPenFillID)
	ON_COMMAND(IDM_YELLOWPEN, OnPenFillID)
	ON_COMMAND(IDM_BLACKPEN, OnPenFillID)
	ON_COMMAND(IDM_CUSTOM_PEN, OnPenFillID)

	ON_UPDATE_COMMAND_UI(IDM_REDPEN, OnPenFillCheck)
	ON_UPDATE_COMMAND_UI(IDM_GREENPEN, OnPenFillCheck)
	ON_UPDATE_COMMAND_UI(IDM_BLUEPEN, OnPenFillCheck)
	ON_UPDATE_COMMAND_UI(IDM_YELLOWPEN, OnPenFillCheck)
	ON_UPDATE_COMMAND_UI(IDM_BLACKPEN, OnPenFillCheck)
	ON_UPDATE_COMMAND_UI(IDM_CUSTOM_PEN, OnPenFillCheck)

END_MESSAGE_MAP()

//**************************************************************************
// CMainWin::CMainWin(LPCSTR ClassName)
// - Constructor for CMainWin
//**************************************************************************
CMainWin::CMainWin(LPCSTR ClassName)
{
	
	// Create the window
	Create(ClassName, "MB Software - Paint 2000 v1.0",
         WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, rectDefault,
         NULL, "ToolbarMenu");

	// Load accelerator table
	if(!LoadAccelTable("ToolbarMenu"))
		MessageBox("Cannot Load Accelerators", "Error");

	// Get Screen System Metrics
	maxX = GetSystemMetrics(SM_CXSCREEN);
	maxY = GetSystemMetrics(SM_CYSCREEN);

	// Create Client DC
	CClientDC DC(this);
     
	// create a virtual output window
	m_memDC.CreateCompatibleDC(&DC);
	m_bmp.CreateCompatibleBitmap(&DC, maxX, maxY);
	m_memDC.SelectObject(&m_bmp);

	// use standard background
	m_bkbrush.CreateStockObject(WHITE_BRUSH);
	m_memDC.SelectObject(&m_bkbrush);
	// paint background of virtual window
	m_memDC.PatBlt(0, 0, maxX, maxY, PATCOPY);

	m_redoDC.CreateCompatibleDC(&DC);
	m_undoDC.CreateCompatibleDC(&DC);
	m_redoDC.SelectObject(&m_bkbrush);
	m_undoDC.SelectObject(&m_bkbrush);
	m_redoDC.PatBlt(0, 0, maxX, maxY, PATCOPY);
	m_undoDC.PatBlt(0, 0, maxX, maxY, PATCOPY);

	// create pens
	m_RedPen.CreatePen(PS_SOLID, 1, RGB(255,0,0));
	m_GreenPen.CreatePen(PS_SOLID, 1, RGB(0,255,0));
	m_BluePen.CreatePen(PS_SOLID, 1, RGB(0,0,255));
	m_YellowPen.CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
	m_OldPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	m_CurrentPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	corCol = RGB(0, 0, 0);

	// create brushes
	m_RedBrush.CreateSolidBrush(RGB(255,0,0));
	m_GreenBrush.CreateSolidBrush(RGB(0,255,0));
	m_BlueBrush.CreateSolidBrush(RGB(0,0,255));
	m_YellowBrush.CreateSolidBrush(RGB(255, 255, 0));
	m_BlackBrush.CreateStockObject(BLACK_BRUSH);
	m_OldBrush.CreateStockObject(WHITE_BRUSH);
	
	m_PreviousBrush.CreateStockObject(WHITE_BRUSH);
	m_CurrentBrush.CreateStockObject(WHITE_BRUSH);

	// display, but don't fill, rectangles
	HollowBrush.CreateStockObject(HOLLOW_BRUSH);

	// Load the toolbars
	dothis = 0;
	globalPEN = 5;
	cbcheck = 1;

	redraw = redo = undo = doundoredo = 0;

	firsterase = 1;

	// initialize viewport extents and origin
	m_vpX = m_vpY = 10;
	m_orgX = m_orgY = 0;

	// Create the status bar
	if (!m_wndStatusBar.CreateEx(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
	}
	
	// Create Main Toolbar
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
	}

	// Create Pen Size Toolbar
	if (!m_wndPenSizeBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndPenSizeBar.LoadToolBar(IDR_PENLINE))
	{
		TRACE0("Failed to create toolbar\n");
	}

	// Create Pen Colour Toolbar
	if (!m_wndPenFillBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndPenFillBar.LoadToolBar(IDR_PENFILL))
	{
		TRACE0("Failed to create toolbar\n");
	}

	// Create Background Colour Toolbar
	if (!m_wndBrushBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER) ||
		!m_wndBrushBar.LoadToolBar(IDR_BRUSHFILL))
	{
		TRACE0("Failed to create toolbar\n");
	}
	
	// Create Tools Toolbar
	if (!m_wndTools.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_LEFT
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER) ||
		!m_wndTools.LoadToolBar(IDR_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
	}
	
	// Initialize toolbar's buttons
	InitToolBars();
	
	// Create the rebar control and add the toolbar
	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndPenSizeBar) ||
		!m_wndReBar.AddBar(&m_wndPenFillBar))
	{
		TRACE0("Failed to create rebar\n");
	}

	// Delete this if you don't want docking enabled
	m_wndTools.EnableDocking(CBRS_ALIGN_ANY);
	m_wndBrushBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndBrushBar);
	DockControlBar(&m_wndTools);

	// Set the brush to blue
	globalBRUSH = 1;

	checkBrush();

}

//**************************************************************************
// Initialize Toolbar Buttons.
//**************************************************************************
void CMainWin::InitToolBars()
{

	// Button info for Tools
	m_wndTools.SetButtonInfo( 0, IDM_LINE, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 0 );
	m_wndTools.SetButtonInfo( 1, IDM_HALLOW_RECTANGLE, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 1 );
	m_wndTools.SetButtonInfo( 2, IDM_RECTANGLE, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 2 );
	m_wndTools.SetButtonInfo( 3, IDM_HALLOW_ROUNDRECT, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 3 );
	m_wndTools.SetButtonInfo( 4, IDM_ROUNDRECT, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 4 );
	m_wndTools.SetButtonInfo( 5, IDM_HALLOW_ELLIPSE, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 5 );
	m_wndTools.SetButtonInfo( 6, IDM_ELLIPSE, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 6 );
	m_wndTools.SetButtonInfo( 7, IDM_HALLOW_OVAL, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 7 );
	m_wndTools.SetButtonInfo( 8, IDM_OVAL, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 8 );
	m_wndTools.SetButtonInfo( 9, IDM_ARC, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 9 );
	m_wndTools.SetButtonInfo( 10, IDM_HALLOW_PIE, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 10 );
	m_wndTools.SetButtonInfo( 11, IDM_PIE, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 11 );
	m_wndTools.SetButtonInfo( 12, IDM_FREEHAND, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 12 );
	m_wndTools.SetButtonInfo( 13, IDM_ERASER, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 13 );
	m_wndTools.SetButtonInfo( 14, IDM_FILL, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 14 );
	m_wndTools.SetButtonInfo( 15, IDM_TEXT, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 15 );
	m_wndTools.SetWindowText(("Tools"));

	// Button info for Brush Bar
	m_wndBrushBar.SetButtonInfo( 0, IDM_WHITEBRUSH, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 0 );
	m_wndBrushBar.SetButtonInfo( 1, IDM_REDBRUSH, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 1 );
	m_wndBrushBar.SetButtonInfo( 2, IDM_GREENBRUSH, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 2 );
	m_wndBrushBar.SetButtonInfo( 3, IDM_BLUEBRUSH, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 3 );
	m_wndBrushBar.SetButtonInfo( 4, IDM_YELLOWBRUSH, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 4 );
	m_wndBrushBar.SetButtonInfo( 5, IDM_BLACKBRUSH, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 5 );
	m_wndBrushBar.SetButtonInfo( 6, IDM_CUSTOM_BRUSH, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 6 );
	m_wndBrushBar.SetWindowText(("Background Colours"));

	// Button info for Pen Size Bar
	m_wndPenSizeBar.SetButtonInfo( 0, IDM_ONE, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 0 );
	m_wndPenSizeBar.SetButtonInfo( 1, IDM_TWO, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 1 );
	m_wndPenSizeBar.SetButtonInfo( 2, IDM_THREE, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 2 );
	m_wndPenSizeBar.SetButtonInfo( 3, IDM_FOUR, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 3 );
	m_wndPenSizeBar.SetButtonInfo( 4, IDM_CUSTOM_SIZE, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 4 );
	m_wndPenSizeBar.SetWindowText(("Pen Sizes"));

	// Button info for Pen Fill Bar
	m_wndPenFillBar.SetButtonInfo( 0, IDM_REDPEN, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 0 );
	m_wndPenFillBar.SetButtonInfo( 1, IDM_GREENPEN, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 1 );
	m_wndPenFillBar.SetButtonInfo( 2, IDM_BLUEPEN, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 2 );
	m_wndPenFillBar.SetButtonInfo( 3, IDM_YELLOWPEN, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 3 );
	m_wndPenFillBar.SetButtonInfo( 4, IDM_BLACKPEN, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 4 );
	m_wndPenFillBar.SetButtonInfo( 5, IDM_CUSTOM_PEN, TBBS_CHECKBOX|TBBS_CHECKGROUP     , 5 );
	m_wndPenFillBar.SetWindowText(("Pen Colours"));

}

//**************************************************************************
// For the text
//**************************************************************************
afx_msg void CMainWin::OnText()
{

	CFontDialog *fontDlg = new CFontDialog();

	retval = fontDlg->DoModal();

	if (retval == IDOK) 
	{
		
		one.DeleteObject();
		fontDlg->GetCurrentFont(&m_logfont);
		one.CreateFontIndirect(&m_logfont);

		m_memDC.SelectObject(&one);

	}

}

//**************************************************************************
// Set redo and undo flags to 1
//**************************************************************************
afx_msg void CMainWin::OnWantUndoRedo()
{

	if (doundoredo)
	{
		doundoredo = 0;
	}
	else
	{
		doundoredo = 1;
	}

}

//**************************************************************************
// Check for Undo / Redo ID
//**************************************************************************
afx_msg void CMainWin::OnUndoRedoID()
{

	ui_undoredo = LOWORD(GetCurrentMessage()->wParam);

	switch (ui_undoredo)
	{
	case ID_EDIT_UNDO:
		undo = 1;
		OnUndo();
		break;
	case ID_EDIT_REDO:
		redo = 1;
		OnRedo();
		break;
	}

}

//**************************************************************************
// Check the used one
//**************************************************************************
afx_msg void CMainWin::OnUndoRedoCheck(CCmdUI *UI)
{

	UI->SetCheck(UI->m_nID == ui_undoredo);

}

//**************************************************************************
// File Open Method.
//**************************************************************************
afx_msg void CMainWin::OnFileOpen()
{

	CFileDialog *fileDlg = new CFileDialog(TRUE, NULL, "*.bmp", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL );

	retval = fileDlg->DoModal();

	CString filePath;

	if (retval == IDOK) 
	{

		filePath = fileDlg->GetPathName();
	
		m_mybitmap.LoadBitmap(filePath);

		m_memDC.SelectObject(&m_mybitmap);
		InvalidateRect(NULL);

	}

}

//**************************************************************************
// Print File Method.
//**************************************************************************
afx_msg void CMainWin::OnPrintFile()
{
	
	CPrintDialog *printDlg = new CPrintDialog(TRUE, PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION, NULL);
	
	retval = printDlg->DoModal();

	if (retval == IDOK) 
	{

		MessageBox("Place code for printing here.");

	}

}

//**************************************************************************
// File Save Method.
//**************************************************************************
afx_msg void CMainWin::OnFileSave()
{
	
	CFileDialog *fileDlg = new CFileDialog(TRUE, NULL, "*.bmp", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL );

	retval = fileDlg->DoModal();

	CString filePath;

	if (retval == IDOK) 
	{

		filePath = fileDlg->GetPathName();
		
		WriteWindowToDIB(filePath, this);
		
	}

}

//**************************************************************************
// Bitmap WriteWindowToDIB Method.
// - Modified it so it will only save the client area.
// - FIXED!!! getting m_memDC and saving that to a file
//**************************************************************************
BOOL CMainWin::WriteWindowToDIB( CString szFile, CWnd *pWnd )
{
	CBitmap 	bitmap;
	CClientDC	dc(pWnd);
	CDC 		memDC;
	CRect		rect;

	memDC.CreateCompatibleDC(&m_memDC); 

	CWnd::GetClientRect(rect);

	bitmap.CreateCompatibleBitmap(&m_memDC, maxX/*rect.Width()*/,maxY/*rect.Height() */);
	
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, maxX/*rect.Width()*/,maxY/*rect.Height()*/, &m_memDC, 0, 0, SRCCOPY); 

	// Create logical palette if device support a palette
	CPalette pal;
	if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;

		pLP->palNumEntries = 
			GetSystemPaletteEntries( dc, 0, 255, pLP->palPalEntry );

		// Create the palette
		pal.CreatePalette( pLP );

		delete[] pLP;
	}

	memDC.SelectObject(pOldBitmap);

	// Convert the bitmap to a DIB
	HANDLE hDIB = DDBToDIB( bitmap, BI_RGB, &pal );

	if( hDIB == NULL )
		return FALSE;

	// Write it to file
	WriteDIB( szFile, hDIB );

	// Free the memory allocated by DDBToDIB for the DIB
	GlobalFree( hDIB );
	return TRUE;
}

//**************************************************************************
// WriteDIB		- Writes a DIB to file
// Returns		- TRUE on success
// szFile		- Name of file to write to
// hDIB			- Handle of the DIB
// - Modified this so that it will accept CString instead of regular characters
//**************************************************************************
BOOL CMainWin::WriteDIB( CString szFile, HANDLE hDIB)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	CFile file;
	if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
		return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );

	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) );

	return TRUE;
}


//**************************************************************************
// Convert DIB to DDB
//**************************************************************************
HBITMAP CMainWin::DIBToDDB( HANDLE hDIB )
{
	LPBITMAPINFOHEADER	lpbi;
	HBITMAP 		hbm;
	CPalette		pal;
	CPalette*		pOldPal;
	CClientDC		dc(NULL);

	if (hDIB == NULL)
		return NULL;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = lpbi->biClrUsed ? lpbi->biClrUsed : 
						1 << lpbi->biBitCount;

	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + 
			bmInfo.bmiHeader.biClrUsed) + 
			((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	// Create and select a logical palette if needed
	if( nColors <= 256 && dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;

		for( int i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
			pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}

		pal.CreatePalette( pLP );

		delete[] pLP;

		// Select and realize the palette
		pOldPal = dc.SelectPalette( &pal, FALSE );
		dc.RealizePalette();
	}

	
	hbm = CreateDIBitmap(dc.GetSafeHdc(),		// handle to device context
			(LPBITMAPINFOHEADER)lpbi,	// pointer to bitmap info header 
			(LONG)CBM_INIT,			// initialization flag
			lpDIBBits,			// pointer to initialization data 
			(LPBITMAPINFO)lpbi,		// pointer to bitmap info
			DIB_RGB_COLORS );		// color-data usage 

	if (pal.GetSafeHandle())
		dc.SelectPalette(pOldPal,FALSE);

	return hbm;
}

//**************************************************************************
// DDBToDIB		- Creates a DIB from a DDB
// bitmap		- Device dependent bitmap
// dwCompression	- Type of compression - see BITMAPINFOHEADER
// pPal			- Logical palette
//**************************************************************************
HANDLE CMainWin::DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal ) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;


	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 ) 
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
						* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);
		
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	::ReleaseDC(NULL,hDC);
	return hDIB;
}

//**************************************************************************
// Undo changes
// - This implement a save file and reopen it when we need to undo the last 
//	changes
//**************************************************************************
afx_msg void CMainWin::OnUndo()
{

	if (undo) 
	{
	
		m_mybitmap.DeleteObject();
		m_mybitmap.LoadBitmap("undo.bmp");
		m_memDC.SelectObject(&m_mybitmap);
	
	}

	InvalidateRect(NULL);

}

//**************************************************************************
// Redo changes
// - This implement a save file and reopen it when we need to redo the last
//	changes
//**************************************************************************
afx_msg void CMainWin::OnRedo()
{

	if (redo)
	{

		m_mybitmap.DeleteObject();
		m_mybitmap.LoadBitmap("redo.bmp");
		m_memDC.SelectObject(&m_mybitmap);

	}

	InvalidateRect(NULL);

}

//**************************************************************************
// Update screen using contents of virtual window.
//**************************************************************************
afx_msg void CMainWin::OnPaint()
{

	if (redraw)
	{
	//	SetRedraw(FALSE);
	}

	CPaintDC DC(this);

	// set mapping mode, window and viewport extents
	//DC.SetMapMode(MM_ISOTROPIC);
	//DC.SetWindowExt(200, 200);
	DC.SetViewportExt(m_vpX, m_vpY);
	DC.SetViewportOrg(m_orgX, m_orgY);
	
	DC.BitBlt(0, 0, maxX, maxY, &m_memDC, 0, 0, SRCCOPY);

	//SetRedraw(TRUE);
	
	redraw = 0;
	redo = undo = 0;

}

//**************************************************************************
// Process IDM_EXIT.
//**************************************************************************
afx_msg void CMainWin::OnExit()
{
  int response;

  response = MessageBox("Quit the Program?",
                        "Exit", MB_YESNO);

  if(response == IDYES)
    SendMessage(WM_CLOSE); // terminate app
}

//**************************************************************************
// Process IDM_HELP.
//**************************************************************************
afx_msg void CMainWin::OnHelp()
{
	
	CSampleDialog diagOb("SampleDialog", this);
	
	diagOb.DoModal();


}

//**************************************************************************
// Process Show Splash Screen
//**************************************************************************
afx_msg void CMainWin::OnShowSplash()
{

	CSplashWnd::EnableSplashScreen(TRUE);
	CSplashWnd::ShowSplashScreen(NULL);

}

//**************************************************************************
// Process IDM_TERMINATE
//**************************************************************************
afx_msg void CMainWin::OnTerminate()
{
	
	CSampleDialog2 diagOb("SampleDialog2", this);
	
	diagOb.DoModal();


}

//**************************************************************************
// fix the flickering window
//**************************************************************************
afx_msg void CMainWin::OnFixFlicker()
{
	
	if (fixflicker) 
	{
		fixflicker = 0;
	}
	else
	{
		fixflicker = 1;
	}

}

//**************************************************************************
// Reset screen coordinates and erase window.
//**************************************************************************
afx_msg void CMainWin::OnReset()
{
	
	m_memDC.SelectObject(&m_bkbrush);
	m_memDC.PatBlt(0, 0, maxX, maxY, PATCOPY);
	
	checkBrush();

	InvalidateRect(NULL);

}

//**************************************************************************
// Show Tool Bar.
//**************************************************************************
afx_msg void CMainWin::OnShowToolbar()
{

	m_wndTools.ShowWindow(SW_RESTORE);
  
}

//**************************************************************************
// Show Fill Bar
//**************************************************************************
afx_msg void CMainWin::OnShowFillbar()
{

	m_wndBrushBar.ShowWindow(SW_RESTORE);

}

//**************************************************************************
// Hide Tool Bar.
//**************************************************************************
afx_msg void CMainWin::OnHideToolbar()
{

	m_wndTools.ShowWindow(SW_HIDE);

}

//**************************************************************************
// Hide Fill Bar
//**************************************************************************
afx_msg void CMainWin::OnHideFillbar()
{

	m_wndBrushBar.ShowWindow(SW_HIDE);

}


//**************************************************************************
// This handler gets the ID from the clicked toolbar button
//**************************************************************************
afx_msg void CMainWin::OnToolID()
{
	
	ui_tool = LOWORD(GetCurrentMessage()->wParam);

	switch (ui_tool)
	{
	case IDM_ERASER:
		OnEraser();
		break;
	case IDM_FILL:
		OnFill();
		break;
	case IDM_TEXT:
		OnText();
		break;
	}

}

//**************************************************************************
// This handler checks the current clicked button and unclicked the others
//**************************************************************************
afx_msg void CMainWin::OnToolCheck(CCmdUI *UI)
{

	UI->SetCheck(UI->m_nID == ui_tool);

}

//**************************************************************************
// This handler gets the ID from the clicked toolbar button
//**************************************************************************
afx_msg void CMainWin::OnBrushID()
{

	ui_brush = LOWORD(GetCurrentMessage()->wParam);

	switch (ui_brush)
	{
	case IDM_WHITEBRUSH:
		OnWhiteBrush();
		break;
	case IDM_REDBRUSH:
		OnRedBrush();
		break;
	case IDM_GREENBRUSH:
		OnGreenBrush();
		break;
	case IDM_BLUEBRUSH:
		OnBlueBrush();
		break;
	case IDM_YELLOWBRUSH:
		OnYellowBrush();
		break;
	case IDM_BLACKBRUSH:
		OnBlackBrush();
		break;
	case IDM_CUSTOM_BRUSH:
		OnCustomBrush();
		break;
	}

}

//**************************************************************************
// This handler checks the current clicked button and unclicked the others
//**************************************************************************
afx_msg void CMainWin::OnBrushCheck(CCmdUI *UI)
{

	UI->SetCheck(UI->m_nID == ui_brush);

}

//**************************************************************************
// This handler gets the ID from the clicked toolbar button
//**************************************************************************
afx_msg void CMainWin::OnPenSizeID()
{

	ui_pensize = LOWORD(GetCurrentMessage()->wParam);

	switch (ui_pensize)
	{
	case IDM_ONE:
		OnOne();
		break;
	case IDM_TWO:
		OnTwo();
		break;
	case IDM_THREE:
		OnThree();
		break;
	case IDM_FOUR:
		OnFour();
		break;
	case IDM_CUSTOM_SIZE:
		OnCustomSize();
		break;
	}

}

//**************************************************************************
// This handler checks the current clicked button and unclicked the others
//**************************************************************************
afx_msg void CMainWin::OnPenSizeCheck(CCmdUI *UI)
{

	UI->SetCheck(UI->m_nID == ui_pensize);

}

//**************************************************************************
// This handler gets the ID from the clicked toolbar button
//**************************************************************************
afx_msg void CMainWin::OnPenFillID()
{

	ui_penfill = LOWORD(GetCurrentMessage()->wParam);

	switch (ui_penfill)
	{
	case IDM_REDPEN:
		OnRedPen();
		break;
	case IDM_GREENPEN:
		OnGreenPen();
		break;
	case IDM_BLUEPEN:
		OnBluePen();
		break;
	case IDM_YELLOWPEN:
		OnYellowPen();
		break;
	case IDM_BLACKPEN:
		OnBlackPen();
		break;
	case IDM_CUSTOM_PEN:
		OnCustomPen();
		break;
	}

}

//**************************************************************************
// This handler checks the current clicked button and unclicked the others
//**************************************************************************
afx_msg void CMainWin::OnPenFillCheck(CCmdUI *UI)
{

	UI->SetCheck(UI->m_nID == ui_penfill);

}


//**************************************************************************
// Used for our eraser
//**************************************************************************
afx_msg void CMainWin::OnEraser()
{

	if (fixflicker)
	{
		Invalidate();
	}

	if (firsterase)
	{
		OnFour();
		ui_pensize = IDM_FOUR;
		firsterase = 0;
	}

}

//**************************************************************************
// Used for our Fill
// - This uses a Colour Dialog box
//**************************************************************************
afx_msg void CMainWin::OnFill()
{
	
	if (fixflicker)
	{
		Invalidate();
	}

	CColorDialog *colDlg = new CColorDialog(TRUE);

	retval = colDlg->DoModal();

	if (retval == IDOK) 
	{

		newCol = colDlg->GetColor();
	
		m_memDC.SelectObject(&m_OldBrush);	

		m_CustomBK.DeleteObject();

		m_CustomBK.CreateSolidBrush(newCol);
		
		m_memDC.SelectObject(&m_CustomBK);	

		m_PreviousBrush.DeleteObject();
		m_PreviousBrush.CreateSolidBrush(newCol);

		ui_brush = IDM_CUSTOM_BRUSH;

		globalBRUSH = 7;

	}
	else
	{

	}
	
}

//**************************************************************************
// Set the size of the pens
//**************************************************************************
afx_msg void CMainWin::setSize()
{

	m_RedPen.DeleteObject();
	m_GreenPen.DeleteObject();
	m_BluePen.DeleteObject();
	m_YellowPen.DeleteObject();
	m_CustPen.DeleteObject();
	m_CurrentPen.DeleteObject();

	m_RedPen.CreatePen(PS_SOLID, globalSIZE, RGB(255,0,0));
	m_GreenPen.CreatePen(PS_SOLID, globalSIZE, RGB(0,255,0));
	m_BluePen.CreatePen(PS_SOLID, globalSIZE, RGB(0,0,255));
	m_YellowPen.CreatePen(PS_SOLID, globalSIZE, RGB(255, 255, 0));
	m_CurrentPen.CreatePen(PS_SOLID, globalSIZE, corCol);
	m_CustPen.CreatePen(PS_SOLID, globalSIZE, bgCol);

	m_OldPen.DeleteObject();
	m_OldPen.CreatePen(PS_SOLID, globalSIZE, RGB(0, 0, 0));

}

//**************************************************************************
// Predefined Pen Size Settings
//**************************************************************************
afx_msg void CMainWin::OnOne()
{
  
	m_memDC.SelectObject(&m_OldPen);

	globalSIZE = 1;

	setSize();

	checkPen();


}

//**************************************************************************
// Predefined Pen Size Settings
//**************************************************************************
afx_msg void CMainWin::OnTwo()
{

	m_memDC.SelectObject(&m_OldPen);

	globalSIZE = 2;

	setSize();

	checkPen();

}

//**************************************************************************
// Predefined Pen Size Settings
//**************************************************************************
afx_msg void CMainWin::OnThree()
{
	
	m_memDC.SelectObject(&m_OldPen);

	globalSIZE = 3;

	setSize();

	checkPen();
	
}

//**************************************************************************
// Predefined Pen Size Settings
//**************************************************************************
afx_msg void CMainWin::OnFour()
{

	m_memDC.SelectObject(&m_OldPen);

	globalSIZE = 4;

	setSize();

	checkPen();

}

//**************************************************************************
// Check the Current Pen
//**************************************************************************
void CMainWin::checkPen()
{

	switch (globalPEN)
	{
	case 1:
		m_memDC.SelectObject(&m_RedPen);
		break;
	case 2:
		m_memDC.SelectObject(&m_GreenPen);
		break;
	case 3:
		m_memDC.SelectObject(&m_BluePen);
		break;
	case 4:
		m_memDC.SelectObject(&m_YellowPen);
		break;
	case 5:
		m_memDC.SelectObject(&m_OldPen);
		break;
	case 6:
		m_memDC.SelectObject(&m_CustPen);
		break;
	}

}

//**************************************************************************
// Check the Current Brush
//**************************************************************************
void CMainWin::checkBrush()
{

	switch (globalBRUSH)
	{
	case 1:
		m_memDC.SelectObject(&m_OldBrush);
		break;
	case 2:
		m_memDC.SelectObject(&m_RedBrush);
		break;
	case 3:
		m_memDC.SelectObject(&m_GreenBrush);
		break;
	case 4:
		m_memDC.SelectObject(&m_BlueBrush);
		break;
	case 5:
		m_memDC.SelectObject(&m_YellowBrush);
		break;
	case 6:
		m_memDC.SelectObject(&m_BlackBrush);
		break;
	case 7:
		m_memDC.SelectObject(&m_CustomBK);
		break;
	}

}

//**************************************************************************
// Predefined Pen Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnRedPen()
{

	m_memDC.SelectObject(&m_RedPen);
	
	corCol = RGB(255, 0, 0);

	globalPEN = 1;

	setSize();

}

//**************************************************************************
// Predefined Pen Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnGreenPen()
{

	m_memDC.SelectObject(&m_GreenPen);

	corCol = RGB(0, 255, 0);

	globalPEN = 2;

	setSize();

}

//**************************************************************************
// Predefined Pen Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnBluePen()
{

	m_memDC.SelectObject(&m_BluePen);

	corCol = RGB(0, 0, 255);

	globalPEN = 3;

	setSize();

}

//**************************************************************************
// Predefined Pen Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnYellowPen()
{

	m_memDC.SelectObject(&m_YellowPen);

	corCol = RGB(255, 255, 0);

	globalPEN = 4;

	setSize();

}

//**************************************************************************
// Predefined Pen Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnBlackPen()
{

	m_memDC.SelectObject(&m_OldPen);
	
	corCol = RGB(0, 0, 0);
	
	globalPEN = 5;

	setSize();

}

//**************************************************************************
// Custom Pen Colour Settings
// - This uses the CColorDialog Dialog Box
//**************************************************************************
afx_msg void CMainWin::OnCustomPen()
{
	
	CColorDialog *colDlg = new CColorDialog(TRUE);

	retval = colDlg->DoModal();

	if (retval == IDOK) {
	
		bgCol = colDlg->GetColor();

		m_memDC.SelectObject(&m_OldPen);
		
		m_CustPen.DeleteObject();

		m_CustPen.CreatePen(PS_SOLID, globalSIZE, bgCol);
		
		m_memDC.SelectObject(&m_CustPen);
		
		fillCol = bgCol;

		corCol = bgCol;

		globalPEN = 6;

		setSize();
	
	}

}
	
//**************************************************************************
// Predefined Brush Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnWhiteBrush()
{

	m_memDC.SelectObject(&m_OldBrush);

	m_PreviousBrush.DeleteObject();
	m_PreviousBrush.CreateStockObject(WHITE_BRUSH);
	
	m_CurrentBrush.DeleteObject();
	m_CurrentBrush.CreateStockObject(WHITE_BRUSH);

	globalBRUSH = 1;

}

//**************************************************************************
// Predefined Brush Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnRedBrush()
{

	m_memDC.SelectObject(&m_RedBrush);

	m_PreviousBrush.DeleteObject();
	m_PreviousBrush.CreateSolidBrush(RGB(255,0,0));

	m_CurrentBrush.DeleteObject();
	m_CurrentBrush.CreateSolidBrush(RGB(255,0,0));

	globalBRUSH = 2;

}

//**************************************************************************
// Predefined Brush Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnGreenBrush()
{

	m_memDC.SelectObject(&m_GreenBrush);
	
	m_PreviousBrush.DeleteObject();
	m_PreviousBrush.CreateSolidBrush(RGB(0,255,0));

	m_CurrentBrush.DeleteObject();
	m_CurrentBrush.CreateSolidBrush(RGB(0,255,0));

	globalBRUSH = 3;

}

//**************************************************************************
// Predefined Brush Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnBlueBrush()
{

	m_memDC.SelectObject(&m_BlueBrush);

	m_PreviousBrush.DeleteObject();
	m_PreviousBrush.CreateSolidBrush(RGB(0,0,255));

	m_CurrentBrush.DeleteObject();
	m_CurrentBrush.CreateSolidBrush(RGB(0,0,255));

	globalBRUSH = 4;

}

//**************************************************************************
// Predefined Brush Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnYellowBrush()
{

	m_memDC.SelectObject(&m_YellowBrush);
	
	m_PreviousBrush.DeleteObject();
	m_PreviousBrush.CreateSolidBrush(RGB(255,255,0));
	
	m_CurrentBrush.DeleteObject();
	m_CurrentBrush.CreateSolidBrush(RGB(255,255,0));

	globalBRUSH = 5;

}

//**************************************************************************
// Predefined Brush Colour Settings
//**************************************************************************
afx_msg void CMainWin::OnBlackBrush()
{

	m_memDC.SelectObject(&m_BlackBrush);
	
	m_PreviousBrush.DeleteObject();
	m_PreviousBrush.CreateStockObject(BLACK_BRUSH);
	
	m_CurrentBrush.DeleteObject();
	m_CurrentBrush.CreateStockObject(BLACK_BRUSH);

	globalBRUSH = 6;
	
}

//**************************************************************************
// Custom Brush Colour Settings
// - This also uses the CColorDialog Dialog Box
//**************************************************************************
afx_msg void CMainWin::OnCustomBrush()
{
	
	CColorDialog *colDlg = new CColorDialog(TRUE);

	retval = colDlg->DoModal();

	if (retval == IDOK) 
	{

		bgCol = colDlg->GetColor();
	
		m_memDC.SelectObject(&m_OldBrush);	

		m_CustomBK.DeleteObject();

		m_CustomBK.CreateSolidBrush(bgCol);
		
		m_memDC.SelectObject(&m_CustomBK);	

		m_PreviousBrush.DeleteObject();
		m_PreviousBrush.CreateSolidBrush(bgCol);
	
		m_CurrentBrush.DeleteObject();
		m_CurrentBrush.CreateSolidBrush(bgCol);

		globalBRUSH = 7;

	}
	else
	{
	
	}

}

//**************************************************************************
// OnLButtonDown
// - This is where we set the mouse capture
// - Set the CClientDC ROP
// - Set the Cursors
// - Set the Undo
//**************************************************************************
afx_msg void CMainWin::OnLButtonDown(UINT flags, CPoint loc)
{

	SetCapture();

	CClientDC DC(this);
	DC.SetROP2(R2_NOT);

	if (doundoredo)
	{
		WriteWindowToDIB("undo.bmp", this);
	}

	switch (ui_tool)
	{
		case IDM_LINE:
			m_memDC.MoveTo(loc.x, loc.y);
			DC.MoveTo(loc.x, loc.y);
			hCursor = pApp->LoadCursor(IDC_PEN);
			::SetCursor(hCursor);
			::ClipCursor(NULL);
			break;
		case IDM_HALLOW_RECTANGLE:
			break;
		case IDM_RECTANGLE:
			break;
		case IDM_HALLOW_ROUNDRECT:
			break;
		case IDM_ROUNDRECT:
			break;
		case IDM_HALLOW_ELLIPSE:
			break;
		case IDM_ELLIPSE:
			break;
		case IDM_HALLOW_OVAL:
			break;
		case IDM_OVAL:
			break;
		case IDM_ARC:
			break;
		case IDM_HALLOW_PIE:
			break;
		case IDM_PIE:
			break;
		case IDM_FREEHAND:
			m_memDC.MoveTo(loc.x, loc.y);
			DC.MoveTo(loc.x, loc.y);
			hCursor = pApp->LoadCursor(IDC_FREEHAND);
			::SetCursor(hCursor);
			::ClipCursor(NULL);
			break;
		case IDM_ERASER:
			eraserPEN.DeleteObject();
			eraserPEN.CreatePen(PS_SOLID, globalSIZE, RGB(255, 255, 255));
			m_memDC.SelectObject(&eraserPEN);
			DC.SelectObject(&eraserPEN);
			hCursor = pApp->LoadCursor(IDC_ERASER);
			::SetCursor(hCursor);
			::ClipCursor(NULL);
			break;
		case IDM_FILL:
			m_memDC.ExtFloodFill(loc.x, loc.y, m_memDC.GetPixel(loc), FLOODFILLSURFACE);
			checkBrush();
			break;
		case IDM_TEXT:
			CSampleDialog4 diagOb("IDD_TEXTVIEW", this);
			diagOb.DoModal();
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.TextOut(loc.x, loc.y, globalTEXT, strlen(globalTEXT));
			m_memDC.SelectObject(&m_PreviousBrush);
			InvalidateRect(NULL);
		break;
	}

	rect1 = loc.x;
	rect2 = loc.y;
	dothis = 1;

	oldPoint = loc;

}

//**************************************************************************
// OnMouseMove
// - This is my RubberBand Settings
//**************************************************************************
afx_msg void CMainWin::OnMouseMove(UINT nFlags, CPoint loc)
{

	char str[80], str2[80] = "Mouse located at: ";

	wsprintf(str, "%d %d", loc.x, loc.y);
	strcat(str2, str);
	m_wndStatusBar.SetWindowText(str2);

	if (GetCapture() != this)
		return;

	CClientDC DC(this);
	checkPen();
	
	if (dothis) 
	{
		switch(ui_tool)
		{
		case IDM_LINE:
			DC.SelectObject(&m_CurrentPen);
			DC.SelectObject(&m_CurrentBrush);
			DC.SetROP2(R2_NOTXORPEN);
			DC.MoveTo(rect1, rect2);
			DC.LineTo(loc.x, loc.y);
			DC.MoveTo(rect1, rect2);
			DC.LineTo(oldPoint.x, oldPoint.y);
			break;
		case IDM_HALLOW_RECTANGLE:
			DC.SelectObject(&HollowBrush);
			DC.SelectObject(&m_CurrentPen);
			DC.SetROP2(R2_NOTXORPEN);
			DC.Rectangle(rect1, rect2, loc.x, loc.y);
			DC.Rectangle(rect1, rect2, oldPoint.x, oldPoint.y);
			DC.SelectObject(&m_PreviousBrush);
			break;
		case IDM_RECTANGLE:
			DC.SelectObject(&m_CurrentPen);
			DC.SelectObject(&m_CurrentBrush);
			DC.SetROP2(R2_NOTXORPEN);
			DC.Rectangle(rect1, rect2, loc.x, loc.y);
			DC.Rectangle(rect1, rect2, oldPoint.x, oldPoint.y);
			break;
		case IDM_HALLOW_ROUNDRECT:
			DC.SelectObject(&HollowBrush);
			DC.SelectObject(&m_CurrentPen);
			DC.SetROP2(R2_NOTXORPEN);
			DC.RoundRect(rect1, rect2, loc.x, loc.y, 15, 13);
			DC.RoundRect(rect1, rect2, oldPoint.x, oldPoint.y, 15, 13);
			DC.SelectObject(&m_PreviousBrush);
			break;
		case IDM_ROUNDRECT:
			DC.SelectObject(&m_CurrentPen);
			DC.SelectObject(&m_CurrentBrush);
			DC.SetROP2(R2_NOTXORPEN);
			DC.RoundRect(rect1, rect2, loc.x, loc.y, 15, 13);
			DC.RoundRect(rect1, rect2, oldPoint.x, oldPoint.y, 15, 13);
			break;
		case IDM_HALLOW_ELLIPSE:
			DC.SelectObject(&HollowBrush);
			DC.SelectObject(&m_CurrentPen);
			DC.SetROP2(R2_NOTXORPEN);
			DC.Ellipse(rect1, rect2, loc.x, loc.y);
			DC.Ellipse(rect1, rect2, oldPoint.x, oldPoint.y);
			DC.SelectObject(&m_PreviousBrush);
			break;
		case IDM_ELLIPSE:
			DC.SelectObject(&m_CurrentPen);
			DC.SelectObject(&m_CurrentBrush);
			DC.SetROP2(R2_NOTXORPEN);
			DC.Ellipse(rect1, rect2, loc.x, loc.y);
			DC.Ellipse(rect1, rect2, oldPoint.x, oldPoint.y);
			break;
		case IDM_HALLOW_OVAL:
			DC.SelectObject(&HollowBrush);
			DC.SelectObject(&m_CurrentPen);
			DC.SetROP2(R2_NOTXORPEN);
			DC.RoundRect(rect1, rect2, loc.x, loc.y, 100, 100);
			DC.RoundRect(rect1, rect2, oldPoint.x, oldPoint.y, 100, 100);
			DC.SelectObject(&m_PreviousBrush);
			break;
		case IDM_OVAL:
			DC.SelectObject(&m_CurrentPen);
			DC.SelectObject(&m_CurrentBrush);
			DC.SetROP2(R2_NOTXORPEN);
			DC.RoundRect(rect1, rect2, loc.x, loc.y, 100, 100);
			DC.RoundRect(rect1, rect2, oldPoint.x, oldPoint.y, 100, 100);
			break;
		case IDM_ARC:
			DC.SelectObject(&m_CurrentPen);
			DC.SelectObject(&m_CurrentBrush);
			DC.SetROP2(R2_NOTXORPEN);
			DC.Arc(rect1, rect2, loc.x, loc.y, rect1, rect2, loc.x, loc.y);
			DC.Arc(rect1, rect2, oldPoint.x, oldPoint.y, rect1, rect2, oldPoint.x, oldPoint.y);
			break;
		case IDM_HALLOW_PIE:
			DC.SelectObject(&HollowBrush);
			DC.SelectObject(&m_CurrentPen);
			DC.SetROP2(R2_NOTXORPEN);
			DC.Pie(rect1, rect2, loc.x, loc.y, rect1, rect2, loc.x, loc.y);
			DC.Pie(rect1, rect2, oldPoint.x, oldPoint.y, rect1, rect2, oldPoint.x, oldPoint.y);
			DC.SelectObject(&m_PreviousBrush);
			break;
		case IDM_PIE:
			DC.SelectObject(&m_CurrentPen);
			DC.SelectObject(&m_CurrentBrush);
			DC.SetROP2(R2_NOTXORPEN);
			DC.Pie(rect1, rect2, loc.x, loc.y, rect1, rect2, loc.x, loc.y);
			DC.Pie(rect1, rect2, oldPoint.x, oldPoint.y, rect1, rect2, oldPoint.x, oldPoint.y);
			break;
		case IDM_FREEHAND:
			DC.SelectObject(&m_CurrentPen);
			DC.SelectObject(&m_CurrentBrush);
			DC.MoveTo(oldPoint.x, oldPoint.y);
			DC.LineTo(loc.x, loc.y);
			m_memDC.MoveTo(oldPoint.x, oldPoint.y);
			m_memDC.LineTo(loc.x, loc.y);
			break;
		case IDM_ERASER:
			DC.SetROP2(R2_WHITE);
			DC.SelectObject(&eraserPEN);
			m_memDC.SelectObject(&eraserPEN);
			DC.MoveTo(oldPoint.x, oldPoint.y);
			DC.LineTo(loc.x, loc.y);
			m_memDC.MoveTo(oldPoint.x, oldPoint.y);
			m_memDC.LineTo(loc.x, loc.y);
			break;
		}
	}

	oldPoint = loc;

}

//**************************************************************************
// OnLButtonUP
// - This is where we write all objects and call InvalidateRect(NULL);
// - also used if we only terminate from the Left Button.
// - also this is the default way to terminate the lines
//**************************************************************************
afx_msg void CMainWin::OnLButtonUp(UINT nFlags, CPoint loc)
{

	if (cbcheck)
	{

		checkPen();

		if (GetCapture() != this)
			return;

		switch(ui_tool)
		{
		case IDM_LINE:
			m_memDC.LineTo(loc.x, loc.y);
			break;
		case IDM_HALLOW_RECTANGLE:
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.Rectangle(rect1, rect2, loc.x, loc.y);
			m_memDC.SelectObject(&m_PreviousBrush); // select default brush
			break;
		case IDM_RECTANGLE:
			m_memDC.Rectangle(rect1, rect2, loc.x, loc.y);
			break;
		case IDM_HALLOW_ROUNDRECT:
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.RoundRect(rect1, rect2, loc.x, loc.y, 15, 13);
			m_memDC.SelectObject(&m_PreviousBrush); // select default brush
			break;
		case IDM_ROUNDRECT:
			m_memDC.RoundRect(rect1, rect2, loc.x, loc.y, 15, 13);
			break;
		case IDM_HALLOW_ELLIPSE:
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.Ellipse(rect1, rect2, loc.x, loc.y);
			m_memDC.SelectObject(&m_PreviousBrush); // select default brush
			break;
		case IDM_ELLIPSE:
			m_memDC.Ellipse(rect1, rect2, loc.x, loc.y);
			break;
		case IDM_HALLOW_OVAL:
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.RoundRect(rect1, rect2, loc.x, loc.y, 100, 100);
			m_memDC.SelectObject(&m_PreviousBrush); // select default brush
			break;
		case IDM_OVAL:
			m_memDC.RoundRect(rect1, rect2, loc.x, loc.y, 100, 100);
			break;
		case IDM_ARC:
			m_memDC.Arc(rect1, rect2, loc.x, loc.y, rect1, rect2, loc.x, loc.y);
			break;
		case IDM_HALLOW_PIE:
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.Pie(rect1, rect2, loc.x, loc.y, rect1, rect2, loc.x, loc.y);
			m_memDC.SelectObject(&m_PreviousBrush); // select default brush
			break;
		case IDM_PIE:
			m_memDC.Pie(rect1, rect2, loc.x, loc.y, rect1, rect2, loc.x, loc.y);
			break;
		case IDM_FREEHAND:
			m_memDC.MoveTo(oldPoint.x, oldPoint.y);
			m_memDC.LineTo(loc.x, loc.y);
			break;
		case IDM_ERASER:
			m_memDC.SelectObject(&eraserPEN);
			m_memDC.MoveTo(oldPoint.x, oldPoint.y);
			m_memDC.LineTo(loc.x, loc.y);
			checkPen();
			break;
		}

		dothis = 0;

		if (fixflicker)
		{
			redraw = 1;
		}
		
		fromlup = 1;

		InvalidateRect(NULL);
	
		if (doundoredo && fromlup)
		{
			WriteWindowToDIB("redo.bmp", this);
			fromlup = 0;
		}
	
		ReleaseCapture();

	}

}

//**************************************************************************
// OnRButtonDown
// - This is where we write all objects and call InvalidateRect(NULL);
// - also used if we terminate from the Right Button.
// - also this is the default way to show the floating menu
//**************************************************************************
afx_msg void CMainWin::OnRButtonDown(UINT nFlags, CPoint loc)
{

	if (cbcheck == 0)
	{

		checkPen();

		if (GetCapture() != this)
			return;

		switch(ui_tool)
		{
		case IDM_LINE:
			m_memDC.LineTo(loc.x, loc.y);
			break;
		case IDM_HALLOW_RECTANGLE:
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.Rectangle(rect1, rect2, loc.x, loc.y);
			m_memDC.SelectObject(&m_PreviousBrush); // select default brush
			break;
		case IDM_RECTANGLE:
			m_memDC.Rectangle(rect1, rect2, loc.x, loc.y);
			break;
		case IDM_HALLOW_ROUNDRECT:
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.RoundRect(rect1, rect2, loc.x, loc.y, 15, 13);
			m_memDC.SelectObject(&m_PreviousBrush); // select default brush
			break;
		case IDM_ROUNDRECT:
			m_memDC.RoundRect(rect1, rect2, loc.x, loc.y, 15, 13);
			break;
		case IDM_HALLOW_ELLIPSE:
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.Ellipse(rect1, rect2, loc.x, loc.y);
			m_memDC.SelectObject(&m_PreviousBrush); // select default brush
			break;
		case IDM_ELLIPSE:
			m_memDC.Ellipse(rect1, rect2, loc.x, loc.y);
			break;
		case IDM_HALLOW_OVAL:
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.RoundRect(rect1, rect2, loc.x, loc.y, 100, 100);
			m_memDC.SelectObject(&m_PreviousBrush); // select default brush
			break;
		case IDM_OVAL:
			m_memDC.RoundRect(rect1, rect2, loc.x, loc.y, 100, 100);
			break;
		case IDM_ARC:
			m_memDC.Arc(rect1, rect2, loc.x, loc.y, rect1, rect2, loc.x, loc.y);
			break;
		case IDM_HALLOW_PIE:
			m_memDC.SelectObject(&HollowBrush);
			m_memDC.Pie(rect1, rect2, loc.x, loc.y, rect1, rect2, loc.x, loc.y);
			m_memDC.SelectObject(&m_PreviousBrush); // select default brush
			break;
		case IDM_PIE:
			m_memDC.Pie(rect1, rect2, loc.x, loc.y, rect1, rect2, loc.x, loc.y);
			break;
		case IDM_FREEHAND:
			m_memDC.MoveTo(oldPoint.x, oldPoint.y);
			m_memDC.LineTo(loc.x, loc.y);
			break;
		case IDM_ERASER:
			m_memDC.SelectObject(&eraserPEN);
			m_memDC.MoveTo(oldPoint.x, oldPoint.y);
			m_memDC.LineTo(loc.x, loc.y);
			checkPen();
			break;
		}

		dothis = 0;

		if (fixflicker)
		{
			redraw = 1;
		}
	
		fromlup = 1;

		InvalidateRect(NULL);
	
		if (doundoredo && fromlup)
		{
			WriteWindowToDIB("redo.bmp", this);
			fromlup = 0;
		}

		ReleaseCapture();

	}
	else
	{
		
		m_FloatMenu.DestroyMenu();
		CMenu *SubMenu;

		ClientToScreen(&loc);

		m_FloatMenu.LoadMenu("POPUPMENU");
		SubMenu = m_FloatMenu.GetSubMenu(0);

		SubMenu->TrackPopupMenu(1, loc.x, loc.y, this);

		m_FloatMenu.DestroyMenu();

	}


}

//**************************************************************************
// Custom Pen Size
// - This uses a Dialog Box to show the slider and editbox control
// - We can also override the slider to get our desired pensize
//**************************************************************************
afx_msg void CMainWin::OnCustomSize()
{


	CSampleDialog3 diagOb3("SampleDialog3", this);
	
	diagOb3.DoModal();

	m_memDC.SelectObject(&m_OldPen);

	setSize();
	
	checkPen();

}

//**************************************************************************
// Zoom In.
//**************************************************************************
afx_msg void CMainWin::OnZoomIn()
{

	if (m_vpX == 10 || m_orgX == 0)
	{
	}
	else
	{
		m_vpX -= 10;
		m_vpY -= 10;
		m_orgX -= 50;
		m_orgY -= 50;
	}
	
	InvalidateRect(NULL);

}

//**************************************************************************
// Zoom Out.
//**************************************************************************
afx_msg void CMainWin::OnZoomOut()
{

	m_vpX += 10;
	m_vpY += 10;
	m_orgX += 50;
	m_orgY += 50;
	
	InvalidateRect(NULL);

}

//**************************************************************************
// Reset the view.
//**************************************************************************
afx_msg void CMainWin::OnViewReset()
{
	
	m_vpX = m_vpY = 10;
	m_orgX = m_orgY = 0;

	InvalidateRect(NULL);
		
}

//**************************************************************************
// MessageMap for CSampleDialog.
//**************************************************************************
BEGIN_MESSAGE_MAP(CSampleDialog, CDialog)
	ON_BN_CLICKED(IDC_CREDITS, OnCredits)
END_MESSAGE_MAP()

//**************************************************************************
// Initialize CSampleDialog
//**************************************************************************
BOOL CSampleDialog::OnInitDialog()
{

	CDialog::OnInitDialog(); // Call base class version
		
	return TRUE;

}

//**************************************************************************
// Credits.
// - This is where we popup the scrolling credits
//**************************************************************************
afx_msg void CSampleDialog::OnCredits()
{

    CCredits dlgCredits(this);
    dlgCredits.DoModal();

}

//**************************************************************************
// MessageMap for CSampleDialog2.
//**************************************************************************
BEGIN_MESSAGE_MAP(CSampleDialog2, CDialog)
	ON_BN_CLICKED(IDC_LEFT, OnCBLeft)
	ON_BN_CLICKED(IDC_RIGHT, OnCBRight)
END_MESSAGE_MAP()

//**************************************************************************
// Initialize CSampleDialog2
//**************************************************************************
BOOL CSampleDialog2::OnInitDialog()
{

	CDialog::OnInitDialog(); // Call base class version
		
	CButton *cbleftptr = (CButton *) GetDlgItem(IDC_LEFT);
	CButton *cbrightptr = (CButton *) GetDlgItem(IDC_RIGHT);

	if (cbcheck)
	{
		cbleftptr->SetCheck(1);
		cbrightptr->SetCheck(0);
	}
	else
	{
		cbrightptr->SetCheck(1);
		cbleftptr->SetCheck(0);
	}

	return TRUE;

}

//**************************************************************************
// Enable Left Mouse Termination
//**************************************************************************
afx_msg void CSampleDialog2::OnCBLeft()
{
	
	CButton *cbleftptr = (CButton *) GetDlgItem(IDC_LEFT);
	CButton *cbrightptr = (CButton *) GetDlgItem(IDC_RIGHT);
	
	cbleftptr->SetCheck(1);
	cbrightptr->SetCheck(0);

	cbcheck = 1;

}

//**************************************************************************
// Enable Right Mouse Termination
//**************************************************************************
afx_msg void CSampleDialog2::OnCBRight()
{
	
	CButton *cbleftptr = (CButton *) GetDlgItem(IDC_LEFT);
	CButton *cbrightptr = (CButton *) GetDlgItem(IDC_RIGHT);
	
	cbleftptr->SetCheck(0);
	cbrightptr->SetCheck(1);

	cbcheck = 0;

}


//**************************************************************************
// MessageMap for CSampleDialog3.
//**************************************************************************
BEGIN_MESSAGE_MAP(CSampleDialog3, CDialog)
	ON_COMMAND(IDOK, OnOk)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

//**************************************************************************
// Initialize CSampleDialog3
//**************************************************************************
BOOL CSampleDialog3::OnInitDialog()
{

	CDialog::OnInitDialog(); // Call base class version
	RECT r;
	char str[80];

	r.left = 10;
	r.top = 5;
	r.right = 450;
	r.bottom = 60;

	m_Slider.Create(WS_VISIBLE | WS_CHILD | WS_BORDER | TBS_AUTOTICKS | WS_TABSTOP, r, this, IDC_SLIDER);
	m_Slider.SetRange(0, 50);
	m_Slider.SetPos(globalSIZE);

	wsprintf(str, "%d", globalSIZE);

	CEdit *ebptr = (CEdit *) GetDlgItem(IDC_CUST_SIZE);
	ebptr->SetWindowText(str);

	return TRUE;

}

//**************************************************************************
// Do nothing
//**************************************************************************
afx_msg void CSampleDialog3::OnCancel()
{

	EndDialog(0);

}

//**************************************************************************
// Check if editbox is equal to the slider
// if true then use the slider as the size
// if not then use the editbox
//**************************************************************************
afx_msg void CSampleDialog3::OnOk()
{

	CEdit *ebptr = (CEdit *) GetDlgItem(IDC_CUST_SIZE);
	
	int itr;
	char str[80];
	int i;

	i = ebptr->GetWindowText(str, sizeof str);
	
	itr = atoi(str);
	
	if (itr == m_Slider.GetPos())
	{
		globalSIZE = m_Slider.GetPos();
	}
	else
	{
		globalSIZE = itr;
	}
	
	EndDialog(0);

}

//**************************************************************************
// For the slider value
//**************************************************************************
afx_msg void CSampleDialog3::OnHScroll(UINT SBCode, UINT Pos, CScrollBar *SB)
{

	CEdit *ebptr = (CEdit *) GetDlgItem(IDC_CUST_SIZE);
	char str[50];
	int itr;

	itr = m_Slider.GetPos();

	wsprintf(str, "%d", itr);

	m_Slider.SetPos(m_Slider.GetPos());
	ebptr->SetWindowText(str);

}

//**************************************************************************
// MessageMap for CSampleDialog4.
//**************************************************************************
BEGIN_MESSAGE_MAP(CSampleDialog4, CDialog)
	ON_COMMAND(IDOK, OnOk)
	ON_COMMAND(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

//**************************************************************************
// Initialize CSampleDialog3
//**************************************************************************
BOOL CSampleDialog4::OnInitDialog()
{

	CDialog::OnInitDialog(); // Call base class version

	CEdit *ebptr = (CEdit *) GetDlgItem(IDC_TEXT);
	ebptr->SetWindowText("");

	return TRUE;

}

//**************************************************************************
// Do nothing
//**************************************************************************
afx_msg void CSampleDialog4::OnCancel()
{

	EndDialog(0);

}

//**************************************************************************
// Check if editbox has something in it
//**************************************************************************
afx_msg void CSampleDialog4::OnOk()
{

	CEdit *ebptr = (CEdit *) GetDlgItem(IDC_TEXT);
	
	char str[80];

	ebptr->GetWindowText(str, sizeof str);
	
	wsprintf(globalTEXT, str);
	
	EndDialog(0);

}

//**************************************************************************
// MessageMap for our Application
//**************************************************************************
BEGIN_MESSAGE_MAP(CApp, CWinApp)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

// Initialize the application.
BOOL CApp::InitInstance()
{
	
	CBrush bkbrush;

	bkbrush.CreateStockObject(WHITE_BRUSH);

	LoadStdProfileSettings();

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	Enable3dControls();

	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
	CSplashWnd::ShowSplashScreen(NULL);

	while (timer != 1000000000)
	{
		timer += 10;

	}

	LPCSTR cname = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW, NULL,
						bkbrush,
						LoadIcon(IDI_ICON1));

	hCursor = pApp->LoadStandardCursor(IDC_CROSS);
	::SetCursor(hCursor);
	::ClipCursor(NULL);

	m_pMainWnd = new CMainWin(cname);
	m_pMainWnd->ShowWindow(SW_NORMAL); //SW_MAXIMIZE);
	m_pMainWnd->UpdateWindow();

	return TRUE;

}

//**************************************************************************
// For loading the bitmap
//**************************************************************************
BOOL CMyBitmap::LoadBitmap(CString szFilename) 
{ 
	ASSERT(szFilename);
	DeleteObject();

	HBITMAP hBitmap = NULL; 
	hBitmap = (HBITMAP)LoadImage(NULL, szFilename, IMAGE_BITMAP, 0, 0, 
		LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE); 
	return Attach(hBitmap); 
}

//**************************************************************************
// instantiate the application
//**************************************************************************
CApp App; // instantiate the application
