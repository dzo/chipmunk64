#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include "SDL2_gfxPrimitives.h"
// #include <GL/gl.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#define MYLIB_G
#define XK_MISCELLANY

#define ENABLE_DEBUGGING 1
typedef struct {
    int x;
    int y;
} XPoint;

/* Trying to speed up graphics */
#undef SAVECURSOR
#undef EXTRA_BUFFERING

/* Support for 24-plane TrueColor X */
#define HIRES
/* #define HIRESDB */ /* debugging only */

#define __TYPES__

#include <stdio.h>
#ifdef HIRES
#include <stdlib.h>
#include <string.h>
#endif /* HIRES */
#include <math.h>

#if defined(aux)
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#elif defined(__sgi)
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#elif defined(rs6000)
#include <sys/time.h>
#include <time.h>
#elif defined(BSD)
#include <sys/time.h>
#else
#include <time.h>
#endif
#ifdef OS2
#include <sys/timeb.h>
#endif

#include <p2c/p2c.h>
#include <p2c/mylib.h>

#ifdef HIRES

#endif /* HIRES */

#if 0
/* previously missing defs in keysymdef */
#define XK_Reset 0x1000FF6C /* HP -- The shift of Break */
#endif

#if defined(rs6000)
/* previously missing defs in keysymdef -- may be needed for RS/6000 */
#define XK_System 0x1000FF6D     /* HP */
#define XK_User 0x1000FF6E       /* HP */
#define XK_ClearLine 0x1000FF6F  /* HP */
#define XK_InsertLine 0x1000FF70 /* HP */
#define XK_DeleteLine 0x1000FF71 /* HP */
#define XK_InsertChar 0x1000FF72 /* HP */
#define XK_DeleteChar 0x1000FF73 /* HP */
#define XK_BackTab 0x1000FF74    /* HP */
#define XK_KP_BackTab 0x1000FF75 /* HP */
#endif

SDL_Color m_colors[ColorSets + 1][ColorsInSet];
long cols[256];

int WindowWidth = 1280, WindowHeight = 720;

/* daveg, 10/6/89:  Just to improve readability of the rest of the code! */

static int show_all_mylib_calls, show_all_X_calls, show_flushes, show_pen_calls,
    show_key_calls, sync_all_calls;

static void init_debug_flags() {
    show_all_mylib_calls = (getenv("SHOW_ALL_MYLIB_CALLS") != NULL);
    show_all_X_calls = (getenv("SHOW_ALL_X_CALLS") != NULL);
    show_flushes = (getenv("SHOW_FLUSHES") != NULL);
    show_pen_calls = (getenv("SHOW_PEN_CALLS") != NULL);
    show_key_calls = (getenv("SHOW_KEY_CALLS") != NULL);
    sync_all_calls = (getenv("SYNC_ALL_CALLS") != NULL);
}

extern void nc_refreshScreen PP((void));
extern void nc_cursor_on PP((void));
extern void nc_cursor_off PP((void));

#define zfprintf fprintf /* MDG */
extern int zfprintf(FILE *, const char *, ...);

#ifdef ENABLE_DEBUGGING
#define Mfprintf show_all_mylib_calls &&fprintf
#define Xfprintf show_all_X_calls &&fprintf
#define Ffprintf show_flushes &&fprintf
#define Pfprintf show_pen_calls &&fprintf
#define Kfprintf show_key_calls &&fprintf
#else
#define Mfprintf 0 && fprintf
#define Xfprintf 0 && fprintf
#define Ffprintf 0 && fprintf
#define Pfprintf 0 && fprintf
#define Kfprintf 0 && fprintf
#endif

/* #define XFlush(d)  if (sync_all_calls) XSync(d,False); else XFlush(d) */

/*  newcrt stuff  */
#define nc_fontwidth 8
#define nc_fontheight 13
extern SDL_Window *nc_window;
extern int nc_initialized;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define TRNSFRM(x, y)     \
    if (flip)             \
        y = m_down - (y); \
    else if (trans)       \
        GeneralTransform(&(x), &(y));
#define DTRNSFRM(x, y) \
    if (flip)          \
        y = -y;        \
    else if (trans)    \
        DeltaTransform(&(x), &(y));
#define LTRNSFRM(x, y)    \
    if (flip)             \
        y = m_down - (y); \
    else if (trans)       \
        LimitedTransform(&(x), &(y));
#define UNTRNSFRM(x, y)   \
    if (flip)             \
        y = m_down - (y); \
    else if (trans)       \
        GeneralUnTransform(&(x), &(y));
#define pi 3.1415926535897932384626433833
#define dr 0.0174532925199433
#define WinBorder 1
#define WinDepth 4

#if defined(MYLIB_FONT)
#define DefaultFont MYLIB_FONT
#else
#define DefaultFont "6x10"
#endif

#ifdef OS2
static struct timeb first, second, lapsed;
#else
static struct timeval first, second, lapsed;
static struct timezone tzp;
#endif

static char *timername;

void starttimer(name) char *name;
{
    timername = name;

#ifdef OS2
    _ftime(&first);
#else
    gettimeofday(&first, &tzp);
#endif
}

void stoptimer() {
#ifdef OS2
    _ftime(&second);

    if (first.millitm > second.millitm) {
        second.millitm += 1000;
        second.time--;
    }
    lapsed.time = second.time - first.time;
    lapsed.millitm = second.millitm - first.millitm;
    printf("%s:  %f seconds\n", timername,
           lapsed.time + lapsed.millitm / 1000.0);

#else

    gettimeofday(&second, &tzp);

    if (first.tv_usec > second.tv_usec) {
        second.tv_usec += 1000000;
        second.tv_sec--;
    }
    lapsed.tv_usec = second.tv_usec - first.tv_usec;
    lapsed.tv_sec = second.tv_sec - first.tv_sec;

    printf("%s:  %f seconds\n", timername,
           lapsed.tv_sec + lapsed.tv_usec / 1000000.0);
#endif
}

/* Added for command line display specification. - stafford 7/17/91 */

#define DISPLAY_NAME_LENGTH 100
char m_display_name[DISPLAY_NAME_LENGTH] = "";

/* Added flag for window pop-up mode -- jl */

boolean m_autoraise = false;

// Display *m_display;
SDL_Window *m_window;
int screennum;
int BlackAndWhite = False;
int m_events_received;

/*int m_across, m_down, m_maxcolor;*/

static int nocache;
static int flip, trans;
static int trans_XtoX, trans_XtoY, trans_YtoY, trans_YtoX, trans_denom,
    trans_addx, trans_addy;
/*
static GC gc[ColorsInSet];
static GC CursorGC, CursorGC2, CursorGC3;
static Pixmap UnderCursor;
*/
static int currentcolor = 0;
static int currentcolorindex = 0;
int currentmode = 0;
// static Font fontnum;
static int currentfont;
static int fontasc;

static int RealWinDepth = WinDepth;
// static Colormap colormap;
static unsigned long plane_masks[1 << WinDepth];
static unsigned long plane_mask;
static unsigned long pixel;
#ifdef HIRES
static int planeCount;
#endif /* HIRES */

static unsigned long notAllPlanes;
/*
static struct grid {
  int dx, dy, ax, ay;
  unsigned long color;
} grid1 = { -1, -1, -1, -1, 0, 0, 0, 0, 0 },
  grid2 = { -1, -1, -1, -1, 0, 0, 0, 0, 0 },
  *newgrid, *oldgrid;
*/
static char *progname = "mylib";

void GeneralTransform(x, y) int *x, *y;
{
    int newx, newy;

    newx = (*x * trans_XtoX + *y * trans_YtoX) / trans_denom + trans_addx;
    newy = (*x * trans_XtoY + *y * trans_YtoY) / trans_denom + trans_addy;

    *x = newx;
    *y = newy;
}

void DeltaTransform(x, y) int *x, *y;
{
    int newx, newy;

    newx = (*x * trans_XtoX + *y * trans_YtoX) / trans_denom;
    newy = (*x * trans_XtoY + *y * trans_YtoY) / trans_denom;

    *x = newx;
    *y = newy;
}

void LimitedTransform(x, y) int *x, *y;
{
    int newx, newy;

    newx = *x * trans_XtoX / trans_denom + trans_addx;
    newy = *y * trans_YtoY / trans_denom + trans_addy;

    *x = newx;
    *y = newy;
}

void GeneralUnTransform(x, y) int *x, *y;
{
    int x1, y1, newx, newy, det;

    x1 = *x * trans_denom - trans_addx;
    y1 = *y * trans_denom - trans_addy;

    det = (trans_XtoX * trans_YtoY - trans_YtoX * trans_XtoY);
    newx = (x1 * trans_YtoY - y1 * trans_YtoX) / det;
    newy = (y1 * trans_XtoX - x1 * trans_XtoY) / det;

    *x = newx;
    *y = newy;
}

void m_cache(newstate) int newstate;
{
    if (!newstate && !nocache) {
        Ffprintf(stderr, "XFlush()\n");
        // XFlush(m_display);
    }
    nocache = !newstate;
}

void m_choosecolors(colorset) int colorset;
{}

/* Added X display name support.  stafford 7/17/91 */

static int ScrDepth;
int red_size, green_size, blue_size, red_shift, green_shift, blue_shift;
int red_mask, green_mask, blue_mask;

void DisplayInitialize() {
    currentfont = 0;
    fontasc = 8;  // TTF_FontHeight(currentfont) ;//->max_bounds.ascent;
}

/* These are WOL-versions of function-keys */
#define wol_select "\003"
#define wol_left_arrow "\b"
#define wol_right_arrow "\034"
#define wol_down_arrow "\n"
#define wol_up_arrow "\037"

void mapkey() {}
unsigned curxor[5][32] = {
    {0xf0, 0xc0, 0xa0, 0x90, 0x08, 0x04, 0x02, 0x01, 0, 0, 0, 0, 0, 0, 0, 0,
     0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0, 0, 0, 0},
    {0xf0,   0xc0, 0xa0, 0x90, 0x08, 0x04, 0x02, 0x01, 0x8000, 0x4000, 0x2000,
     0x1000, 0,    0,    0,    0,    0,    0,    0,    0,      0,      0,
     0,      0,    0,    0,    0,    0,    0,    0,    0,      0},
    {0x10,   0x08,   0x08,   0x84,   0x64,   0x1e,   0x06,   0xc01,
     0xb200, 0x7200, 0xce00, 0xc000, 0x2001, 0x2001, 0xe000, 0,
     0,      0,      0,      0,      0,      0,      0,      0,
     0,      0,      0,      0,      0,      0,      0,      0},
    {0x4000, 0x4000,   0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
     0x4000, 0xe0ffff, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
     0x4000, 0x4000,   0x4000, 0,      0,      0,      0,      0,
     0,      0,        0,      0,      0,      0,      0,      0},
    {0x4000, 0x4000,   0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
     0x4000, 0xe0ffff, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
     0x4000, 0x4000,   0x4000, 0,      0,      0,      0,      0,
     0,      0,        0,      0,      0,      0,      0,      0}};
SDL_Cursor *cursors[32];
unsigned curand[32] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int curpos[5] = {0, 0, 6, 9, 9};
unsigned curzero[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

SDL_Texture *buffer;
void WindowInitialize() {
    // SDL_Init(SDL_INIT_VIDEO);
    // SDL_SetHint(SDL_HINT_EMSCRIPTEN_ASYNCIFY, "1");
    //...
    Xfprintf(stderr, "SDL_Init\n");
    SDL_Init(SDL_INIT_VIDEO);
    // SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK,
    // SDL_GL_CONTEXT_PROFILE_CORE); //OpenGL core profile SDL_GL_SetAttribute
    // (SDL_GL_CONTEXT_MAJOR_VERSION, 3); //OpenGL 3+ SDL_GL_SetAttribute
    // (SDL_GL_CONTEXT_MINOR_VERSION, 2); //OpenGL 3.3 SDL_SetHint
    // (SDL_HINT_RENDER_DRIVER, "opengl") ;
    //   SDL_SetHint (SDL_HINT_RENDER_VSYNC,"1");
    // SDL_CreateWindowAndRenderer(WindowWidth,WindowHeight,
    // SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL, &m_window, &m_renderer);

 // buffer = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGB888,
 //                                       SDL_TEXTUREACCESS_TARGET, WindowWidth, WindowHeight); 
 //  SDL_SetRenderTarget(m_renderer, buffer);                                      
  //SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,31);
  for(int i=0;i<5;i++)
     cursors[i]=SDL_CreateCursor((const unsigned char *)curzero,(const unsigned char *)(curxor[i]),32,32,curpos[i],curpos[i]);
  Xfprintf(stderr, "SDL_CreateWindow\n");

  #ifdef __EMSCRIPTEN__
  m_window=SDL_CreateWindow("log",200,200,WindowWidth,WindowHeight,0);
  #else
  m_window=SDL_CreateWindow("log",200,200,WindowWidth,WindowHeight,SDL_WINDOW_SHOWN|  SDL_WINDOW_RESIZABLE );
  #endif
  Xfprintf(stderr, "SDL_CreateRenderer\n");
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
  SDL_SetHint(SDL_HINT_RENDER_BATCHING,"1");
  m_renderer=SDL_CreateRenderer(m_window,-1,SDL_RENDERER_ACCELERATED);
//  m_renderer=SDL_CreateRenderer(m_window,-1,0);
//SDL_CreateWindowAndRenderer(WindowWidth,WindowHeight, 0, &m_window, &m_renderer);

//   SDL_GLContext openglContext = SDL_GL_CreateContext (m_window);
//    printf ("glGetString (GL_VERSION) returns %s\n", glGetString (GL_VERSION));
//  SDL_RendererInfo info;
//  SDL_GetRendererInfo(m_renderer,&info);
//  printf("Renderer %x\n",info.flags);
  //#ifdef __EMSCRIPTEN__
  Xfprintf(stderr, "SDL_CreateTexture\n");
  buffer = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_TARGET, WindowWidth, WindowHeight); 
  Xfprintf(stderr, "SDL_SetRenderTarget\n");
  SDL_SetRenderTarget(m_renderer, buffer);
  //#endif
}

#define LINESTIPPLELENGTH 4

static unsigned char dotted[LINESTIPPLELENGTH] = {2, 1, 2, 1};
static unsigned char dot_dashed[LINESTIPPLELENGTH] = {3, 4, 3, 1};
static unsigned char sh_dashed[LINESTIPPLELENGTH] = {4, 3, 4, 3};
static unsigned char lo_dashed[LINESTIPPLELENGTH] = {7, 1, 7, 1};
static unsigned char odd_dashed[LINESTIPPLELENGTH] = {1, 2, 3, 4};
static unsigned char black_dashed[LINESTIPPLELENGTH] = {1, 7, 1, 7};

static unsigned char *default_linestyle[ColorsInSet];

static unsigned char *FigureOutBWLine(r, g, b)
int r, g, b;
{
    /* This is just a matter of taste */

    if ((r > 10) && (g == 0) && (b == 0)) /* red */
        return lo_dashed;
    else if ((r > 11) && (g > 12) && (b == 0))
        /* yellow */
        return sh_dashed;
    else if ((r < 2) && (g < 2) && (b < 2))
        /* black */
        return lo_dashed;
    else
        return NULL;
}

extern int WindowWidth, WindowHeight;
static void do_init_screen(full) int full;
{
    int i, x, y;
    int w, h, bw, d;

    init_debug_flags();

    if (!m_initialized) DisplayInitialize();

    if (!m_initialized) WindowInitialize();

    if (full) m_clear();

    Xfprintf(stderr, "XGetGeometry()\n");
    SDL_GetWindowSize(m_window, &w, &h);
    m_across = WindowWidth;
    m_down = WindowHeight;
    RealWinDepth = d;
    m_across--;
    m_down--;
    m_maxcolor = 16;
    m_color(m_red);
    m_choosecursor(0);

    Ffprintf(stderr, "XFlush()\n");

    nocache = sync_all_calls;
    flip = 1;
    trans = 0;

    m_initialized = 1;
}

/* Added X display name support.  WES 7/17/91 */

void m_set_display_name(display_name) char *display_name;
{
    Mfprintf(stderr, "m_set_display_name(\"%s\")\n", display_name);

    if (strlen(display_name) >= DISPLAY_NAME_LENGTH)
        strncpy(m_display_name, display_name, DISPLAY_NAME_LENGTH);
    else
        strcpy(m_display_name, display_name);
}

void m_init_screen() {
    Mfprintf(stderr, "m_init_screen()\n");

    do_init_screen(0);
}

void m_init_graphics() {
    Mfprintf(stderr, "m_init_graphics()\n");

    do_init_screen(1);
    m_choosecolors(0);
}

#ifndef ENABLE_DEBUGGING
#define nocache 0
#endif

void m_init_graphics_nopen() {
    Mfprintf(stderr, "m_init_graphics_nopen()\n");
    do_init_screen(1);
}

void m_init_dzg() {
    Mfprintf(stderr, "m_init_dzg()\n");

    m_init_graphics();
    flip = 0;
}

void m_init_colors() {
    Mfprintf(stderr, "m_init_colors()\n");

    m_choosecolors(0);
}

void m_init_pen(hpib_address) int hpib_address;
{ Mfprintf(stderr, "m_init_pen(%d)\n", hpib_address); }

void m_version(version) int version;
{
    Mfprintf(stderr, "m_version(%d)\n", version);

    if (version != 0)
        fprintf(stderr, "Mylib:  m_version(%d) not supported\n", version);
}

long m_curversion() {
    Mfprintf(stderr, "m_curversion()\n");

    return (0);
}

void m_modern(flag) int flag;
{
    Mfprintf(stderr, "m_modern(%d)\n", flag);

    if (flag)
        m_version(1);
    else
        m_version(0);
}

/*****************************************************************************/
/*            These are internal routines to do better buffering             */

#define BUF_SIZE 1024

static SDL_Point pointbuf[16][BUF_SIZE];
static int pointbuf_size[16];

void set_color(int color) {
    SDL_SetRenderDrawColor(m_renderer, cols[color] & 255,
                           (cols[color] >> 8) & 255, (cols[color] >> 16) & 255,
                           SDL_ALPHA_OPAQUE);
}
static void buffer_point(color, x, y) int color, x, y;
{
    pointbuf[color][pointbuf_size[color]].x = x;
    pointbuf[color][pointbuf_size[color]].y = y;
    if (++pointbuf_size[color] == BUF_SIZE) {
        set_color(color);
        SDL_RenderDrawPoints(m_renderer, pointbuf[color], BUF_SIZE);
        //    XDrawPoints(m_display, m_window, gc[color],
        //		pointbuf[color], BUF_SIZE, CoordModeOrigin);
        pointbuf_size[color] = 0;
    }
}

static void flush_points() {
    int color;

    for (color = 0; color < 16; color++) {
        if (pointbuf_size[color]) {
            set_color(color);
            SDL_RenderDrawPoints(m_renderer, pointbuf[color],
                                 pointbuf_size[color]);
            //  XDrawPoints(m_display, m_window, gc[color],
            //  pointbuf[color], pointbuf_size[color], CoordModeOrigin);
            //  pointbuf_size[color] = 0;
        }
    }
}

static SDL_Point linebuf[16][BUF_SIZE][2];
static int linebuf_size[16];

static void buffer_line(color, x1, y1, x2, y2) int color, x1, y1, x2, y2;
{
    linebuf[color][linebuf_size[color]][0].x = x1;
    linebuf[color][linebuf_size[color]][0].y = y1;
    linebuf[color][linebuf_size[color]][1].x = x2;
    linebuf[color][linebuf_size[color]][1].y = y2;
    if (++linebuf_size[color] == BUF_SIZE) {
        set_color(color);
        for (int i = 0; i < BUF_SIZE; i++)
            SDL_RenderDrawLine(m_renderer, linebuf[color][i][0].x,
                               linebuf[color][i][0].y, linebuf[color][i][1].x,
                               linebuf[color][i][1].y);
        // XDrawSegments(m_display, m_window, gc[color], linebuf[color],
        // BUF_SIZE);
        linebuf_size[color] = 0;
    }
}

static void flush_lines() {
    int color;

    for (color = 0; color < 16; color++) {
        if (linebuf_size[color]) {
            /*
            XDrawSegments(m_display, m_window, gc[color],
                          linebuf[color], linebuf_size[color]);
              */
            set_color(color);
            for (int i = 0; i < linebuf_size[color]; i++)
                SDL_RenderDrawLine(
                    m_renderer, linebuf[color][i][0].x, linebuf[color][i][0].y,
                    linebuf[color][i][1].x, linebuf[color][i][1].y);

            linebuf_size[color] = 0;
        }
    }
}

static SDL_Rect rectbuf[16][BUF_SIZE];
static int rectbuf_size[16];

static void buffer_rect(color, x, y, width, height) int color, x, y, width,
    height;
{
    rectbuf[color][rectbuf_size[color]].x = x;
    rectbuf[color][rectbuf_size[color]].y = y;
    rectbuf[color][rectbuf_size[color]].w = width;
    rectbuf[color][rectbuf_size[color]].h = height;
    if (++rectbuf_size[color] == BUF_SIZE) {
        set_color(color);
        SDL_RenderDrawRects(m_renderer, rectbuf[color], BUF_SIZE);
        //  XDrawRectangles(m_display, m_window, gc[color], rectbuf[color],
        //  BUF_SIZE);
        rectbuf_size[color] = 0;
    }
}

static void flush_rects() {
    int color;

    for (color = 0; color < 16; color++) {
        if (rectbuf_size[color]) {
            set_color(color);
            SDL_RenderDrawRects(m_renderer, rectbuf[color],
                                rectbuf_size[color]);
            // XDrawRectangles(m_display, m_window, gc[color],
            //     rectbuf[color], rectbuf_size[color]);
            rectbuf_size[color] = 0;
        }
    }
}

static SDL_Rect fillrectbuf[16][BUF_SIZE];
static int fillrectbuf_size[16];

static void buffer_fillrect(color, x, y, width, height) int color, x, y, width,
    height;
{
    fillrectbuf[color][fillrectbuf_size[color]].x = x;
    fillrectbuf[color][fillrectbuf_size[color]].y = y;
    fillrectbuf[color][fillrectbuf_size[color]].w = width;
    fillrectbuf[color][fillrectbuf_size[color]].h = height;
    if (++fillrectbuf_size[color] == BUF_SIZE) {
        set_color(color);
        SDL_RenderFillRects(m_renderer, rectbuf[color], BUF_SIZE);
        fillrectbuf_size[color] = 0;
    }
}

static void flush_fillrects() {
    int color;

    for (color = 0; color < 16; color++) {
        if (fillrectbuf_size[color]) {
            set_color(color);
            SDL_RenderFillRects(m_renderer, rectbuf[color],
                                fillrectbuf_size[color]);
            fillrectbuf_size[color] = 0;
        }
    }
}

static void flush_buffers() {
    flush_points();
    flush_lines();
    flush_rects();
    flush_fillrects();
}

/*                                                                           */
/*****************************************************************************/

void m_clear() {
    Mfprintf(stderr, "m_clear()\n");

#ifdef EXTRA_BUFFERING
    flush_buffers();
#endif /* EXTRA_BUFFERING */
    Xfprintf(stderr, "XClearWindow()\n");
    // XClearWindow(m_display, m_window);
    set_color(0);
    SDL_RenderClear(m_renderer);

    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        //   XFlush(m_display);
    }
}

void m_clearwindow(from, lines) int from, lines;
{ Mfprintf(stderr, "m_clearwindow(%d, %d)\n", from, lines); }

void m_vsync() { Mfprintf(stderr, "m_vsync()\n"); }

int m_clip_x1, m_clip_y1, m_clip_x2, m_clip_y2;

void m_clip(x1, y1, x2, y2) int x1, y1, x2, y2;
{
    SDL_Rect rect;
    int i;

    Mfprintf(stderr, "m_clip(%d, %d, %d, %d)\n", x1, y1, x2, y2);

    if (x2 == 32767) {
        m_noclip();
        return;
    }

    LTRNSFRM(x1, y1);
    LTRNSFRM(x2, y2);

    if (x1 > x2) i = x1, x1 = x2, x2 = i;
    if (y1 > y2) i = y1, y1 = y2, y2 = i;

    if ((m_clip_x1 != x1) || (m_clip_y1 != y1) || (m_clip_x2 != x2) ||
        (m_clip_y2 != y2)) {
#ifdef EXTRA_BUFFERING
        flush_buffers();
#endif /* EXTRA_BUFFERING */
        rect.x = x1;
        rect.y = y1;
        rect.w = x2 - x1;
        rect.h = y2 - y1;

        SDL_RenderSetClipRect(m_renderer, &rect);
        m_clip_x1 = x1;
        m_clip_y1 = y1;
        m_clip_x2 = x2;
        m_clip_y2 = y2;
    }
}

void m_noclip() {
    int i;

    Mfprintf(stderr, "m_noclip()\n");

    if ((m_clip_x1 != 0) || (m_clip_y1 != 0) || (m_clip_x2 != 32767) ||
        (m_clip_y2 != 32767)) {
#ifdef EXTRA_BUFFERING
        flush_buffers();
#endif /* EXTRA_BUFFERING */

        SDL_RenderSetClipRect(m_renderer, NULL);
        m_clip_x1 = 0;
        m_clip_y1 = 0;
        m_clip_x2 = 32767;
        m_clip_y2 = 32767;
    }
}

void m_transform(xx, yx, xy, yy, d, ax, ay) int xx, yx, xy, yy, d, ax, ay;
{
    flip = 0;
    trans = 1;

    Mfprintf(stderr, "m_transform(%d, %d, %d, %d, %d, %d, %d)\n", xx, yx, xy,
             yy, d, ax, ay);

    trans_XtoX = xx;
    trans_XtoY = xy;
    trans_YtoY = yy;
    trans_YtoX = yx;
    trans_denom = d;
    trans_addx = ax;
    trans_addy = ay;
}

void m_notransform() {
    Mfprintf(stderr, "m_notransform()\n");

    flip = 1;
    trans = 0;
}

void m_upside_down() {
    Mfprintf(stderr, "m_upside_down()\n");

    flip = 0;
    trans = 0;
}

void m_rotscale(rot, scale, ax, ay) double rot, scale;
int ax, ay;
{
    int s, c;

    Mfprintf(stderr, "m_rotscale(%f, %f, %d, %d)\n", rot, scale, ax, ay);

    s = (int)(sin(rot) * scale * 256);
    c = (int)(cos(rot) * scale * 256);

    m_transform(c, -s, -s, -c, 256, ax, m_down - ay);
}

void m_rotscaled(rot, scale, ax, ay) double rot, scale;
int ax, ay;
{
    Mfprintf(stderr, "m_rotscaled(%f, %f, %d, %d)\n", rot, scale, ax, ay);

    m_rotscale(rot * dr, scale, ax, ay);
}

static int cursor_is_on = 0;
static int curcursor = 1000000;
static int cursx, cursy;

static void turncursoroff() {
    Mfprintf(stderr, "turncursoroff()\n");
    /*  fprintf(stderr, "turncursoroff() at (%d,%d)\n", cursx, cursy);  */

    if (cursor_is_on) {
        Xfprintf(stderr, "XCopyArea()   (turncursoroff)\n");
    }
}

static void turncursoron() {
    Mfprintf(stderr, "turncursoron() at (%d,%d)\n", cursx, cursy);

    if (cursor_is_on) {
        Xfprintf(stderr, "XCopyArea()   (turncursoron)\n");
    }
}

void m_nocursor() {
    Mfprintf(stderr, "m_nocursor()\n");

    if (cursor_is_on) {
        turncursoroff();
        cursor_is_on = 0;
        /*    fprintf(stderr, "XDefineCursor() (%d)\n", curcursor);  */
        Xfprintf(stderr, "XDefineCursor()  (m_nocursor)\n");
        // XDefineCursor(m_display, m_window, cursors[curcursor].sub);
    }
}

static m_tablet_info mouse;

void m_cursor(x, y) int x, y;
{
    Mfprintf(stderr, "m_cursor(%d, %d)\n", x, y);
    /*  fprintf(stderr, "m_cursor(%d, %d)\n", x, y);  */

    TRNSFRM(x, y);

    if (x != cursx || y != cursy || !cursor_is_on) {
        turncursoroff();
        cursx = x;
        cursy = y;
        Xfprintf(stderr, "XCopyArea()  (m_cursor)\n");
    }
}

void m_choosecursor(n) int n;
{
    // SDL_Cursor newcursor;

    Mfprintf(stderr, "m_choosecursor(%d)\n", n);

    if ((n >= 0) && (n <= 3) && (n != curcursor)) {
        if (cursor_is_on) {
            m_nocursor();
            curcursor = n;
            /*      fprintf(stderr, "m_cursor(%d, %d)   from m_choosecursor\n",
                          mouse.x, mouse.y);  */
            m_cursor(mouse.x, mouse.y);
        } else {
            curcursor = n;
            /*      fprintf(stderr, "XDefineCursor() (%d)\n", n);  */
            Xfprintf(stderr, "XDefineCursor()  (m_choosecursor)\n");
            // XDefineCursor(m_display, m_window, cursors[n].sub);
        }
    }
    /*
        newcursor = XCreateFontCursor(m_display, xcursors[n].cursor);
        XRecolorCursor(m_display, newcursor, &m_colors[0][xcursors[n].color],
                                             &m_colors[0][m_black]);

        XDefineCursor(m_display, m_window, newcursor);
      }
    */
    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        //  XFlush(m_display);
    }
    SDL_SetCursor(cursors[n]);
    SDL_ShowCursor(1);
}

void m_colormode(c) int c;
{
    int i;

    SDL_BlendMode invmode = SDL_ComposeCustomBlendMode(
        SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR, SDL_BLENDFACTOR_ZERO,
        SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_DST_ALPHA,
        SDL_BLENDOPERATION_ADD);
    /*
             SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ZERO,
SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,SDL_BLENDOPERATION_SUBTRACT,
SDL_BLENDFACTOR_ZERO,
SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,SDL_BLENDOPERATION_ADD);
*/

    Mfprintf(stderr, "m_colormode(%d)\n", c);
    currentmode = c;
    switch (c) {
        case m_xor:
            // currentcolor |= GrXOR;
            // #ifndef __EMSCRIPTEN__
            // glEnable (GL_COLOR_LOGIC_OP) ;
            // glLogicOp(GL_XOR);
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
            // glEnable (GL_BLEND) ;
            // glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO );
            // #else

            // #endif
            // m_clear();
            // refresh();
            SDL_SetRenderDrawBlendMode(m_renderer, invmode);
            // SDL_SetRenderDrawColor(m_renderer, 128, 128, 128, 128);
            // currentcolor=0xffffffff;
            // SetROP2(hdc,R2_XORPEN);
            break;
        // case m_over:
        // currentcolor |= GrOR;
        // break;
        case m_erase:
            currentcolor = cols[0];
            break;

        case m_normal:
            // SetROP2(hdc,R2_COPYPEN);
            // glLogicOp(GL_COPY);

            // #ifndef __EMSCRIPTEN__
            // glDisable (GL_COLOR_LOGIC_OP) ;
            // glDisable (GL_BLEND) ;
            // #endif
            SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);
            // currentcolor &= 0xffffff;
            break;
    }
}

void m_color(newcolor) int newcolor;
{
    Mfprintf(stderr, "m_color(%d)\n", newcolor);
    if (newcolor > m_maxcolor)
        newcolor = m_maxcolor;
    else if (newcolor < 0)
        newcolor = 0;
    if (currentmode == m_erase) currentcolor = 0;
    if (currentmode == m_xor)
        currentcolor = -1;
    else
        currentcolor = (currentcolor & 0xff000000) | cols[newcolor];
    currentcolorindex = newcolor;
    // colindex=newcolor;
    // if(newcolor<16) {
    //  SelectObject(hdc,pens[newcolor]);
    SDL_SetRenderDrawColor(m_renderer, (currentcolor) & 255,
                           (currentcolor >> 8) & 255,
                           (currentcolor >> 16) & 255, 255);
}

long m_curcolor() {
    Mfprintf(stderr, "m_curcolor() = %d\n", currentcolorindex);

    return (currentcolorindex);
}

long m_curcolormode() {
    Mfprintf(stderr, "m_curcolormode() = %d\n", currentmode);

    return (currentmode);
}

void m_setcolor(c, r, g, b) int c, r, g, b;
{
    Mfprintf(stderr, "m_setcolor(%d, %d, %d, %d)\n", c, r, g, b);
    if (r > 0) r = r * 16;
    if (g > 0) g = g * 16;
    if (b > 0) b = b * 16;
    if (r == 0xf0) r = 255;
    if (g == 0xf0) g = 255;
    if (b == 0xf0) b = 255;

    cols[c] = 0xff000000 | (b << 16) | (g << 8) | r;
}

void m_seecolor(c, r, g, b) int c, *r, *g, *b;
{
    Mfprintf(stderr, "m_seecolor(%d)\n", c);

    if (c >= 0 && c <= m_maxcolor) {
        *r = m_colors[ColorSets][c].r / 4369;
        *g = m_colors[ColorSets][c].g / 4369;
        *b = m_colors[ColorSets][c].b / 4369;
    } else
        *r = *g = *b = 0;
}

void m_setcolors(r, g, b) m_colorarray r, g, b;
{
    int i;
    unsigned char *d;
    for (i = 0; i <= m_maxcolor; i++) {
        m_colors[ColorSets][i].r = r[i + 1] * 4369;
        m_colors[ColorSets][i].g = g[i + 1] * 4369;
        m_colors[ColorSets][i].b = b[i + 1] * 4369;
    }
}

void m_seecolors(r, g, b) m_colorarray r, g, b;
{
    int i;

    Mfprintf(stderr, "m_seecolors(r, g, b)\n");

    for (i = 0; i <= m_maxcolor; i++) {
        r[i + 1] = m_colors[ColorSets][i].r / 4369;
        g[i + 1] = m_colors[ColorSets][i].g / 4369;
        b[i + 1] = m_colors[ColorSets][i].b / 4369;
    }
}

void m_vsetcolors(first, num, r, g, b) int first, num;
m_vcolorarray r, g, b;
{
    int i;
    unsigned char *d;

#ifdef HIRES
    unsigned long pixelvalue;
#endif /* HIRES */

    Mfprintf(stderr, "m_vsetcolors(%d, %d, r, g, b)\n", first, num);

    if (first + num > m_maxcolor) num = m_maxcolor - first + 1;
    if (num < 0) return;
    for (i = first; i <= m_maxcolor && i < first + num; i++) {
        m_colors[ColorSets][i].r = r[i - first];
        m_colors[ColorSets][i].g = g[i - first];
        m_colors[ColorSets][i].b = b[i - first];
    }
}

void m_vseecolors(first, num, r, g, b) int first, num;
m_colorarray r, g, b;
{
    int i;

    Mfprintf(stderr, "m_vseecolors(%d, %d, r, g, b)\n", first, num);

    for (i = first; i < first + num; i++) {
        r[i - first] = m_colors[ColorSets][i].r / 257;
        g[i - first] = m_colors[ColorSets][i].g / 257;
        b[i - first] = m_colors[ColorSets][i].b / 257;
    }
}

static int linestyles[16] = {
    0xffff, 0x8000, 0x8080, 0x8888, 0xff00, 0xf0f0, 0xcccc, 0xaaaa,
    0xfafa, 0,      0,      0,      0,      0,      0,      0,
};
static int currentlinestyle = 0;
static int linewidth = 0;

void m_linestyle(s) int s;
{ currentlinestyle = s; }

void m_nolinestyle() { currentlinestyle = 0; }

long m_curlinestyle() {
    Mfprintf(stderr, "m_curlinestyle()\n");

    return (currentlinestyle);
}

void m_setlinestyle(s, mask) int s, mask;
{
    Mfprintf(stderr, "m_setlinestyle(%d, %d)\n", s, mask);

    if (s >= 0 && s < 16) {
        linestyles[s] = mask;
        if (s == currentlinestyle) {
            currentlinestyle = -1;
            m_linestyle(s);
        }
    } else
        fprintf(
            stderr,
            "mylib:  invalid linestyle number (%d) passed to m_setlinestyle",
            s);
}

void m_seelinestyle(s, mask) int s, *mask;
{
    Mfprintf(stderr, "m_seelinestyle(%d, mask)\n", s);

    *mask = linestyles[s];
}

void m_linewidth(w) int w;
{}

void m_nolinewidth() {}

static int curx = 0, cury = 0;

void m_move(x, y) int x, y;
{
    Mfprintf(stderr, "m_move(%d, %d)\n", x, y);

    TRNSFRM(x, y);

    curx = x;
    cury = y;
}

void m_moverel(dx, dy) int dx, dy;
{
    Mfprintf(stderr, "m_moverel(%d, %d)\n", dx, dy);

    DTRNSFRM(dx, dy);

    curx += dx;
    cury += dy;
}

int hitdet_line(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
    int oc1 = 0, oc2 = 0;
    int val;
    static int table[9][9] = {0, 0, 0, 0, 1, 2, 0, 4, 4, 0, 0, 0, 4, 1, 5, 4, 1,
                              5, 0, 0, 0, 2, 1, 0, 5, 5, 0, 0, 2, 2, 0, 1, 1, 0,
                              4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 0, 1, 1, 0,
                              5, 5, 0, 0, 4, 4, 0, 1, 4, 0, 0, 0, 4, 1, 5, 4, 1,
                              5, 0, 0, 0, 5, 5, 0, 5, 1, 0, 0, 0, 0};

    if (x1 > m_clip_x2)
        oc1 += 2;
    else if (x1 >= m_clip_x1)
        oc1 += 1;
    if (y1 > m_clip_y2)
        oc1 += 6;
    else if (y1 >= m_clip_y1)
        oc1 += 3;
    if (x2 > m_clip_x2)
        oc2 += 2;
    else if (x2 >= m_clip_x1)
        oc2 += 1;
    if (y2 > m_clip_y2)
        oc2 += 6;
    else if (y2 >= m_clip_y1)
        oc2 += 3;
    switch (table[oc1][oc2]) {
        case 0: /* trivial reject */
            return 0;

        case 1: /* trivial accept */
            return 1;

        case 2: /* crosses m_clip_y1 */
            val = x1 + (x2 - x1) * (m_clip_y1 - y1) / (y2 - y1);
            if (y1 < y2)
                return (hitdet_line(x1, y1, val, m_clip_y1 - 1) ||
                        hitdet_line(val, m_clip_y1, x2, y2));
            else
                return (hitdet_line(x2, y2, val, m_clip_y1 - 1) ||
                        hitdet_line(val, m_clip_y1, x1, y1));

        case 4: /* crosses m_clip_x1 */
            val = y1 + (y2 - y1) * (m_clip_x1 - x1) / (x2 - x1);
            if (x1 < x2)
                return (hitdet_line(x1, y1, m_clip_x1 - 1, val) ||
                        hitdet_line(m_clip_x1, val, x2, y2));
            else
                return (hitdet_line(x2, y2, m_clip_x1 - 1, val) ||
                        hitdet_line(m_clip_x1, val, x1, y1));

        case 5: /* crosses m_clip_x2 */
            val = y1 + (y2 - y1) * (m_clip_x2 - x1) / (x2 - x1);
            if (x1 < x2)
                return (hitdet_line(x1, y1, m_clip_x2, val) ||
                        hitdet_line(m_clip_x2 + 1, val, x2, y2));
            else
                return (hitdet_line(x2, y2, m_clip_x2, val) ||
                        hitdet_line(m_clip_x2 + 1, val, x1, y1));
    }
    return 0; /* should never occur */
}

void m_draw(x, y) int x, y;
{
    Mfprintf(stderr, "m_draw(%d, %d)\n", x, y);

    TRNSFRM(x, y);

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_line(curx, cury, x, y);
        curx = x;
        cury = y;
        return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

#ifdef EXTRA_BUFFERING
    buffer_line(currentcolor, curx, cury, x, y);
#else
    Xfprintf(stderr, "XDrawLine()\n");
    // XDrawLine(m_display, m_window, gc[currentcolor], curx, cury, x, y);
    // set_color(currentcolorindex);
    SDL_RenderDrawLine(m_renderer, curx, cury, x, y);
#endif /* EXTRA_BUFFERING */
    curx = x;
    cury = y;

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif

    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        // XFlush(m_display);
    }
}

void m_drawrel(dx, dy) int dx, dy;
{
    Mfprintf(stderr, "m_drawrel(%d, %d)\n", dx, dy);

    DTRNSFRM(dx, dy);

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_line(curx, cury, curx + dx, cury + dy);
        curx += dx;
        cury += dy;
        return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

#ifdef EXTRA_BUFFERING
    buffer_line(currentcolor, curx, cury, curx + dx, cury + dy);
#else
    Xfprintf(stderr, "XDrawLine()\n");
    // XDrawLine(m_display, m_window, gc[currentcolor], curx, cury, curx+dx,
    // cury+dy);
    // set_color(currentcolor);
    SDL_RenderDrawLine(m_renderer, curx, cury, curx + dx, cury + dy);
#endif /* EXTRA_BUFFERING */
    curx += dx;
    cury += dy;

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif

    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        // XFlush(m_display);
    }
}

void m_move2(x, y) int x, y;
{
    Mfprintf(stderr, "m_move2(%d, %d)\n", x, y);

    TRNSFRM(x, y);

    curx = x;
    cury = y;
}

void m_moverel2(dx, dy) int dx, dy;
{
    Mfprintf(stderr, "m_moverel2(%d, %d)\n", dx, dy);

    DTRNSFRM(dx, dy);

    curx += dx;
    cury += dy;
}

void m_seeposn(x, y) int *x, *y;
{
    int tx, ty;

    Mfprintf(stderr, "m_seeposn(x, y)\n");

    tx = curx;
    ty = cury;

    UNTRNSFRM(tx, ty);

    *x = curx;
    *y = cury;
}

void m_drawline(x1, y1, x2, y2) int x1, y1, x2, y2;
{
    Mfprintf(stderr, "m_drawline(%d, %d, %d, %d)\n", x1, y1, x2, y2);

    TRNSFRM(x1, y1);
    TRNSFRM(x2, y2);

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_line(x1, y1, x2, y2);
        return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

#ifdef EXTRA_BUFFERING
    if ((x1 == x2) && (y1 == y2))
        buffer_point(currentcolor, x1, y1);
    else
        buffer_line(currentcolor, x1, y1, x2, y2);
#else
    Xfprintf(stderr, "XDrawLine()\n");
    // set_color(currentcolor);
    if ((x1 == x2) && (y1 == y2))
        //    XDrawPoint(m_display, m_window, gc[currentcolor], x1, y1);
        SDL_RenderDrawPoint(m_renderer, x1, y1);

    else
        //    XDrawLine(m_display, m_window, gc[currentcolor], x1, y1, x2, y2);
        SDL_RenderDrawLine(m_renderer, x1, y1, x2, y2);
#endif /* EXTRA_BUFFERING */

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif
    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        // XFlush(m_display);
    }
}

int hitdet_drawrect(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
    if (((x1 >= m_clip_x1 && x1 <= m_clip_x2) ||
         (x2 >= m_clip_x1 && x2 <= m_clip_x2)) &&
        (y1 >= m_clip_y1 || y2 >= m_clip_y1) &&
        (y1 <= m_clip_y2 || y2 <= m_clip_y2))
        return 1;
    if (((y1 >= m_clip_y1 && y1 <= m_clip_y2) ||
         (y2 >= m_clip_y1 && y2 <= m_clip_y2)) &&
        (x1 >= m_clip_x1 || x2 >= m_clip_x1) &&
        (x1 <= m_clip_x2 || x2 <= m_clip_x2))
        return 1;
    return 0;
}

void m_drawrect(x1, y1, x2, y2) int x1, y1, x2, y2;
{
    int x, y;

    Mfprintf(stderr, "m_drawrect(%d, %d, %d, %d)\n", x1, y1, x2, y2);

    LTRNSFRM(x1, y1);
    LTRNSFRM(x2, y2);

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_drawrect(x1, y1, x2, y2);
        return;
    }

    x = MIN(x1, x2);
    y = MIN(y1, y2);

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

#ifdef EXTRA_BUFFERING
    if (x1 == x2)
        if (y1 == y2)
            buffer_point(currentcolor, x, y);
        else
            buffer_line(currentcolor, x1, y1, x1, y2);
    else if (y1 == y2)
        buffer_line(currentcolor, x1, y1, x2, y1);
    else
        buffer_rect(currentcolor, x, y, x1 + x2 - x - x, y1 + y2 - y - y);
#else
    Xfprintf(stderr, "XDrawRectangle()\n");
    // set_color(currentcolor);
    if (x1 == x2)
        if (y1 == y2)
            SDL_RenderDrawPoint(m_renderer, x1, y1);
        else
            SDL_RenderDrawLine(m_renderer, x1, y1, x1, y2);
    else if (y1 == y2)
        SDL_RenderDrawLine(m_renderer, x1, y1, x2, y1);
    else {
        SDL_Rect r;
        r.x = x;
        r.y = y;
        r.w = x1 + x2 - x - x;
        r.h = y1 + y2 - y - y;
        SDL_RenderDrawRect(m_renderer, &r);
    }
#endif /* EXTRA_BUFFERING */

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif

    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        //    XFlush(m_display);
    }
}

int hitdet_fillrect(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
    return ((x1 >= m_clip_x1 || x2 >= m_clip_x1) &&
            (x1 <= m_clip_x2 || x2 <= m_clip_x2) &&
            (y1 >= m_clip_y1 || y2 >= m_clip_y1) &&
            (y1 <= m_clip_y2 || y2 <= m_clip_y2));
}

void m_fillrect(x1, y1, x2, y2) int x1, y1, x2, y2;
{
    int x, y;

    Mfprintf(stderr, "m_fillrect(%d, %d, %d, %d)\n", x1, y1, x2, y2);

    LTRNSFRM(x1, y1);
    LTRNSFRM(x2, y2);

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_fillrect(x1, y1, x2, y2);
        return;
    }

    x = MIN(x1, x2);
    y = MIN(y1, y2);

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

#ifdef EXTRA_BUFFERING
    if (x1 == x2)
        if (y1 == y2)
            buffer_point(currentcolor, x, y);
        else
            buffer_line(currentcolor, x1, y1, x1, y2);
    else if (y1 == y2)
        buffer_line(currentcolor, x1, y1, x2, y1);
    else
        buffer_fillrect(currentcolor, x, y, x1 + x2 - x - x + 1,
                        y1 + y2 - y - y + 1);
#else
    Xfprintf(stderr, "XFillRectangle()\n");
    //  set_color(currentcolor);
    if (x1 == x2)
        if (y1 == y2)
            SDL_RenderDrawPoint(m_renderer, x1, y1);
        else
            SDL_RenderDrawLine(m_renderer, x1, y1, x1, y2);
    else if (y1 == y2)
        SDL_RenderDrawLine(m_renderer, x1, y1, x2, y1);
    else {
        SDL_Rect r;
        r.x = x;
        r.y = y;
        r.w = x1 + x2 - x - x + 1;
        r.h = y1 + y2 - y - y + 1;
        SDL_RenderFillRect(m_renderer, &r);
    }
#endif /* EXTRA_BUFFERING */

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif

    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        //  XFlush(m_display);
    }
}

void m_grid(x1, y1, x2, y2, dx, dy, ax, ay) int x1, y1, x2, y2, dx, dy, ax, ay;
{
    int x, y, wid, hei, i, j;

    Mfprintf(stderr, "m_grid(%d, %d, %d, %d, %d, %d, %d, %d)\n", x1, y1, x2, y2,
             dx, dy, ax, ay);

    LTRNSFRM(x1, y1);
    LTRNSFRM(x2, y2);
    DTRNSFRM(dx, dy);
    TRNSFRM(ax, ay);
    for (i = x1; i < x2; i += dx)
        for (j = y1; j < y2; j += dy) {
            SDL_RenderDrawLine(m_renderer, i, j, i + 1, j + 1);
        }
}

int hitdet_point(x, y)
int x, y;
{
    return (x >= m_clip_x1 && x <= m_clip_x2 && y >= m_clip_y1 &&
            y <= m_clip_y2);
}

void m_drawpoint(x, y) int x, y;
{
    TRNSFRM(x, y);

    Mfprintf(stderr, "m_drawpoint()\n");

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_point(x, y);
        return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

#ifdef EXTRA_BUFFERING
    buffer_point(currentcolor, x, y);
#else
    Xfprintf(stderr, "XDrawPoint()\n");
    // set_color(currentcolor);
    SDL_RenderDrawPoint(m_renderer, x, y);
#endif /* EXTRA_BUFFERING */

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif

    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        //   XFlush(m_display);
    }
}

int hitdet_ellipse(x, y, rx, ry, filled)
int x, y, rx, ry, filled;
{
    int clp_x1 = m_clip_x1, clp_x2 = m_clip_x2;
    int clp_y1 = m_clip_y1, clp_y2 = m_clip_y2;
    int rsq, xx, yy;

    if (rx == 0 || ry == 0)
        return hitdet_drawrect(x - rx, y - ry, x + rx, y + ry);
    if (ry > rx) {
        clp_x1 = clp_x1 * ry / rx;
        clp_x2 = clp_x2 * ry / rx;
        x = x * ry / rx;
        rx = ry;
    } else if (rx > ry) {
        clp_y1 = clp_y1 * rx / ry;
        clp_y2 = clp_y2 * rx / ry;
        y = y * rx / ry;
    }
    clp_x1 -= x;
    clp_x2 -= x;
    clp_y1 -= y;
    clp_y2 -= y;
    rsq = rx * rx;
    if (clp_x1 > 0) {
        if (clp_y1 > 0) {
            if (clp_x1 * clp_x1 + clp_y1 * clp_y1 > rsq) return 0;
            return (filled || clp_x2 * clp_x2 + clp_y2 * clp_y2 >= rsq);
        } else if (clp_y2 >= 0) {
            if (clp_x1 > rx) return 0;
            yy = (-clp_y1 > clp_y2) ? clp_y1 : clp_y2;
            return (filled || clp_x2 * clp_x2 + yy * yy >= rsq);
        } else {
            if (clp_x1 * clp_x1 + clp_y2 * clp_y2 > rsq) return 0;
            return (filled || clp_x2 * clp_x2 + clp_y1 * clp_y1 >= rsq);
        }
    } else if (clp_x2 >= 0) {
        if (clp_y1 > 0) {
            if (clp_y1 > rx) return 0;
            xx = (-clp_x1 > clp_x2) ? clp_x1 : clp_x2;
            return (filled || clp_y2 * clp_y2 + xx * xx >= rsq);
        } else if (clp_y2 >= 0) {
            if (filled) return 1;
            xx = (-clp_x1 > clp_x2) ? clp_x1 : clp_x2;
            yy = (-clp_y1 > clp_y2) ? clp_y1 : clp_y2;
            return (xx * xx + yy * yy >= rsq);
        } else {
            if (-clp_y2 > rx) return 0;
            xx = (-clp_x1 > clp_x2) ? clp_x1 : clp_x2;
            return (filled || clp_y1 * clp_y1 + xx * xx >= rsq);
        }
    } else {
        if (clp_y1 > 0) {
            if (clp_x2 * clp_x2 + clp_y1 * clp_y1 > rsq) return 0;
            return (filled || clp_x1 * clp_x1 + clp_y2 * clp_y2 >= rsq);
        } else if (clp_y2 >= 0) {
            if (-clp_x2 > rx) return 0;
            yy = (-clp_y1 > clp_y2) ? clp_y1 : clp_y2;
            return (filled || clp_x1 * clp_x1 + yy * yy >= rsq);
        } else {
            if (clp_x2 * clp_x2 + clp_y2 * clp_y2 > rsq) return 0;
            return (filled || clp_x1 * clp_x1 + clp_y1 * clp_y1 >= rsq);
        }
    }
}

void m_circle(x, y, r) int x, y, r;
{
    Mfprintf(stderr, "m_circle(%d, %d, %d)\n", x, y, r);

    TRNSFRM(x, y);
    r = abs(r);

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_ellipse(x, y, r, r, 0);
        return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

    Xfprintf(stderr, "XDrawArc()\n");
    circleColor(m_renderer, x, y, r, currentcolor);

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif

    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        // XFlush(m_display);
    }
}

void m_ellipse(x, y, rx, ry, c) int x, y, rx, ry, c;
{
    Mfprintf(stderr, "m_ellipse(%d, %d, %d, %d, %d)\n", x, y, rx, ry, c);

    TRNSFRM(x, y);
    DTRNSFRM(rx, ry);
    rx = abs(rx);
    ry = abs(ry);

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_ellipse(x, y, rx, ry, (currentcolor != m_trans));
        return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif
    m_color(c);
    //  if (currentcolor != m_trans) {
    //   Xfprintf(stderr, "XFillArc()\n");
    // XFillArc(m_display, m_window, gc[currentcolor], x-rx, y-ry, rx*2, ry*2,
    // 0, 360*64);
    //   ellipseColor(m_renderer,x, y, rx, ry,ColorSets);
    // }

    // if (c != m_trans) {
    //   Xfprintf(stderr, "XDrawArc()\n");
    ellipseColor(m_renderer, x, y, rx, ry, currentcolor);
    // }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif
    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        //  XFlush(m_display);
    }
}

void m_ellipse2(x, y, rx, ry, c1, c2) int x, y, rx, ry, c1, c2;
{}

void m_drawarc(x, y, rx, ry, theta1, theta2, rotate, chord) long x, y, rx, ry;
double theta1, theta2, rotate;
long chord;
{
    double a, c1, s1, c2, s2, c3, s3, rc, rs, d, th1, th2, start, stop, rcc1,
        rcs1, rsc1, rss1, rcc2, rcs2, rsc2, rss2, temp;
    long q, quad1, quad2, chx, chy;
    boolean rotflag;

    Mfprintf(stderr, "m_drawarc(%ld, %ld, %ld, %ld, %f, %f, %f, %ld)\n", x, y,
             rx, ry, theta1, theta2, rotate, chord);

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

    if (rx < 0 && ry < 0) {
        temp = theta1;
        theta1 = theta2;
        theta2 = temp;
    }

    if (!(theta1 < theta2 && rx && ry)) return;
    if (rx < 0) rx = -rx;
    if (ry < 0) ry = -ry;

    if ((rotflag = (rotate != 0))) {
        if (rx == ry) {
            rotflag = 0;
            theta1 += rotate;
            theta2 += rotate;
        } else {
            rs = sin(rotate * dr);
            rc = cos(rotate * dr);
        }
    }
    if (theta2 - theta1 >= 360.0) {
        theta1 = 0.0;
        theta2 = 360.0;
        s1 = 0.0;
        c1 = 1.0;
        if (chord != m_extarc) {
            if (rotflag)
                m_move2(x + (long)floor(rx * rc + 0.5),
                        y + (long)floor(rx * rs + 0.5));
            else
                m_move2(x + rx, y);
        }
        chord = 0;
    } else {
        s1 = sin(theta1 * dr);
        c1 = cos(theta1 * dr);
        if (rotflag) {
            chx = x + (long)floor(rx * c1 * rc - ry * s1 * rs + 0.5);
            chy = y + (long)floor(ry * s1 * rc + rx * c1 * rs + 0.5);
        } else {
            chx = x + (long)floor(rx * c1 + 0.5);
            chy = y + (long)floor(ry * s1 + 0.5);
        }
        switch (chord) {
            case m_pie:
                m_move2(x, y);
                m_draw(chx, chy);
                break;
            case m_extarc:
                break;
            default:
                m_move(chx, chy);
        }
    }
    th1 = theta1 - floor(theta1 / 90.0) * 90.0;
    quad1 = (int)(theta1 / 90.0) & 3;
    th2 = theta2 - floor(theta2 / 90.0) * 90.0;
    quad2 = (int)(theta2 / 90.0) & 3;
    start = th1;
    if (quad2 < quad1 || (quad2 == quad1 && th2 <= th1)) quad2 += 4;
    if (quad2 == quad1 + 1 && th2 <= th1) {
        quad2 = quad1;
        th2 += 90.0;
    }
    if (quad1 < quad2) {
        stop = 90.0;
        d = (90.0 - start) / 2;
    }
    for (q = quad1; q <= quad2; q++) {
        if (q == quad2) {
            stop = th2;
            s2 = sin(theta2 * dr);
            c2 = cos(theta2 * dr);
            d = (stop - start) / 2;
        } else {
            s2 = sin((stop + q * 90) * dr);
            c2 = cos((stop + q * 90) * dr);
        }
        if (d > 1e-5) {
            if (d == 45.0)
                a = 0.5522847498;
            else {
                s3 = sin(d * dr);
                c3 = cos(d * dr);
                a = 4 * (1 - c3) / (3 * s3);
            }
            if (rotflag) {
                rcc1 = rc * c1;
                rcs1 = rc * s1;
                rsc1 = rs * c1;
                rss1 = rs * s1;
                rcc2 = rc * c2;
                rcs2 = rc * s2;
                rsc2 = rs * c2;
                rss2 = rs * s2;
                m_cbezier(x + (long)floor(rx * rcc1 - ry * rss1 + 0.5),
                          y + (long)floor(ry * rcs1 + rx * rsc1 + 0.5),
                          x + (long)floor(rx * (rcc1 - a * rcs1) -
                                          ry * (rss1 + a * rsc1) + 0.5),
                          y + (long)floor(ry * (rcs1 + a * rcc1) +
                                          rx * (rsc1 - a * rss1) + 0.5),
                          x + (long)floor(rx * (rcc2 + a * rcs2) +
                                          ry * (a * rsc2 - rss2) + 0.5),
                          y + (long)floor(ry * (rcs2 - a * rcc2) +
                                          rx * (rsc2 + a * rss2) + 0.5),
                          x + (long)floor(rx * rcc2 - ry * rss2 + 0.5),
                          y + (long)floor(ry * rcs2 + rx * rsc2 + 0.5), 1000L);
            } else
                m_cbezier(x + (long)floor(rx * c1 + 0.5),
                          y + (long)floor(ry * s1 + 0.5),
                          x + (long)floor(rx * (c1 - a * s1) + 0.5),
                          y + (long)floor(ry * (s1 + a * c1) + 0.5),
                          x + (long)floor(rx * (c2 + a * s2) + 0.5),
                          y + (long)floor(ry * (s2 - a * c2) + 0.5),
                          x + (long)floor(rx * c2 + 0.5),
                          y + (long)floor(ry * s2 + 0.5), 1000L);
        }
        if (q < quad2) {
            start = 0.0;
            d = 45.0;
            s1 = s2;
            c1 = c2;
        }
    }
    switch (chord) {
        case m_pie:
            m_draw(x, y);
            break;
        case m_chord:
            m_draw(chx, chy);
            break;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif
}

typedef long pts[506];

void m_fillarc(x, y, rx, ry, theta1, theta2, rotate, chord) long x, y, rx, ry;
double theta1, theta2, rotate;
long chord;
{
    pts xp, yp;
    double ss, cc, rs, rc, rsx, rsy, rcx, rcy, th, temp;
    long i, n;
    boolean rotflag, fullflag;

    Mfprintf(stderr, "m_fillarc(%ld, %ld, %ld, %ld, %f, %f, %f, %ld)\n", x, y,
             rx, ry, theta1, theta2, rotate, chord);

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

    if (rx < 0 && ry < 0) {
        temp = theta1;
        theta1 = theta2;
        theta2 = temp;
    }
    if (!(theta1 < theta2 && rx && ry)) return;
    if (rx < 0) rx = -rx;
    if (ry < 0) ry = -ry;
    rotflag = (rotate != 0);
    fullflag = (theta2 - theta1 >= 360.0);
    if (fullflag && !rotflag) {
        m_ellipse(x, y, rx, ry, m_trans);
        return;
    }
    if (fullflag) {
        theta1 = 0.0;
        theta2 = 360.0;
    }
    if (rotflag) {
        if (rx == ry) {
            rotflag = 0;
            theta1 += rotate;
            theta2 += rotate;
        } else {
            rs = sin(rotate * dr);
            rc = cos(rotate * dr);
            rcx = rc * rx;
            rcy = rc * ry;
            rsx = rs * rx;
            rsy = rs * ry;
        }
    }
    n = (long)floor((theta2 - theta1) * (rx + ry) / 2000 + 0.5) + 3;
    if (n > 500) n = 500;
    th = (theta2 - theta1) / n;
    if (fullflag) --n;
    for (i = 0; i <= n; i++) {
        if (i == n) {
            ss = sin(theta2 * dr);
            cc = cos(theta2 * dr);
        } else {
            ss = sin((theta1 + th * i) * dr);
            cc = cos((theta1 + th * i) * dr);
        }
        if (rotflag) {
            xp[i] = x + (long)floor(rcx * cc - rsy * ss + 0.5);
            yp[i] = y + (long)floor(rcy * ss + rsx * cc + 0.5);
        } else {
            xp[i] = x + (long)floor(rx * cc + 0.5);
            yp[i] = y + (long)floor(ry * ss + 0.5);
        }
    }
    if (chord == m_pie && !fullflag) {
        ++n;
        xp[n] = x;
        yp[n] = y;
    }
    m_fillpoly(n + 1, (int *)xp, (int *)yp);

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif
}

void m_roundrect(x1, y1, x2, y2, rx, ry, c) int x1, y1, x2, y2, rx, ry, c;
{}

void m_roundrect2(x1, y1, x2, y2, rx, ry, c1, c2) int x1, y1, x2, y2, rx, ry,
    c1, c2;
{}

static SDL_Point bezbuf[2048];
static int bezbufp;
static int bezthresh;

int hitdet_bezier(x1, y1, x2, y2, x3, y3, x4, y4)
int x1, y1, x2, y2, x3, y3, x4, y4;
{
    int minx, maxx, miny, maxy;

    minx = (x1 < x2) ? x1 : x2;
    if (x3 < minx) minx = x3;
    if (x4 < minx) minx = x4;
    maxx = (x1 > x2) ? x1 : x2;
    if (x3 > maxx) maxx = x3;
    if (x4 > maxx) maxx = x4;
    miny = (y1 < y2) ? y1 : y2;
    if (y3 < miny) miny = y3;
    if (y4 < miny) miny = y4;
    maxy = (y1 > y2) ? y1 : y2;
    if (y3 > maxy) maxy = y3;
    if (y4 > maxy) maxy = y4;
    if (maxx < m_clip_x1 || minx > m_clip_x2 || maxy < m_clip_y1 ||
        miny > m_clip_y2)
        return 0;
    if (minx >= m_clip_x1 && maxx <= m_clip_x2 && miny >= m_clip_y1 &&
        maxy <= m_clip_y2)
        return 1;
    if (x1 >= m_clip_x1 && x1 <= m_clip_x2 && y1 >= m_clip_y1 &&
        y1 <= m_clip_y2)
        return 1;
    if (x4 >= m_clip_x1 && x4 <= m_clip_x2 && y4 >= m_clip_y1 &&
        y4 <= m_clip_y2)
        return 1;
    if (maxx - minx < 2 || maxy - miny < 2)
        return hitdet_fillrect(minx, miny, maxx, maxy);
    return (
        hitdet_bezier(x1, y1, (x1 + x2) >> 1, (y1 + y2) >> 1,
                      (x1 + (x2 << 1) + x3) >> 2, (y1 + (y2 << 1) + y3) >> 2,
                      (x1 + 3 * x2 + 3 * x3 + x4) >> 3,
                      (y1 + 3 * y2 + 3 * y3 + y4) >> 3) ||
        hitdet_bezier((x4 + 3 * x3 + 3 * x2 + x1) >> 3,
                      (y4 + 3 * y3 + 3 * y2 + y1) >> 3,
                      (x4 + (x3 << 1) + x2) >> 2, (y4 + (y3 << 1) + y2) >> 2,
                      (x4 + x3) >> 1, (y4 + y3) >> 1, x4, y4));
}

void dobezier(x1, y1, x2, y2, x3, y3, x4, y4, fx, fy) int x1, y1, x2, y2, x3,
    y3, x4, y4, fx, fy;
{
    if ((abs((y4 - y3) * (x3 - x1) - (y3 - y1) * (x4 - x3)) < bezthresh) &&
        (abs((y4 - y2) * (x2 - x1) - (y2 - y1) * (x4 - x2)) < bezthresh)) {
        bezbuf[bezbufp].x = x4 >> 4;
        bezbuf[bezbufp++].y = y4 >> 4;
        if (x4 == fx && y4 == fy) {
            Xfprintf(stderr, "XDrawLines()\n");
            //  XDrawLines(m_display, m_window, gc[currentcolor], bezbuf,
            //  bezbufp, CoordModeOrigin);
            // set_color(currentcolor);
            SDL_RenderDrawLines(m_renderer, bezbuf, bezbufp);
        }
    } else {
        dobezier(x1, y1, (x1 + x2) >> 1, (y1 + y2) >> 1,
                 (x1 + (x2 << 1) + x3) >> 2, (y1 + (y2 << 1) + y3) >> 2,
                 (x1 + 3 * x2 + 3 * x3 + x4) >> 3,
                 (y1 + 3 * y2 + 3 * y3 + y4) >> 3, fx, fy);
        dobezier((x4 + 3 * x3 + 3 * x2 + x1) >> 3,
                 (y4 + 3 * y3 + 3 * y2 + y1) >> 3, (x4 + (x3 << 1) + x2) >> 2,
                 (y4 + (y3 << 1) + y2) >> 2, (x4 + x3) >> 1, (y4 + y3) >> 1, x4,
                 y4, fx, fy);
    }
}

void m_bezier(x1, y1, x2, y2, x3, y3, x4, y4) int x1, y1, x2, y2, x3, y3, x4,
    y4;
{
    Mfprintf(stderr, "m_bezier(%d, %d, %d, %d, %d, %d, %d, %d)\n", x1, y1, x2,
             y2, x3, y3, x4, y4);

    TRNSFRM(x1, y1);
    TRNSFRM(x2, y2);
    TRNSFRM(x3, y3);
    TRNSFRM(x4, y4);

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_bezier(x1, y1, x2, y2, x3, y3, x4, y4);
        return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

    bezbuf[0].x = x1;
    bezbuf[0].y = y1;
    bezbufp = 1;
    bezthresh = 2500;
    dobezier((x1 << 4) + 8, (y1 << 4) + 8, (x2 << 4) + 8, (y2 << 4) + 8,
             (x3 << 4) + 8, (y3 << 4) + 8, (x4 << 4) + 8, (y4 << 4) + 8,
             (x4 << 4) + 8, (y4 << 4) + 8);

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif
    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        // XFlush(m_display);
    }
}

void m_bezier2(x1, y1, x2, y2, x3, y3, x4, y4, thresh) int x1, y1, x2, y2, x3,
    y3, x4, y4, thresh;
{
    Mfprintf(stderr, "m_bezier2(%d, %d, %d, %d, %d, %d, %d, %d, %d)\n", x1, y1,
             x2, y2, x3, y3, x4, y4, thresh);

    TRNSFRM(x1, y1);
    TRNSFRM(x2, y2);
    TRNSFRM(x3, y3);
    TRNSFRM(x4, y4);

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_bezier(x1, y1, x2, y2, x3, y3, x4, y4);
        return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

    bezbuf[0].x = x1;
    bezbuf[0].y = y1;
    bezbufp = 1;
    bezthresh = thresh;
    dobezier((x1 << 4) + 8, (y1 << 4) + 8, (x2 << 4) + 8, (y2 << 4) + 8,
             (x3 << 4) + 8, (y3 << 4) + 8, (x4 << 4) + 8, (y4 << 4) + 8,
             (x4 << 4) + 8, (y4 << 4) + 8);

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif

    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        //  XFlush(m_display);
    }
}

void m_cbezier(x1, y1, x2, y2, x3, y3, x4, y4, thresh) int x1, y1, x2, y2, x3,
    y3, x4, y4, thresh;
{
    Mfprintf(stderr, "m_cbezier(%d, %d, %d, %d, %d, %d, %d, %d, %d)\n", x1, y1,
             x2, y2, x3, y3, x4, y4, thresh);

    TRNSFRM(x1, y1);
    TRNSFRM(x2, y2);
    TRNSFRM(x3, y3);
    TRNSFRM(x4, y4);

    if (currentmode == m_hitdet) {
        m_hitcount += (hitdet_line(curx, cury, x1, y1) ||
                       hitdet_bezier(x1, y1, x2, y2, x3, y3, x4, y4));
        return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

    bezbuf[0].x = curx;
    bezbuf[0].y = cury;
    bezbuf[1].x = x1;
    bezbuf[1].y = y1;

    bezbufp = 2;
    bezthresh = thresh;

    dobezier((x1 << 4) + 8, (y1 << 4) + 8, (x2 << 4) + 8, (y2 << 4) + 8,
             (x3 << 4) + 8, (y3 << 4) + 8, (x4 << 4) + 8, (y4 << 4) + 8,
             (x4 << 4) + 8, (y4 << 4) + 8);

    curx = x4;
    cury = y4;

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif

    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        //  XFlush(m_display);
    }
}

/*
  Polycurve stuff added by Adam Greenblatt, 3 March 1990.  Sigh.
*/

#define SCALE 256

/* Local variables for polycurve: */
struct LOC_polycurve {
    int meef;
};

static void polycurve_doit(x0, y0, x1, y1, x2, y2, x3, y3, x4, y4, x5, y5,
                           LINK_polycurve) long x0,
    y0, x1, y1, x2, y2, x3, y3, x4, y4, x5, y5;
struct LOC_polycurve *LINK_polycurve;
{
    if (((x4 < 0) ? -x4 : x4) + ((x5 < 0) ? -x5 : x5) + ((y4 < 0) ? -y4 : y4) +
            ((y5 < 0) ? -y5 : y5) <
        SCALE) {
        m_cbezier(x0 / SCALE, y0 / SCALE, (x0 + x1 / 3) / SCALE,
                  (y0 + y1 / 3) / SCALE, (x0 + (x1 * 2 + x2) / 3) / SCALE,
                  (y0 + (y1 * 2 + y2) / 3) / SCALE,
                  (x0 + x1 + x2 + x3 + x4 + x5) / SCALE,
                  (y0 + y1 + y2 + y3 + y4 + y5) / SCALE, 2500L);
        return;
    }
    polycurve_doit(x0, y0, x1 / 2, y1 / 2, x2 / 4, y2 / 4, x3 / 8, y3 / 8,
                   x4 / 16, y4 / 16, x5 / 32, y5 / 32, LINK_polycurve);
    x1 *= 16;
    y1 *= 16;
    x2 *= 8;
    y2 *= 8;
    x3 *= 4;
    y3 *= 4;
    x4 *= 2;
    y4 *= 2;
    polycurve_doit(
        x0 + (x1 + x2 + x3 + x4 + x5) / 32, y0 + (y1 + y2 + y3 + y4 + y5) / 32,
        (x1 + x2 * 2 + x3 * 3 + x4 * 4 + x5 * 5) / 32,
        (y1 + y2 * 2 + y3 * 3 + y4 * 4 + y5 * 5) / 32,
        (x2 + x3 * 3 + x4 * 6 + x5 * 10) / 32,
        (y2 + y3 * 3 + y4 * 6 + y5 * 10) / 32, (x3 + x4 * 4 + x5 * 10) / 32,
        (y3 + y4 * 4 + y5 * 10) / 32, (x4 + x5 * 5) / 32, (y4 + y5 * 5) / 32,
        x5 / 32, y5 / 32, LINK_polycurve);
}

void m_polycurve(x0, y0, x1, y1, x2, y2, x3, y3, x4, y4, x5, y5) double x0, y0,
    x1, y1, x2, y2, x3, y3, x4, y4, x5, y5;
{
    struct LOC_polycurve V_polycurve;

    m_move((long)floor(SCALE * x0 + 0.5) / SCALE,
           (long)floor(SCALE * y0 + 0.5) / SCALE);
    polycurve_doit((long)floor(SCALE * x0 + 0.5), (long)floor(SCALE * y0 + 0.5),
                   (long)floor(SCALE * x1 + 0.5), (long)floor(SCALE * y1 + 0.5),
                   (long)floor(SCALE * x2 + 0.5), (long)floor(SCALE * y2 + 0.5),
                   (long)floor(SCALE * x3 + 0.5), (long)floor(SCALE * y3 + 0.5),
                   (long)floor(SCALE * x4 + 0.5), (long)floor(SCALE * y4 + 0.5),
                   (long)floor(SCALE * x5 + 0.5), (long)floor(SCALE * y5 + 0.5),
                   &V_polycurve);
}

int hitdet_drawpoly(n, points)
int n;
SDL_Point *points;
{
    int i;

    if (hitdet_line(points[0].x, points[0].y, points[n - 1].x, points[n - 1].y))
        return 1;
    for (i = 1; i < n; i++) {
        if (hitdet_line(points[i].x, points[i].y, points[i - 1].x,
                        points[i - 1].y)) {
            return 1;
        }
    }
    return 0;
}

void m_drawpoly(n, x, y) int n, x[], y[];
{}

/* If no edges intersect, then rectangle must be entirely inside or
   entirely outside polygon.  Thus, we can simply check if any one point
   in the rectangle is in the polygon. */

int hitdet_fillpoly(n, points)
int n;
SDL_Point *points;
{
    int i, y1, y2, count;
    SDL_Point *pt1, *pt2;

    if (hitdet_drawpoly(n, points)) return 1;
    count = 0;
    pt1 = points;
    pt2 = &points[n - 1];
    for (i = 0; i < n; i++) {
        y1 = MIN(pt1->y, pt2->y);
        y2 = MAX(pt1->y, pt2->y);
        if (y1 <= m_clip_y1 && y2 > m_clip_y1) {
            if ((pt2->x - pt1->x) * (m_clip_y1 - pt1->y) / (pt2->y - pt1->y) >
                (m_clip_x1 - pt1->x))
                count++;
        }
        pt2 = pt1;
        pt1++;
    }
    return (count & 1);
}

void m_fillpoly(n, x, y) int n, x[], y[];
{
    SDL_Point *pointlist;
    int i, j, newx, newy;

    Mfprintf(stderr, "m_fillpoly(%d, x, y)\n", n);

    pointlist = (SDL_Point *)calloc(n, sizeof(SDL_Point));

    for (i = 0; i < n; i++) {
        newx = x[i];
        newy = y[i];
        TRNSFRM(newx, newy);
        pointlist[i].x = (short)newx;
        pointlist[i].y = (short)newy;
    }

    if (currentmode == m_hitdet) {
        m_hitcount += hitdet_fillpoly(n, pointlist);
        free(pointlist);
        return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

    Xfprintf(stderr, "XFillPolygon()\n");
    // XFillPolygon(m_display, m_window, gc[currentcolor], pointlist, n,
    // Complex, CoordModeOrigin);
    free(pointlist);

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif
    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        // XFlush(m_display);
    }
}

void m_displaytext(str) char *str;
{
    int dir, desc, len;
    m_drawstr(curx, cury, "", str);
}
extern unsigned char *font6x10;
void m_drawstr(x, y, f, str) int x, y;
char *f, *str;
{
    int dir, desc, len;

    Mfprintf(stderr, "m_drawstr(%d, %d, f, %s)\n", x, y, str);

    TRNSFRM(x, y);

    len = strlen(str);

    if (currentmode == m_hitdet) {
        //  XTextExtents(currentfont, str, len, &dir, &fontasc, &desc, &size);
        //  m_hitcount += hitdet_fillrect(x, y,
        //			  x + size.width, y + fontasc + desc);
        //  return;
    }

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoroff();
#endif

    // Xfprintf(stderr, "XDrawString()\n");
    // XDrawString(m_display, m_window, gc[currentcolor], x, y+fontasc-1, str,
    // len);
    gfxPrimitivesSetFont(&font6x10, 6, 10);
    stringColor(m_renderer, x, y, str, currentcolor);

#ifdef SAVECURSOR
    if (cursor_is_on) turncursoron();
#endif

    if (nocache) {
        Ffprintf(stderr, "XFlush()\n");
        //   XFlush(m_display);
    }
}

void m_centerstr(x, y, f, str) int x, y;
char *f, *str;
{ m_drawstr(x - strlen(str) * 6 / 2, y, f, str); }

void m_rightstr(x, y, f, str) int x, y;
char *f, *str;
{ m_drawstr(x - strlen(str) * 6, y, f, str); }

long m_strwidth(f, str) /* daveg, 10/6/89 */
char *f, *str;
{ return strlen(str) * 6; }

/* Added these two functions for auto raise and lower support for X11.
 * The functions existed but were just #define'ed as nothing.
 * WES - 7/17/91
 */

/* This function raises the graphics window above the alpha window */

void m_graphics_on() {}

/* This function raises the alpha window above the graphics window */

void m_alpha_on() {}

SDL_Event event;
int nevents = 0;
int keybuf[256];
unsigned char keynext = 0, keyfirst = 0;
#define thekey (keyfirst != keynext) ? keybuf[keyfirst++] : 0;
int thex, they;
int relx, rely;
int theflags;
int thebuttons;

void addkey(int n) {
    if (keyfirst != (keynext + 1)) keybuf[keynext++] = n;
}

uint64_t time_ms() { return SDL_GetTicks64(); }
void resize_screen();
uint64_t lasttime = 0;
uint64_t lastpolltime = 0;
uint64_t keydowntime = 0;
uint64_t keyrepeattime = 0;
int keydown = 0;

void addsc(int sc) {
    if (sc == SDL_SCANCODE_LEFT) addkey('\b');
    if (sc == SDL_SCANCODE_RIGHT) addkey('\34');
    if (sc == SDL_SCANCODE_UP) addkey('\037');
    if (sc == SDL_SCANCODE_DOWN) addkey('\n');
    if (sc == SDL_SCANCODE_RETURN) addkey(13);
    if (sc == SDL_SCANCODE_DELETE) addkey(127);
    if (sc == SDL_SCANCODE_BACKSPACE) addkey(7);
}

void millisleep(int ms) { SDL_Delay(ms); }

void handle_events() {
    uint64_t time = time_ms();
    if (time - lasttime >= 18) {
        SDL_Rect r;
        SDL_RenderGetClipRect(m_renderer, &r);
        SDL_SetRenderTarget(m_renderer, NULL);
        SDL_RenderCopy(m_renderer, buffer, NULL, NULL);
        lasttime = time_ms();  // time;
        SDL_RenderPresent(m_renderer);
        SDL_SetRenderTarget(m_renderer, buffer);
        if (r.w != 0) SDL_RenderSetClipRect(m_renderer, &r);
    }
    int k, sc;
    if (time - lastpolltime >= 18) {
        lastpolltime = time;
        while (SDL_PollEvent(&event)) {
       //   fprintf(stderr,"SDL_EVENT %d \n",event.type);
            switch (event.type) {
                relx = 0;
                rely = 0;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        thebuttons |= 2;
                        theflags = 2;
                    }
                    if (event.button.button == SDL_BUTTON_RIGHT) {
                        thebuttons |= 1;
                        theflags = 1;
                    }
                    if (event.button.button == SDL_BUTTON_MIDDLE) {
                        thebuttons |= 4;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        thebuttons &= ~2;
                        theflags = 8;
                    }
                    if (event.button.button == SDL_BUTTON_RIGHT) {
                        thebuttons &= ~1;
                        theflags = 4;
                    }
                    if (event.button.button == SDL_BUTTON_MIDDLE) {
                        thebuttons &= ~4;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    theflags |= 16;
                    thex = event.motion.x;
                    they = event.motion.y;
                    relx = event.motion.xrel;
                    rely = event.motion.yrel;
                    break;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) addkey('<');
                    if (event.wheel.y < 0) addkey('>');
                    break;
                case SDL_KEYDOWN:
                    if (event.key.repeat != 0) break;
                    sc = event.key.keysym.scancode;
                    if (sc == SDL_SCANCODE_LEFT || sc == SDL_SCANCODE_RIGHT ||
                        sc == SDL_SCANCODE_UP || sc == SDL_SCANCODE_DOWN ||
                        sc == SDL_SCANCODE_RETURN) {
                        if (keydown == 0) keyrepeattime = time + 330;
                        keydown = sc;
                    }
                    addsc(sc);
                    break;
                case SDL_KEYUP:
                    keydown = 0;
                    keyrepeattime = time + 3300000;
                    break;
                case SDL_QUIT:
                    exit(0);
                    break;
                case SDL_TEXTINPUT:
                    k = event.text.text[0];
                    if (k == 8) k = 7;
                    addkey(k);
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) {
                      WindowWidth = event.window.data1;
                      WindowHeight = event.window.data2;
                      if (m_initialized && WindowWidth>0 && WindowHeight>0) {
                          SDL_DestroyTexture(buffer);
                          buffer = SDL_CreateTexture(
                              m_renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_TARGET, WindowWidth,
                              WindowHeight);
                          SDL_SetRenderTarget(m_renderer, buffer);
                          resize_screen();
                      }
                    }
                    break;
                default:
                    break;
            }
        }
        if (keydown && keyrepeattime <= time) {
            addsc(keydown);
            keyrepeattime = time + 33;
        }
    }
}
#define Button1Mask 2
#define Button3Mask 1
#define Button2Mask 4
#define GR_M_RIGHT_DOWN 1
#define GR_M_LEFT_DOWN 2
#define GR_M_RIGHT_UP 4
#define GR_M_LEFT_UP 8
#define GR_M_MOTION 16
#define GR_M_BUTTON_UP 12
#define GR_M_BUTTON_DOWN 3

void m_readpen(pen) m_tablet_info *pen;
{
    int gotevent, found = 0, giveup = 0;
    // printf("m_readpen\n");

    handle_events();

    pen->middle = (thebuttons & Button2Mask) != 0;
    Pfprintf(stderr, "m_readpen(pen) flags=%x, x=%d, y=%d\n", theflags, thex,
             they);
    pen->relx = 0;
    pen->rely = 0;
    if (theflags) {
        if (theflags & GR_M_BUTTON_DOWN) {
            pen->dn = (theflags & GR_M_LEFT_DOWN) != 0;
            pen->depressed = ((thebuttons & Button1Mask) != 0) || pen->dn;
            pen->up = 0;
            pen->near_ =
                !(thebuttons & Button3Mask) && !(theflags & GR_M_RIGHT_DOWN);

        } else if (theflags & GR_M_BUTTON_UP) {
            pen->dn = 0;
            pen->up = (theflags & GR_M_LEFT_UP) != 0;
            pen->depressed = ((thebuttons & Button1Mask) != 0) && (!pen->up);
            pen->near_ =
                !(thebuttons & Button3Mask) || (theflags & GR_M_RIGHT_UP);

        } else {
            pen->dn = 0;
            pen->up = 0;
            pen->depressed = ((thebuttons & Button1Mask) != 0);
            pen->near_ = !(thebuttons & Button3Mask);
        }

        if (theflags & GR_M_MOTION) {
            m_cursor(thex, they);
            pen->relx = relx;
            pen->rely = rely;
            relx = 0;
            rely = 0;
        } else {
            pen->middle = 0;
            pen->relx = 0;
            pen->rely = 0;
        }
        pen->x = thex;
        pen->y = they;
        pen->ax = (thex) / nc_fontwidth;
        pen->ay = (they) / nc_fontheight;

        // UNTRNSFRM(pen->x, pen->y);
    } else {
        pen->dn = pen->up = 0;
        pen->depressed = mouse.depressed;
        pen->near_ = mouse.near_;
        pen->x = mouse.x;
        pen->y = mouse.y;
        pen->ax = mouse.ax;
        pen->ay = mouse.ay;
    }

    theflags = 0;

    pen->moving = pen->x != mouse.x || pen->y != mouse.y ||
                  pen->depressed != mouse.depressed ||
                  pen->near_ != mouse.near_;
    pen->inalpha = mouse.inalpha;

    mouse.x = pen->x;
    mouse.y = pen->y;
    mouse.ax = pen->ax;
    mouse.ay = pen->ay;
    mouse.depressed = pen->depressed;
    mouse.near_ = pen->near_;
    mouse.relx = pen->relx;
    mouse.rely = pen->rely;
    mouse.middle = pen->middle;
}

void m_trackpen(pen) m_tablet_info *pen;
{
    Pfprintf(stderr, "m_trackpen(pen)\n");
    m_readpen(pen);
    /*  fprintf(stderr, "m_cursor(%d, %d)   from m_trackpen\n", pen->x, pen->y);
     */
    m_cursor(pen->x, pen->y);
}

void m_trackpen2(pen) m_tablet_info *pen;
{}

void m_waitpen(pen) m_tablet_info *pen;
{}

boolean m_pollkbd() {
    handle_events();
    return (keyfirst != keynext);
}

uchar m_inkey() {
    int k;
    nc_cursor_on();
    do {
        handle_events();
        k = thekey;
    } while (!k);
    nc_cursor_off();
    Kfprintf(stderr, "m_inkey %d\n", k);
    return k;
}

uchar m_inkeyn() {
    int k;
    Kfprintf(stderr, "m_inkeyn %d\n", keybuf[keyfirst]);
    k = thekey;
    return k;
}

uchar m_testkey() {
    Kfprintf(stderr, "m_testkey %d\n", keybuf[keyfirst]);
    if (keyfirst != keynext) return keybuf[keyfirst];
    return 0;
}

#define buttonw 40
#define buttonh 30
#define buttony 30
#define yesbuttonx 10
#define nobuttonx 60
#define popupw 110
#define popuph 70

boolean m_yes_or_no(prompt)
Char *prompt;
{ return true; }

/* The following added by daveg, 10/6/89 */

void m_makechar(cp, a, b, c, d, e, f) Anyptr *cp;
long a, b, c, d, e, f;
{}

void m_changechar(cp, a, b, c, d, e) Anyptr *cp;
long a, b, c, d, e;
{}

void m_drawchar(cp) Anyptr *cp;
{}
