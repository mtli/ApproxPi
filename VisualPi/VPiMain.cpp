#define MAX_SCR_WIDTH 1366
#define MAX_SCR_HEIGHT 768

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

TCHAR szAppName [] = TEXT ("VisualPi") ;
TCHAR szTitle   [] = TEXT ("VisualPi - Pi in 2^24 base") ;

INT cxClient,cyClient;

RGBQUAD *pGraphMem;
#define COOR_CONV_X(x) (x)
#define COOR_CONV_Y(y) (MAX_SCR_HEIGHT-(y)-1)
#define GM(x,y)  pGraphMem[COOR_CONV_Y(y)*MAX_SCR_WIDTH+COOR_CONV_X(x)]

UINT PiMem[MAX_SCR_WIDTH*MAX_SCR_HEIGHT];

static BITMAPINFOHEADER bmih;
static HBITMAP hBitmap;

static HWND hwnd;
void PiShader()
{
	INT x,y,i=0;
	for(y=0; y<cyClient; y++)
		for(x=0; x<cxClient; x++)
			*((UINT*)&GM(x,y)) = PiMem[i++];
}

void DisplayDigitsInTitle ()
{
	static TCHAR szBuffer [256];
	wsprintf(szBuffer, TEXT ("VisualPi - %dx%d=%d digits of Pi in 2^24 base"), cxClient, cyClient, cxClient*cyClient);
	SetWindowText(hwnd , szBuffer);
}

void FlushGraphMem (HDC hdc)
{    
     HDC    hdcMem ;
	 PiShader();
	 DisplayDigitsInTitle();
	 hdcMem = CreateCompatibleDC (hdc) ;
	 SelectObject (hdcMem, hBitmap) ;
	 BitBlt (hdc, 0, 0, cxClient, cyClient, hdcMem, 0, 0, SRCCOPY) ;
	 DeleteDC (hdcMem) ;
}     

LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	 HDC    hdc ;
	 PAINTSTRUCT     ps ;
	 FILE *fp;
     switch (iMsg)
     {
	 case WM_CREATE:
		  hBitmap = CreateDIBSection (NULL,	(BITMAPINFO*)&bmih,	DIB_RGB_COLORS,(void**)&pGraphMem, NULL,0);
		  if( !( fp = fopen("Pi2_24", "r") ) )
		  {
			 MessageBox (NULL, TEXT ("Cannot open file \"Pi2_24\"."),szAppName, MB_ICONERROR) ;
			 exit(-1);
		  }
		  fread( PiMem, sizeof(*PiMem), sizeof(PiMem)/sizeof(*PiMem), fp);
		  fclose(fp);
          return 0 ;
	 case WM_PAINT:
          hdc = BeginPaint (hwnd, &ps) ;
		  FlushGraphMem(hdc);
          EndPaint (hwnd, &ps) ;
          return 0 ;
	 case WM_SIZE:
          cxClient = __min( LOWORD (lParam), MAX_SCR_WIDTH-1 ) ;
          cyClient = __min( HIWORD (lParam), MAX_SCR_HEIGHT-1 ) ;
          return 0 ;
     case WM_DESTROY:
		  DeleteObject (hBitmap) ;
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     MSG          msg ;
     WNDCLASSEX     wndclass ;

	 bmih.biSize = sizeof (BITMAPINFOHEADER);
	 bmih.biWidth = MAX_SCR_WIDTH;
	 bmih.biHeight = MAX_SCR_HEIGHT;
	 bmih.biPlanes = 1;
	 bmih.biBitCount = 32;
	 bmih.biCompression = BI_RGB;
	 bmih.biSizeImage = 0;
	 bmih.biXPelsPerMeter = 0;
	 bmih.biYPelsPerMeter = 0;
	 bmih.biClrUsed = 0;
	 bmih.biClrImportant = 0;

	 wndclass.cbSize		= sizeof(WNDCLASSEX);
     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, TEXT ("PI_ICON") ) ;
	 wndclass.hIconSm       = LoadIcon (NULL, TEXT ("PI_ICON") ) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;
     
     if (!RegisterClassEx (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"),
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     
     hwnd = CreateWindowEx (WS_EX_CLIENTEDGE, szAppName, szTitle,
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
						  CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}



