/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_bmap.xc". */


#if defined(__GNUC__)
# define MAYBE_UNUSED __attribute__((unused))
#pragma GCC diagnostic ignored "-Wwrite-strings"
#else
# define MAYBE_UNUSED
#endif

#if defined(_MSC_VER)
# include "wx.h"
#endif
#if defined(OS_X)
# include "common.h"
#endif

#include "wx_gdi.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)




#define ESCAPE_NO_RET_VAL /*empty*/


#include "wxscheme.h"
#include "wxs_bmap.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif


#ifndef wx_mac
# define wxBITMAP_TYPE_PICT 101
#endif

#define wxBITMAP_TYPE_UNKNOWN 0
#define wxBITMAP_TYPE_GIF_MASK (wxBITMAP_TYPE_GIF | wxBITMAP_TYPE_MASK)
#define wxBITMAP_TYPE_PNG_MASK (wxBITMAP_TYPE_PNG | wxBITMAP_TYPE_MASK)

static Scheme_Object *bitmapType_wxBITMAP_TYPE_BMP_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_GIF_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_GIF_MASK_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_XBM_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_XPM_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_PICT_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_JPEG_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_PNG_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_PNG_MASK_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_UNKNOWN_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_MASK_sym = NULL;

static void init_symset_bitmapType(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_BMP_sym);
  bitmapType_wxBITMAP_TYPE_BMP_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("bmp"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_GIF_sym);
  bitmapType_wxBITMAP_TYPE_GIF_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("gif"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_GIF_MASK_sym);
  bitmapType_wxBITMAP_TYPE_GIF_MASK_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("gif/mask"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_XBM_sym);
  bitmapType_wxBITMAP_TYPE_XBM_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("xbm"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_XPM_sym);
  bitmapType_wxBITMAP_TYPE_XPM_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("xpm"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_PICT_sym);
  bitmapType_wxBITMAP_TYPE_PICT_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("pict"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_JPEG_sym);
  bitmapType_wxBITMAP_TYPE_JPEG_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("jpeg"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_PNG_sym);
  bitmapType_wxBITMAP_TYPE_PNG_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("png"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_PNG_MASK_sym);
  bitmapType_wxBITMAP_TYPE_PNG_MASK_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("png/mask"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_UNKNOWN_sym);
  bitmapType_wxBITMAP_TYPE_UNKNOWN_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("unknown"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_MASK_sym);
  bitmapType_wxBITMAP_TYPE_MASK_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("unknown/mask"));
}

MAYBE_UNUSED static int unbundle_symset_bitmapType(Scheme_Object *v, const char *where) {
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, v);
  if (!bitmapType_wxBITMAP_TYPE_MASK_sym) WITH_VAR_STACK(init_symset_bitmapType());
  if (0) { }
  else if (v == bitmapType_wxBITMAP_TYPE_BMP_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_BMP; }
  else if (v == bitmapType_wxBITMAP_TYPE_GIF_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_GIF; }
  else if (v == bitmapType_wxBITMAP_TYPE_GIF_MASK_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_GIF_MASK; }
  else if (v == bitmapType_wxBITMAP_TYPE_XBM_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_XBM; }
  else if (v == bitmapType_wxBITMAP_TYPE_XPM_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_XPM; }
  else if (v == bitmapType_wxBITMAP_TYPE_PICT_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_PICT; }
  else if (v == bitmapType_wxBITMAP_TYPE_JPEG_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_JPEG; }
  else if (v == bitmapType_wxBITMAP_TYPE_PNG_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_PNG; }
  else if (v == bitmapType_wxBITMAP_TYPE_PNG_MASK_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_PNG_MASK; }
  else if (v == bitmapType_wxBITMAP_TYPE_UNKNOWN_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_UNKNOWN; }
  else if (v == bitmapType_wxBITMAP_TYPE_MASK_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_MASK; }
  if (where) WITH_VAR_STACK(scheme_wrong_type(where, "bitmapType symbol", -1, 0, &v));
  READY_TO_RETURN;
  return 0;
}

MAYBE_UNUSED static Scheme_Object *bundle_symset_bitmapType(int v) {
  if (!bitmapType_wxBITMAP_TYPE_MASK_sym) init_symset_bitmapType();
  switch (v) {
  case wxBITMAP_TYPE_BMP: return bitmapType_wxBITMAP_TYPE_BMP_sym;
  case wxBITMAP_TYPE_GIF: return bitmapType_wxBITMAP_TYPE_GIF_sym;
  case wxBITMAP_TYPE_GIF_MASK: return bitmapType_wxBITMAP_TYPE_GIF_MASK_sym;
  case wxBITMAP_TYPE_XBM: return bitmapType_wxBITMAP_TYPE_XBM_sym;
  case wxBITMAP_TYPE_XPM: return bitmapType_wxBITMAP_TYPE_XPM_sym;
  case wxBITMAP_TYPE_PICT: return bitmapType_wxBITMAP_TYPE_PICT_sym;
  case wxBITMAP_TYPE_JPEG: return bitmapType_wxBITMAP_TYPE_JPEG_sym;
  case wxBITMAP_TYPE_PNG: return bitmapType_wxBITMAP_TYPE_PNG_sym;
  case wxBITMAP_TYPE_PNG_MASK: return bitmapType_wxBITMAP_TYPE_PNG_MASK_sym;
  case wxBITMAP_TYPE_UNKNOWN: return bitmapType_wxBITMAP_TYPE_UNKNOWN_sym;
  case wxBITMAP_TYPE_MASK: return bitmapType_wxBITMAP_TYPE_MASK_sym;
  default: return NULL;
  }
}



static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_BMP_sym = NULL;
static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_XBM_sym = NULL;
static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_XPM_sym = NULL;
static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_JPEG_sym = NULL;
static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_PNG_sym = NULL;

static void init_symset_saveBitmapType(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(saveBitmapType_wxBITMAP_TYPE_BMP_sym);
  saveBitmapType_wxBITMAP_TYPE_BMP_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("bmp"));
  wxREGGLOB(saveBitmapType_wxBITMAP_TYPE_XBM_sym);
  saveBitmapType_wxBITMAP_TYPE_XBM_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("xbm"));
  wxREGGLOB(saveBitmapType_wxBITMAP_TYPE_XPM_sym);
  saveBitmapType_wxBITMAP_TYPE_XPM_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("xpm"));
  wxREGGLOB(saveBitmapType_wxBITMAP_TYPE_JPEG_sym);
  saveBitmapType_wxBITMAP_TYPE_JPEG_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("jpeg"));
  wxREGGLOB(saveBitmapType_wxBITMAP_TYPE_PNG_sym);
  saveBitmapType_wxBITMAP_TYPE_PNG_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("png"));
}

MAYBE_UNUSED static int unbundle_symset_saveBitmapType(Scheme_Object *v, const char *where) {
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, v);
  if (!saveBitmapType_wxBITMAP_TYPE_PNG_sym) WITH_VAR_STACK(init_symset_saveBitmapType());
  if (0) { }
  else if (v == saveBitmapType_wxBITMAP_TYPE_BMP_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_BMP; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_XBM_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_XBM; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_XPM_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_XPM; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_JPEG_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_JPEG; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_PNG_sym) { READY_TO_RETURN; return wxBITMAP_TYPE_PNG; }
  if (where) WITH_VAR_STACK(scheme_wrong_type(where, "saveBitmapType symbol", -1, 0, &v));
  READY_TO_RETURN;
  return 0;
}


static Bool IsColor(wxBitmap *bm)
{
  return (bm->GetDepth() != 1);
}

extern void wxGetARGBPixels(wxBitmap *bm, double x, double y, int w, int h, char *s, Bool get_alpha);













class os_wxBitmap : public wxBitmap {
 public:

  os_wxBitmap CONSTRUCTOR_ARGS((bstring x0, int x1, int x2));
#ifndef MZ_PRECISE_GC
  os_wxBitmap CONSTRUCTOR_ARGS((int x0, int x1, Bool x2 = 0));
#endif
#ifndef MZ_PRECISE_GC
  os_wxBitmap CONSTRUCTOR_ARGS((pathname x0, int x1 = 0, class wxColour* x2 = NULL));
#endif
  ~os_wxBitmap();
#ifdef MZ_PRECISE_GC
  void gcMark();
  void gcFixup();
#endif
};

#ifdef MZ_PRECISE_GC
void os_wxBitmap::gcMark() {
  wxBitmap::gcMark();
}
void os_wxBitmap::gcFixup() {
  wxBitmap::gcFixup();
}
#endif

static Scheme_Object *os_wxBitmap_class;

os_wxBitmap::os_wxBitmap CONSTRUCTOR_ARGS((bstring x0, int x1, int x2))
CONSTRUCTOR_INIT(: wxBitmap(x0, x1, x2))
{
}

#ifndef MZ_PRECISE_GC
os_wxBitmap::os_wxBitmap CONSTRUCTOR_ARGS((int x0, int x1, Bool x2))
CONSTRUCTOR_INIT(: wxBitmap(x0, x1, x2))
{
}
#endif

#ifndef MZ_PRECISE_GC
os_wxBitmap::os_wxBitmap CONSTRUCTOR_ARGS((pathname x0, int x1, class wxColour* x2))
CONSTRUCTOR_INIT(: wxBitmap(x0, x1, x2))
{
}
#endif

os_wxBitmap::~os_wxBitmap()
{
    objscheme_destroy(this, (Scheme_Object *) __gc_external);
}

static Scheme_Object *os_wxBitmapwxGetARGBPixels(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "get-argb-pixels in bitmap%", n, p);
  double x0;
  double x1;
  int x2;
  int x3;
  wbstring x4 INIT_NULLED_OUT;
  Bool x5;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x4);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_double(p[POFFSET+0], "get-argb-pixels in bitmap%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_double(p[POFFSET+1], "get-argb-pixels in bitmap%"));
  x2 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[POFFSET+2], 0, 10000, "get-argb-pixels in bitmap%"));
  x3 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[POFFSET+3], 0, 10000, "get-argb-pixels in bitmap%"));
  x4 = (wbstring)WITH_VAR_STACK(objscheme_unbundle_mutable_bstring(p[POFFSET+4], "get-argb-pixels in bitmap%"));
  if (n > (POFFSET+5)) {
    x5 = WITH_VAR_STACK(objscheme_unbundle_bool(p[POFFSET+5], "get-argb-pixels in bitmap%"));
  } else
    x5 = FALSE;

  { if (!((wxBitmap *)((Scheme_Class_Object *)THEOBJ)->primdata)->Ok()) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("bitmap%","get-argb-pixels"), "bad bitmap: ", THEOBJ)); }if (SCHEME_BYTE_STRTAG_VAL(p[4+POFFSET]) < (x2 * x3 * 4)) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("bitmap%","get-argb-pixels"), "byte string too short: ", p[4+POFFSET]));
  WITH_VAR_STACK(wxGetARGBPixels(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata), x0, x1, x2, x3, x4, x5));

  
  
  READY_TO_RETURN;
  return scheme_void;
}

static Scheme_Object *os_wxBitmapGetGLConfig(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  class wxGLConfig* r;
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "get-gl-config in bitmap%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)->GetGLConfig());

  
  
  READY_TO_RETURN;
  return WITH_REMEMBERED_STACK(objscheme_bundle_wxGLConfig(r));
}

static Scheme_Object *os_wxBitmapSetGLConfig(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "set-gl-config in bitmap%", n, p);
  class wxGLConfig* x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxGLConfig(p[POFFSET+0], "set-gl-config in bitmap%", 1));

  
  WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)->SetGLConfig(x0));

  
  
  READY_TO_RETURN;
  return scheme_void;
}

static Scheme_Object *os_wxBitmapSetMask(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "set-loaded-mask in bitmap%", n, p);
  class wxBitmap* x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxBitmap(p[POFFSET+0], "set-loaded-mask in bitmap%", 0));

  
  WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)->SetMask(x0));

  
  
  READY_TO_RETURN;
  return scheme_void;
}

static Scheme_Object *os_wxBitmapGetMask(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  class wxBitmap* r;
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "get-loaded-mask in bitmap%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)->GetMask());

  
  
  READY_TO_RETURN;
  return WITH_REMEMBERED_STACK(objscheme_bundle_wxBitmap(r));
}

static Scheme_Object *os_wxBitmapSaveFile(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "save-file in bitmap%", n, p);
  wpathname x0 INIT_NULLED_OUT;
  int x1;
  int x2;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = (wpathname)WITH_VAR_STACK(objscheme_unbundle_write_pathname(p[POFFSET+0], "save-file in bitmap%"));
  x1 = WITH_VAR_STACK(unbundle_symset_saveBitmapType(p[POFFSET+1], "save-file in bitmap%"));
  if (n > (POFFSET+2)) {
    x2 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[POFFSET+2], 0, 100, "save-file in bitmap%"));
  } else
    x2 = 75;

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)->SaveFile(x0, x1, x2));

  if (1) WITH_VAR_STACK(scheme_thread_block(0.0));
  
  READY_TO_RETURN;
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxBitmapLoadFile(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "load-file in bitmap%", n, p);
  pathname x0 INIT_NULLED_OUT;
  int x1;
  class wxColour* x2 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x2);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[POFFSET+0], "load-file in bitmap%"));
  if (n > (POFFSET+1)) {
    x1 = WITH_VAR_STACK(unbundle_symset_bitmapType(p[POFFSET+1], "load-file in bitmap%"));
  } else
    x1 = 0;
  if (n > (POFFSET+2)) {
    x2 = WITH_VAR_STACK(objscheme_unbundle_wxColour(p[POFFSET+2], "load-file in bitmap%", 1));
  } else
    x2 = NULL;

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)->LoadFile(x0, x1, x2));

  if (r) WITH_VAR_STACK(scheme_thread_block(0.0));
  
  READY_TO_RETURN;
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxBitmapIsColor(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "is-color? in bitmap%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(IsColor(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)));

  
  
  READY_TO_RETURN;
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxBitmapOk(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "ok? in bitmap%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)->Ok());

  
  
  READY_TO_RETURN;
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxBitmapGetWidth(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "get-width in bitmap%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)->GetWidth());

  
  
  READY_TO_RETURN;
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxBitmapGetHeight(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "get-height in bitmap%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)->GetHeight());

  
  
  READY_TO_RETURN;
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxBitmapGetDepth(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  p[0] = objscheme_unwrap(p[0], os_wxBitmap_class);
  objscheme_check_valid(os_wxBitmap_class, "get-depth in bitmap%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)p[0])->primdata)->GetDepth());

  
  
  READY_TO_RETURN;
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxBitmap_ConstructScheme(int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, p);
  os_wxBitmap *realobj INIT_NULLED_OUT;
  REMEMBER_VAR_STACK();
  if ((n >= (POFFSET+1)) && WITH_REMEMBERED_STACK(objscheme_istype_pathname(p[POFFSET+0], NULL))) {
    pathname x0 INIT_NULLED_OUT;
    int x1;
    class wxColour* x2 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(4);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, realobj);
    VAR_STACK_PUSH(2, x0);
    VAR_STACK_PUSH(3, x2);

    
    if ((n < (POFFSET+1)) || (n > (POFFSET+3))) 
      WITH_VAR_STACK(scheme_wrong_count_m("initialization in bitmap% (pathname case)", POFFSET+1, POFFSET+3, n, p, 1));
    x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[POFFSET+0], "initialization in bitmap% (pathname case)"));
    if (n > (POFFSET+1)) {
      x1 = WITH_VAR_STACK(unbundle_symset_bitmapType(p[POFFSET+1], "initialization in bitmap% (pathname case)"));
    } else
      x1 = 0;
    if (n > (POFFSET+2)) {
      x2 = WITH_VAR_STACK(objscheme_unbundle_wxColour(p[POFFSET+2], "initialization in bitmap% (pathname case)", 1));
    } else
      x2 = NULL;

    
    realobj = WITH_VAR_STACK(new os_wxBitmap CONSTRUCTOR_ARGS((x0, x1, x2)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxBitmap(x0, x1, x2));
#endif
    realobj->__gc_external = (void *)p[0];
    if (realobj->Ok()) WITH_VAR_STACK(scheme_thread_block(0.0));
    
    READY_TO_PRE_RETURN;
  } else if ((n >= (POFFSET+1)) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[POFFSET+0], NULL))) {
    int x0;
    int x1;
    Bool x2;

    SETUP_VAR_STACK_PRE_REMEMBERED(2);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, realobj);

    
    if ((n < (POFFSET+2)) || (n > (POFFSET+3))) 
      WITH_VAR_STACK(scheme_wrong_count_m("initialization in bitmap% (width/height case)", POFFSET+2, POFFSET+3, n, p, 1));
    x0 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[POFFSET+0], 1, 10000, "initialization in bitmap% (width/height case)"));
    x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[POFFSET+1], 1, 10000, "initialization in bitmap% (width/height case)"));
    if (n > (POFFSET+2)) {
      x2 = WITH_VAR_STACK(objscheme_unbundle_bool(p[POFFSET+2], "initialization in bitmap% (width/height case)"));
    } else
      x2 = 0;

    
    realobj = WITH_VAR_STACK(new os_wxBitmap CONSTRUCTOR_ARGS((x0, x1, x2)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxBitmap(x0, x1, x2));
#endif
    realobj->__gc_external = (void *)p[0];
    
    
    READY_TO_PRE_RETURN;
  } else  {
    bstring x0 INIT_NULLED_OUT;
    int x1;
    int x2;

    SETUP_VAR_STACK_PRE_REMEMBERED(3);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, realobj);
    VAR_STACK_PUSH(2, x0);

    
    if (n != (POFFSET+3)) 
      WITH_VAR_STACK(scheme_wrong_count_m("initialization in bitmap% (datastring case)", POFFSET+3, POFFSET+3, n, p, 1));
    x0 = (bstring)WITH_VAR_STACK(objscheme_unbundle_bstring(p[POFFSET+0], "initialization in bitmap% (datastring case)"));
    x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[POFFSET+1], 1, 10000, "initialization in bitmap% (datastring case)"));
    x2 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[POFFSET+2], 1, 10000, "initialization in bitmap% (datastring case)"));

    if (SCHEME_BYTE_STRTAG_VAL(p[POFFSET]) < (((x1 * x2) + 7) >> 3)) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("bitmap%","initialization"), "byte string too short: ", p[POFFSET]));
    realobj = WITH_VAR_STACK(new os_wxBitmap CONSTRUCTOR_ARGS((x0, x1, x2)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxBitmap(x0, x1, x2));
#endif
    realobj->__gc_external = (void *)p[0];
    
    
    READY_TO_PRE_RETURN;
  }

  ((Scheme_Class_Object *)p[0])->primdata = realobj;
  ((Scheme_Class_Object *)p[0])->primflag = 1;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(p[0], &((Scheme_Class_Object *)p[0])->primdata));
  return scheme_void;
}

void objscheme_setup_wxBitmap(Scheme_Env *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxBitmap_class);

  os_wxBitmap_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "bitmap%", "object%", (Scheme_Method_Prim *)os_wxBitmap_ConstructScheme, 12));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "get-argb-pixels" " method", (Scheme_Method_Prim *)os_wxBitmapwxGetARGBPixels, 5, 6));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "get-gl-config" " method", (Scheme_Method_Prim *)os_wxBitmapGetGLConfig, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "set-gl-config" " method", (Scheme_Method_Prim *)os_wxBitmapSetGLConfig, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "set-loaded-mask" " method", (Scheme_Method_Prim *)os_wxBitmapSetMask, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "get-loaded-mask" " method", (Scheme_Method_Prim *)os_wxBitmapGetMask, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "save-file" " method", (Scheme_Method_Prim *)os_wxBitmapSaveFile, 2, 3));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "load-file" " method", (Scheme_Method_Prim *)os_wxBitmapLoadFile, 1, 3));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "is-color?" " method", (Scheme_Method_Prim *)os_wxBitmapIsColor, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "ok?" " method", (Scheme_Method_Prim *)os_wxBitmapOk, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "get-width" " method", (Scheme_Method_Prim *)os_wxBitmapGetWidth, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "get-height" " method", (Scheme_Method_Prim *)os_wxBitmapGetHeight, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "get-depth" " method", (Scheme_Method_Prim *)os_wxBitmapGetDepth, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxBitmap_class));


  READY_TO_RETURN;
}

int objscheme_istype_wxBitmap(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  obj = objscheme_unwrap(obj, os_wxBitmap_class);
  if (objscheme_is_a(obj, os_wxBitmap_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "bitmap% object or " XC_NULL_STR: "bitmap% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxBitmap(class wxBitmap *realobj)
{
  Scheme_Class_Object *obj INIT_NULLED_OUT;
  Scheme_Object *sobj INIT_NULLED_OUT;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, realobj);

  if ((sobj = WITH_VAR_STACK(objscheme_bundle_by_type(realobj, realobj->__type))))
    { READY_TO_RETURN; return sobj; }
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxBitmap_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(obj, &obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  READY_TO_RETURN;
  return (Scheme_Object *)obj;
}

class wxBitmap *objscheme_unbundle_wxBitmap(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  obj = objscheme_unwrap(obj, os_wxBitmap_class);
  (void)objscheme_istype_wxBitmap(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(NULL, NULL, 0, &obj));
  if (o->primflag)
    return (os_wxBitmap *)o->primdata;
  else
    return (wxBitmap *)o->primdata;
}
