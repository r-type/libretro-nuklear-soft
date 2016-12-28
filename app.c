/* nuklear - v1.00 - public domain */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include <libretro.h>
#include "libretro-core.h"

extern retro_log_printf_t log_cb;

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION

#define NK_RETRO_SOFT_IMPLEMENTATION

#include "nuklear.h"
#include "nuklear_retro_soft.h"

// RSDL surface (implementation from RSDL_wrapper)
static RSDL_Surface *screen_surface;
static nk_retro_Font *RSDL_font;

/* macros */

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

/* Platform */

float bg[4];
struct nk_color background;
/* GUI */
struct nk_context *ctx;

/* ===============================================================
 *
 *                          EXAMPLE
 *
 * ===============================================================*/


/* This are some code examples to provide a small overview of what can be
 * done with this library. To try out an example uncomment the include
 * and the corresponding function. */
//#define NO_EXAMPLE

#include "style.c"

#if !defined(EXAMPLE_CANVAS) && !defined(NO_EXAMPLE)
#include "node_editor.c"
#include "calculator.c"
#include "overview.c"
#endif

int app_init()
{
#ifdef M16B
    screen_surface=Retro_CreateRGBSurface16(rwidth,rheight,16,0,0,0,0);
#else
    screen_surface=Retro_CreateRGBSurface32(rwidth,rheight,32,0,0,0,0);
#endif

    retroscreen=screen_surface->pixels;

    RSDL_font = (nk_retro_Font*)calloc(1, sizeof(nk_retro_Font));
    RSDL_font->width = 8; 
    RSDL_font->height = 8; 
    if (!RSDL_font)
        return -1;

    /* GUI */
    ctx = nk_retro_init(RSDL_font,screen_surface,rwidth,rheight);

    /* style.c */
    /* THEME_BLACK THEME_WHITE THEME_RED THEME_BLUE THEME_DARK */
     set_style(ctx, THEME_DARK);

    /* icons */

#if !defined(EXAMPLE_CANVAS) && !defined(NO_EXAMPLE)
    background = nk_rgb(28,48,62);
#endif

	log_cb(RETRO_LOG_INFO, "Init nuklear %d\n",0);

 return 0;
}

int app_free()
{
//FIXME: memory leak here
    free(RSDL_font);
    nk_retro_shutdown();
    Retro_FreeSurface(screen_surface);

 return 0;
}

int app_event()
{
	int evt;

	nk_input_begin(ctx);
	nk_retro_handle_event(&evt,1);
	nk_input_end(ctx);

 return 0;
}

int app_main()
{
	/* -------------- EXAMPLES ---------------- */
	/* uncomment here and corresponding header  */
	/* to enable demo example		    */
#if !defined(EXAMPLE_CANVAS) && !defined(NO_EXAMPLE)
	calculator(ctx);
	overview(ctx);
	node_editor(ctx);
#endif
	/* ----------------------------------------- */

	/* Draw */
	// nk_color_fv(bg, background);
	nk_retro_render(nk_rgb(30,30,30));

    return 0;
}

int app_frame()
{
   	app_event();
   	app_main();

    return 0;
}

