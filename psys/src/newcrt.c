#define NEWCRT_G
#define HIRES

static long currentcolor=0xffff;
extern long cols[256];
static long bgc=0;

#include <stdio.h>
#include <stdbool.h>
#ifdef VARARGS
#include <varargs.h>
#else
#include <stdarg.h>
#endif
#include <signal.h>

#include <p2c/p2c.h>
#include <p2c/newcrt.h>

#ifndef MYLIB_H
#include <p2c/mylib.h>
#endif

#ifndef SYSDEVS_H
#include <p2c/sysdevs.h>
#endif
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
//#include <SDL3/SDL_video.h>
#include "SDL3_gfxPrimitives.h"
//#include <X11/X.h>
#ifdef OS2
//#include <X11/Xlib.h>
#endif  /* OS2 */
//#include <X11/Xutil.h>
//#include <X11/cursorfont.h>

#define TextW            80
#define TextH            24
#define WinX		 0
#define WinY		 (768-TextH*nc_fontheight)
#define WinW		 (TextW * nc_fontwidth)
#define WinH		 (TextH * nc_fontheight)
#define WinBorder	 1
#define WinDepth         4
#define colormask        (4096 * 7)
#define attrmask         0x0f00

#if defined(NEWCRT_FONT)
#define DefaultFont      NEWCRT_FONT
#else
#define DefaultFont      "8x13"
#endif

#if 0
#define NC_SCREEN(i)  (*({ int _i = (i);   \
                         if (_i < 0 || _i >= TextW*TextH)   \
			   _i=0, _i/=_i;   \
			 nc_screen + _i; }))
#else
#define NC_SCREEN(i)  (nc_screen[i])
#endif

#if 0
static long WindowEventMask = ExposureMask | KeyPressMask |
                              ButtonPressMask | ButtonReleaseMask | 
                              PointerMotionMask | StructureNotifyMask;
static unsigned long WinAttrMask = CWBackPixel | CWBorderPixel | CWEventMask |
                                   CWCursor;
static XSetWindowAttributes WinAttr = {
  None,				      /*  background_pixmap      */
  0,				      /*  background_pixel       */
  CopyFromParent,		      /*  border_pixmap		 */
  0,				      /*  border_pixel		 */
  ForgetGravity,		      /*  bit_gravity		 */
  NorthWestGravity,		      /*  win_gravity		 */
  NotUseful,			      /*  backing_store		 */
  0,     			      /*  backing_planes	 */
  0,				      /*  backing_pixel		 */
  False,			      /*  save_under		 */
  0,             		      /*  event_mask		 */
  0,				      /*  do_not_propogate_mask  */
  False,			      /*  override_redirect      */
  CopyFromParent,		      /*  colormap               */
  None,				      /*  cursor                 */
};
#endif
static char *progname = "newcrt";

static nc_windowRec __nc_curWindow = {
  0, TextH, 0, TextW,
  TextH * TextW,
  0, 0, 0, WinH, 0, WinW, 0, WinW-1, 0, WinH-1,
  0, 0, 0, 0, 0,
  0, NULL,
};

static nc_windowRec __nc_mainWindow = {
  0, TextH, 0, TextW,
  TextH * TextW,
  0, 0, 0, WinH, 0, WinW, 0, WinW-1, 0, WinH-1,
  0, 0, 0, 0, 0,
  0, NULL,
};

nc_windowRec *nc_curWindow = &__nc_curWindow;
nc_windowRec *nc_mainWindow = &__nc_mainWindow;
nc_windowRec *nc_lastLineWindow = &__nc_mainWindow;
nc_windowRec *nc_statusWindow = &__nc_mainWindow;
nc_windowRec *nc_defaultWindow = &__nc_mainWindow;

extern SDL_Color m_colors[ColorSets+1][ColorsInSet];    /*   From mylib   */
static int colortrans[8] = {
  m_green, m_yellow, m_black, m_red, m_cyan, m_white, m_blue, m_purple,
};
static int ascent;

//static GC nc_cursorgc;
static int cursor_flag;

char *usrgeo, *defgeo = {"640x312+60+20"};
int jc_winX, jc_winY, x_pad, y_pad;
/*unsigned*/ int jc_winW, jc_winH;

char * nc_usrfont;

static void WindowInitialize()
{
 
}

void nc_makeWindow(w, ptop, pheight, pleft, pwidth)
nc_windowRec *w;
int ptop, pheight, pleft, pwidth;
{
  w->top = ptop;
  w->height = pheight;
  w->left = pleft;
  w->width = pwidth;
  w->area = pwidth * pheight;
  w->font = 0;
  w->gcolor = 0;
  w->gtop = 0;
  w->gheight = pheight * nc_fontheight;
  w->gleft = 0;
  w->gwidth = pwidth * nc_fontwidth;
  w->gminx = 0;
  w->gmaxx = w->gwidth-1;
  w->gminy = 0;
  w->gmaxy = w->gheight-1;
  w->goffx = 0;
  w->goffy = 0;
  w->savex = 0;
  w->savey = 0;
  w->saveHigh = 0;
  w->saveSize = 0;
  w->saveArea = NULL;
}

void nc_initialize()
{
  int i, dir, des;
//  XCharStruct cs;
//  Font fontnum;

  if (! m_initialized)
    m_init_graphics();
 
  nc_screen = (nc_crtword *) malloc(TextW * TextH * sizeof(nc_crtword));
  for (i = 0; i < TextW * TextH; i++)
    NC_SCREEN(i).i = 32;
  XPOS = YPOS = 0;
  nc_initialized = 1;
}


void nc_setGraphics(on_or_off)
int on_or_off;
{
}


extern void CheckRefresh();

void nc_putChar(x, y, c)
int x, y;
uchar c;
{
  characterRGBA(m_renderer,x,y,c,255,255,255,255);
}


static void show_string(str)
char *str;
{
  unsigned char c;

  while (*str) {
    c = (unsigned char) *str;
    if ((c < ' ') || (c > 126))
      fprintf(stderr, "[%d]", c);
    else
      fprintf(stderr, "%c", c);
    str++;
  }
}


void nc_putChars(x, y, len, cp)      /* daveg, 10/6/89 */
int x, y, len;
char *cp;
{
  if (! nc_text_in_window) {
    fprintf(stderr, "Warning!  nc_putChars(%d,%d,%d,'", x, y, len);
    show_string(cp);
    fprintf(stderr, "') called\n");
    return;
  }
  if (! nc_initialized) 
    nc_initialize();
  CheckRefresh();
  if (x < 0) {
    cp -= x;
    len += x;
    x = 0;
  }
  if (x + len > nc_curWindow->width) {
    len = nc_curWindow->width - x;
  }
  if (len > 0 && y >= 0 && y < nc_curWindow->height) {
    int i;
    for (i = 0; i < len; i++)
      NC_SCREEN(y*TextW+x+i).i = nc_highlight + cp[i];
    stringRGBA(m_renderer, nc_curWindow->gleft+x*nc_fontwidth, 
		     nc_curWindow->top+y*nc_fontheight + ascent, 
		     cp, 255,255,255,255);

    if (nc_highlight & nc_under) {
      SDL_RenderLine(m_renderer,
		nc_curWindow->gleft+x*nc_fontwidth, 
		nc_curWindow->top+y*nc_fontheight + ascent + 1, 
		nc_curWindow->gleft+x*nc_fontwidth + len*nc_fontwidth - 1, 
		nc_curWindow->top+y*nc_fontheight + ascent + 1);
    }
  }
}

static void refresh_area(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
  nc_crtword *p;
  int i, j, len;
  short savehighlight = nc_highlight;
  char linebuf[TextW];

  if (nc_initialized) {
    for (j = y1; j <= y2; j++)
      for (i = x1, p = nc_screen + j*TextW; i <= x2; i += len, p += len) {
	nc_setHighlight(p->i & ~0377);
	if (bigendian)
	  {
	    for (len = 0; len+i <= x2 && p[len].U1.h == p->U1.h; len++)
	      linebuf[len] = p[len].U1.c;
	  }
	else
	  {
	    for (len = 0; len+i <= x2 && p[len].U1.c == p->U1.c; len++)
	      linebuf[len] = p[len].U1.h;
	  }
	stringRGBA(m_renderer,
			 i * nc_fontwidth,
			 j * nc_fontheight + ascent, linebuf, 255,255,255,255);
	if (nc_highlight & nc_under) {
	  SDL_RenderLine(m_renderer,
		    i*nc_fontwidth,
		    j*nc_fontheight + ascent + 1, 
		    i*nc_fontwidth + len*nc_fontwidth - 1, 
		    j*nc_fontheight + ascent + 1);
	}
      }
    nc_setHighlight(savehighlight);
  }
}

void nc_refreshScreen()
{
  if (nc_initialized) {
    refresh_area(0, 0, TextW-1, TextH-1);
  }
}
    
void CheckRefresh()
{
}

void nc_setWindow_(w)
nc_windowRec *w;
{
  if (! nc_text_in_window) {
    return;
  }
  if (! nc_initialized)
    nc_initialize();
  CheckRefresh();
  nc_curWindow = w;
}

void nc_setHighlight(newhighlight)
int newhighlight;
{
  //fprintf(stderr,"sethighlight %d\n",newhighlight);
  if (newhighlight & nc_inv) {
      currentcolor=cols[0];
      bgc=cols[colortrans[(newhighlight & colormask)/4096]];
               
    
  } else {
      currentcolor=cols[colortrans[(newhighlight & colormask)/4096]];
      bgc=0;
  }
//  if (newhighlight==0)
//  currentcolor=nc_green;
  nc_highlight=newhighlight;
}
extern unsigned char * font8x13;
void nc_putStr(x, y, str)
int x, y;
Char *str;
{
  SDL_FRect r={(float)(x*nc_fontwidth),(float)(y*nc_fontheight),(float)(strlen(str)*nc_fontwidth),(float)nc_fontheight};
  SDL_SetRenderDrawColor(m_renderer,(bgc)&255,(bgc>>8)&255,(bgc>>16)&255,255);
  SDL_RenderFillRect(m_renderer,&r);
  gfxPrimitivesSetFont(&font8x13,8,13);
  stringColor(m_renderer,x*nc_fontwidth,y*nc_fontheight,str,currentcolor);
}


void nc_writeStr(x, y, str_)
int x, y;
Char *str_;
{
  int i, len;
  unsigned char *str = (unsigned char *)str_;

  if (! nc_text_in_window) {
    fprintf(stderr, "Warning!  nc_writeStr(%d,%d,'", x, y);
    show_string(str_);
    fprintf(stderr, "') called\n");
    return;
  }
  if (! nc_initialized)
    nc_initialize();
  if (x < 0)
    x = 0;
  if (x >= nc_curWindow->width) {
    x = 0;
    y++;
  }
  if (y < 0)
    y = 0;
  if (y >= nc_curWindow->height) {
    nc_scrollUp();
    y = nc_curWindow->height-1;
  }
  XPOS = x;
  YPOS = y;
  for (i = 0; *str != '\0'; i++, str++) {
    if (*str < 32)
      switch (*str) {
      case 1:                /*  Cursor home		 */
	XPOS = YPOS = 0;
	break;
      case 8:                /*  Cursor left		 */
	if (--XPOS < 0) {
	  if (YPOS > 0) {
	    XPOS = nc_curWindow->width-1;
	    YPOS--;
	  } else
	    XPOS = YPOS = 0;
	}
	break;
      case 9:                /*  Clear to EOL		 */
	nc_clearXY(XPOS, YPOS, nc_curWindow->width-XPOS, 1);
	break;
      case 10:               /*  Cursor down             */
	if (++YPOS == nc_curWindow->height) {
	  nc_scrollUp();
	  YPOS--;
	}
	break;
      case 11:               /*  Clear to end of screen  */
	nc_clearXY(XPOS, YPOS, nc_curWindow->width-XPOS, 1);
	nc_clearXY(0, YPOS+1, nc_curWindow->width,nc_curWindow->height-YPOS-1);
	break;
      case 12:               /*  Clear screen            */
	nc_clearXY(0, 0, nc_curWindow->width, nc_curWindow->height);
	XPOS = 0;
	YPOS = 0;
	break;
      case 13:               /*  Carriage Return         */
	XPOS = 0;
	break;
      case 28:               /*  Cursor right            */
	if (++XPOS == nc_curWindow->width) {
	  XPOS = 0;
	  if (++YPOS == nc_curWindow->height) {
	    nc_scrollUp();
	    YPOS--;
	  }
	}
	break;
      case 31:               /*  Cursor up               */
	if (YPOS > 0)
	  YPOS--;
	break;
      }
    else if (*str >= 128) 
      switch (*str) {
      case 128:              /*  Normal     */
	nc_setHighlight(nc_highlight & colormask);
	break;
      case 129:              /*  Inverse    */
	nc_setHighlight((nc_highlight & colormask) + nc_inv);
	break;
      case 130:              /*  Blinking   */
	nc_setHighlight((nc_highlight & colormask) + nc_blink);
	break;
      case 131:              /*  B + I      */
	nc_setHighlight((nc_highlight & colormask) + nc_blink + nc_inv);
	break;
      case 132:              /*  Underline  */
	nc_setHighlight((nc_highlight & colormask) + nc_under);
	break;
      case 133:              /*  U + I      */
	nc_setHighlight((nc_highlight & colormask) + nc_under + nc_inv);
	break;
      case 134:              /*  U + B      */
	nc_setHighlight((nc_highlight & colormask) + nc_under + nc_blink);
	break;
      case 135:              /*  U + B +I   */
	nc_setHighlight((nc_highlight & colormask) + nc_under+nc_blink+nc_inv);
	break;
      case 136:              /*  Green      */
	nc_setHighlight((nc_highlight & attrmask) + nc_green);
	break;
      case 137:              /*  Yellow     */
	nc_setHighlight((nc_highlight & attrmask) + nc_yellow);
	break;
      case 138:              /*  Black      */
	nc_setHighlight((nc_highlight & attrmask) + nc_black);
	break;
      case 139:              /*  Red        */
	nc_setHighlight((nc_highlight & attrmask) + nc_red);
	break;
      case 140:              /*  Cyan       */
	nc_setHighlight((nc_highlight & attrmask) + nc_cyan);
	break;
      case 141:              /*  White      */
	nc_setHighlight((nc_highlight & attrmask) + nc_white);
	break;
      case 142:              /*  Blue       */
	nc_setHighlight((nc_highlight & attrmask) + nc_blue);
	break;
      case 143:              /*  Purple     */
	nc_setHighlight((nc_highlight & attrmask) + nc_purple);
	break;
      }
    else {
      len = 0;
      do {
	NC_SCREEN(YPOS*TextW+XPOS+len).i = nc_highlight + str[len];
	len++;
      } while (str[len] >= ' ' && str[len] < 128 &&
	       XPOS + len < nc_curWindow->width);
         unsigned char c=str[len];
      str[len]=0;
         nc_putStr(XPOS,YPOS,(char *)str);
         str[len]=c;
     // stringColor(m_renderer,  
		  //     nc_curWindow->gleft+nc_fontwidth*XPOS,
		  //     nc_curWindow->gtop+nc_fontheight*YPOS + ascent,
		  //     (char *)str, currentcolor);
         
      if (nc_highlight & nc_under) {
  	SDL_RenderLine(m_renderer,
		  nc_curWindow->gleft+XPOS*nc_fontwidth, 
		  nc_curWindow->top+YPOS*nc_fontheight + ascent + 1, 
		  nc_curWindow->gleft+XPOS*nc_fontwidth + len*nc_fontwidth-1, 
		  nc_curWindow->top+YPOS*nc_fontheight + ascent + 1);
      }
      str += len - 1;
      XPOS += len;
      if (XPOS == nc_curWindow->width) {
	XPOS = 0;
	if (++YPOS == nc_curWindow->height) {
	  nc_scrollUp();
	  YPOS--;
	}
      }
    }
  }
  CheckRefresh();
/*  fprintf(stderr, "nc_writeStr exits, cursor at (%d,%d)\n", XPOS, YPOS);  */
}

static void Handle_Graphics_Exposures()
{
 
}

void nc_scrollUp()
{
 
}

void nc_clearXY(x, y, dx, dy)
int x, y, dx, dy;
{
  int i, j;

  if (! nc_text_in_window) {
    return;
  }
  if (nc_initialized) {
/*    fprintf(stdout, "nc_clearXY(%d, %d, %d, %d)\n", x, y, dx, dy);  
    getchar();    */
    if ((dx > 0) && (dy > 0)) {
      SDL_FRect r={(float)(nc_curWindow->gleft+nc_fontwidth*x),
                              (float)(nc_curWindow->gtop+nc_fontheight*y),
                              (float)(nc_fontwidth*dx), (float)(nc_fontheight*dy)};
      SDL_SetRenderDrawColor(m_renderer,0,0,0,255);                        
      SDL_RenderFillRect(m_renderer, &r);
      for (j = y; j < y + dy; j++)
	for (i = x; i < x + dx; i++)
	  NC_SCREEN(j * TextW + i).i = 32;
    }
/*    getchar();
    XNoOp(m_display);
    XFlush(m_display);  */
    CheckRefresh();
  }
}

void nc_getXY(x, y)
long *x, *y;
{
  if (! nc_text_in_window) {
    *x = *y = 0;
    return;
  }
  if (! nc_initialized)
    nc_initialize();
  *x = (long) XPOS;
  *y = (long) YPOS;
  CheckRefresh();
}

void nc_gotoXY(x, y)
int x, y;
{
/*  fprintf(stderr, "nc_gotoXY(%d,%d)\n", x, y);  */
  if (! nc_text_in_window) {
    return;
  }
  if (! nc_initialized)
    nc_initialize();
  XPOS = x;
  YPOS = y;
  CheckRefresh();
}


static void fix_string(str, buf)
unsigned char *str, *buf;
{
  unsigned char *cp1, *cp2;
  
  cp2 = buf;
  for (cp1 = str; *cp1; cp1++) {
    switch (*cp1) {
    case 1:                /*  Cursor home		 */
    case 8:                /*  Cursor left		 */
    case 9:                /*  Clear to EOL		 */
    case 11:               /*  Clear to end of screen    */
    case 12:               /*  Clear screen              */
    case 28:               /*  Cursor right              */
    case 31:               /*  Cursor up                 */
      break;
    case 10:               /*  Cursor down               */
    case 13:               /*  Carriage Return           */
    default:
      if (*cp1 <= 127)
	*cp2++ = *cp1;
    }
  }
  *cp2 = '\0';
}

#ifdef VARARGS

void nc_printf(va_alist)
va_dcl
{
  va_list args;
  Char *fmt, fmt2[1024];
  Char printbuffer[2000], printbuffer2[2000];
  Char *p1, *p2;

/*  fprintf(stderr, "nc_printf() called, cursor at (%d,%d)\n", XPOS, YPOS);  */
  va_start(args);
  fmt = va_arg(args, Char *);

  if (! nc_text_in_window) {
    fix_string((unsigned char *) fmt, (unsigned char *) fmt2);
    vprintf(fmt2, args);
    va_end(args);
    return;
  }
  vsprintf(printbuffer, fmt, args);
  va_end(args);
  for (p1 = printbuffer, p2 = printbuffer2; *p1 != '\0'; p1++, p2++)
    if ((*p2 = *p1) == '\n')
      *(++p2) = '\015';
  *p2 = '\0';

  nc_writeStr(XPOS, YPOS, printbuffer2);
/*  fprintf(stderr, "nc_printf() exits, cursor at (%d,%d)\n", XPOS, YPOS);  */
}

#else

void nc_printf(Char * fmt, ...)

{
  va_list args;
  Char fmt2[1024];
  Char printbuffer[2000], printbuffer2[2000];
  Char *p1, *p2;

/*  fprintf(stderr, "nc_printf() called, cursor at (%d,%d)\n", XPOS, YPOS);  */

  va_start(args, fmt);

  if (! nc_text_in_window) {
    fix_string((unsigned char *) fmt, (unsigned char *) fmt2);
    vprintf(fmt2, args);
    va_end(args);
    return;
  }
  vsprintf(printbuffer, fmt, args);
  va_end(args);
  for (p1 = printbuffer, p2 = printbuffer2; *p1 != '\0'; p1++, p2++)
    if ((*p2 = *p1) == '\n')
      *(++p2) = '\015';
  *p2 = '\0';

  nc_writeStr(XPOS, YPOS, printbuffer2);

}

#endif /* OS2 */


#ifdef VARARGS

void zfprintf(va_alist)
va_dcl

{
  va_list args;
  FILE *outfile;
  Char *fmt;
  Char fmt2[1024];
  Char printbuffer[2000], printbuffer2[2000];
  Char *p1, *p2;

  va_start(args);
  outfile = va_arg(args, FILE *);
  fmt = va_arg(args, Char *);
  vfprintf(outfile, fmt, args);
}

#else

void zfprintf(FILE * outfile, Char * fmt, ...)

{
  va_list args;
  Char fmt2[1024];
  Char printbuffer[2000], printbuffer2[2000];
  Char *p1, *p2;

  va_start(args, fmt);
/* This leaves OS2 a no-op  */
}

#endif

#ifdef VARARGS

void nc_fprintf(va_alist)
va_dcl
{
  va_list args;
  FILE *outfile;
  Char *fmt, fmt2[1024];
  Char printbuffer[2000], printbuffer2[2000];
  Char *p1, *p2;

  va_start(args);
  outfile = va_arg(args, FILE *);
  fmt = va_arg(args, Char *);
/*  
  vfprintf(outfile, fmt, args);
*/
  if (nc_text_in_window && ((outfile == stdout)/* || (outfile == stderr) */)) {
    vsprintf(printbuffer, fmt, args);
    va_end(args);
    for (p1 = printbuffer, p2 = printbuffer2; *p1 != '\0'; p1++, p2++)
      if ((*p2 = *p1) == '\n')
	*(++p2) = '\015';
    *p2 = '\0';
    
    nc_writeStr(XPOS, YPOS, printbuffer2);
  } else {
    fix_string((unsigned char *) fmt, (unsigned char *) fmt2);
    vfprintf(outfile, fmt2, args);
    va_end(args);
  }
}

#else

void nc_fprintf(FILE * outfile, Char * fmt, ...)

{
  va_list args;
  Char fmt2[1024];
  Char printbuffer[2000], printbuffer2[2000];
  Char *p1, *p2;

  va_start(args, fmt);

  if (nc_text_in_window && ((outfile == stdout)/* || (outfile == stderr) */)) {
    vsprintf(printbuffer, fmt, args);
    va_end(args);
    for (p1 = printbuffer, p2 = printbuffer2; *p1 != '\0'; p1++, p2++)
      if ((*p2 = *p1) == '\n')
	*(++p2) = '\015';
    *p2 = '\0';
    
    nc_writeStr(XPOS, YPOS, printbuffer2);
  } else {
    fix_string((unsigned char *) fmt, (unsigned char *) fmt2);
    vfprintf(outfile, fmt2, args);
    va_end(args);
  }
}

#endif

void nc_puts(Char * s)

{
  char str2[1024];

/*  fprintf(stderr, "nc_puts() called, cursor at (%d,%d)\n", XPOS, YPOS);  */
  if (! nc_text_in_window) {
    fix_string((unsigned char *) s, (unsigned char *) str2);
#undef puts
    puts(str2);
#define puts nc_puts
    return;
  }
  nc_writeStr(XPOS, YPOS, s);
  nc_writeStr(XPOS, YPOS, "\n\015");
/*  fprintf(stderr, "nc_puts() exits, cursor at (%d,%d)\n", XPOS, YPOS);  */
}
  
void nc_fputs(Char * s, FILE *outfile)

{
  Char printbuffer[2000];
  Char *p;
  Char str2[1024];

  if (nc_text_in_window && ((outfile == stdout) || (outfile == stderr))) {
    for (p = printbuffer; *s != '\0'; s++, p++)
      if ((*p = *s) == '\n')
	*(++p) = '\015';
    *p = '\0';
    nc_writeStr(XPOS, YPOS, printbuffer);
  } else {
    fix_string((unsigned char *) s, (unsigned char *) str2);
#undef fputs
    fputs(str2, outfile);
#define fputs nc_fputs
  }
} 


void nc_putchar(uchar c)

{
  Char buffer[2];

  if (! nc_text_in_window) {
    if (c <= 127)
      nc_putc(c, stdout);
    return;
  }
/*  fprintf(stderr, "nc_putchar() called, cursor at (%d,%d)\n", XPOS, YPOS);  */
  if (c == '\n')
    nc_writeStr(XPOS, YPOS, "\015\n");
  else {
    buffer[0] = c;
    buffer[1] = '\0';
    nc_writeStr(XPOS, YPOS, buffer);
  }
/*  fprintf(stderr, "nc_putchar() exits, cursor at (%d,%d)\n", XPOS, YPOS);  */
}

void nc_putc(uchar c, FILE *outfile)

{
  uchar buffer[2];

  if (nc_text_in_window && ((outfile == stdout) || (outfile == stderr)))
    nc_putchar(c);
  else if (c <= 127)
    fputc(c, outfile);

}

void nc_togglecursor()
{
  if (! nc_initialized)
    return;
  cursor_flag = ! cursor_flag;
}

void nc_cursor_on()
{
  if (!cursor_flag)
    nc_togglecursor();
}

void nc_cursor_off()
{
  if (cursor_flag)
    nc_togglecursor();
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

char *nc_gets(buf)
char *buf;
{
  static char *cursor = "\374";
  static char *space = " ";
  int pos;
  uchar c;

  if (! nc_text_in_window) {
//#undef gets
    return(gets(buf));
#define gets nc_gets
  }
  if (! nc_initialized)
    nc_initialize();
  pos = 0;
  for (;;) {
/*    fprintf(stderr, "Text cursor at (%d, %d)\n", XPOS, YPOS);  */
    c = m_inkey();
    if (c == '\10') {
      if (pos > 0) {
	pos--;
	nc_writeStr(XPOS, YPOS, "\b \b");
      }
#if 0
      XDrawImageString(m_display, nc_window, nc_gc,
		       nc_curWindow->gleft+nc_fontwidth*XPOS,
		       nc_curWindow->gtop+nc_fontheight*YPOS + ascent,
		       space, 1);
#endif
    } else if (c == 'U' - '@') {
      while (pos > 0) {
	pos--;
	nc_writeStr(XPOS, YPOS, "\b \b");
      }
    } else if (c == '\15') {
      XPOS = 0;
      if (++YPOS >= nc_curWindow->height) {
	YPOS--;
	nc_scrollUp();
      }
      buf[pos] = '\0';
#if 0
      XDrawImageString(m_display, nc_window, nc_gc,
		       nc_curWindow->gleft+nc_fontwidth*XPOS,
		       nc_curWindow->gtop+nc_fontheight*YPOS + ascent,
		       space, 1);
#endif
      return(buf);
#if 0
    } else if (c == 'C' - '@') {
      kill(getpid(), SIGINT);
#endif
    } else if (c >= ' ' && c < 127) {
      buf[pos++] = c;
      nc_putChar(XPOS, YPOS, c);
#if 0
      NC_SCREEN(YPOS*TextW+XPOS).i = nc_highlight + c;
      XDrawImageString(m_display, nc_window, nc_gc,
		       nc_curWindow->gleft+nc_fontwidth*XPOS,
		       nc_curWindow->gtop+nc_fontheight*YPOS + ascent,
		       &c, 1);
#endif
      if (++XPOS == nc_curWindow->width) {
	XPOS = 0;
	if (++YPOS == nc_curWindow->height) {
	  nc_scrollUp();
	  YPOS--;
	}
      }
#if 0
      XDrawImageString(m_display, nc_window, nc_gc,
		       nc_curWindow->gleft+nc_fontwidth*XPOS,
		       nc_curWindow->gtop+nc_fontheight*YPOS + ascent,
		       cursor, 1);
#endif
    }
  }
}

#pragma clang diagnostic pop

char *nc_fgets(s, n, stream)
char *s;
int n;
FILE *stream;
{
  if (nc_text_in_window && (stream == stdin))
    return(nc_gets(s));
  else
#undef fgets
    return(fgets(s, n, stream));
#define fgets nc_fgets
}




short nc_gType()
{
  return nc_g300;
}

boolean nc_alphashared()
{
  return true;
}

