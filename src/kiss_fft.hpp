#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// Define KISS_FFT_SHARED macro to properly export symbols
#ifdef KISS_FFT_SHARED
# ifdef _WIN32
#  ifdef KISS_FFT_BUILD
#   define KISS_FFT_API __declspec(dllexport)
#  else
#   define KISS_FFT_API __declspec(dllimport)
#  endif
# else
#  define KISS_FFT_API __attribute__ ((visibility ("default")))
# endif
#else
# define KISS_FFT_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 ATTENTION!
 If you would like a :
 -- a utility that will handle the caching of fft objects
 -- real-only (no imaginary time component ) FFT
 -- a multi-dimensional FFT
 -- a command-line utility to perform ffts
 -- a command-line utility to perform fast-convolution filtering

 Then see kfc.h kiss_fftr.h kiss_fftnd.h fftutil.c kiss_fastfir.c
  in the tools/ directory.
*/

/* User may override KISS_FFT_MALLOC and/or KISS_FFT_FREE. */
#ifdef USE_SIMD
# include <xmmintrin.h>
# define kiss_fft_scalar __m128
# ifndef KISS_FFT_MALLOC
#  define KISS_FFT_MALLOC(nbytes) _mm_malloc(nbytes,16)
#  define KISS_FFT_ALIGN_CHECK(ptr) 
#  define KISS_FFT_ALIGN_SIZE_UP(size) ((size + 15UL) & ~0xFUL)
# endif
# ifndef KISS_FFT_FREE
#  define KISS_FFT_FREE _mm_free
# endif
#else
# define KISS_FFT_ALIGN_CHECK(ptr)
# define KISS_FFT_ALIGN_SIZE_UP(size) (size)
# ifndef KISS_FFT_MALLOC
#  define KISS_FFT_MALLOC malloc
# endif
# ifndef KISS_FFT_FREE
#  define KISS_FFT_FREE free
# endif
#endif


#ifdef FIXED_POINT
#include <stdint.h>
# if (FIXED_POINT == 32)
#  define kiss_fft_scalar int32_t
# else	
#  define kiss_fft_scalar int16_t
# endif
#else
# ifndef kiss_fft_scalar
/*  default is float */
#   define kiss_fft_scalar float
# endif
#endif



#define MAXFACTORS 32
/* e.g. an fft of length 128 has 4 factors
 as far as kissfft is concerned
 4*4*4*2
 */

struct kiss_fft_state{
    int nfft;
    int inverse;
    int factors[2*MAXFACTORS];
    kiss_fft_cpx twiddles[1];
};

/*
  Explanation of macros dealing with complex math:

   C_MUL(m,a,b)         : m = a*b
   C_FIXDIV( c , div )  : if a fixed point impl., c /= div. noop otherwise
   C_SUB( res, a,b)     : res = a - b
   C_SUBFROM( res , a)  : res -= a
   C_ADDTO( res , a)    : res += a
 * */
#ifdef FIXED_POINT
#include <stdint.h>
#if (FIXED_POINT==32)
# define FRACBITS 31
# define SAMPPROD int64_t
#define SAMP_MAX INT32_MAX
#define SAMP_MIN INT32_MIN
#else
# define FRACBITS 15
# define SAMPPROD int32_t
#define SAMP_MAX INT16_MAX
#define SAMP_MIN INT16_MIN
#endif

#if defined(CHECK_OVERFLOW)
#  define CHECK_OVERFLOW_OP(a,op,b)  \
    if ( (SAMPPROD)(a) op (SAMPPROD)(b) > SAMP_MAX || (SAMPPROD)(a) op (SAMPPROD)(b) < SAMP_MIN ) { \
        KISS_FFT_WARNING("overflow (%d " #op" %d) = %ld", (a),(b),(SAMPPROD)(a) op (SAMPPROD)(b)); }
#endif


#   define smul(a,b) ( (SAMPPROD)(a)*(b) )
#   define sround( x )  (kiss_fft_scalar)( ( (x) + (1<<(FRACBITS-1)) ) >> FRACBITS )

#   define S_MUL(a,b) sround( smul(a,b) )

#   define C_MUL(m,a,b) \
      do{ (m).r = sround( smul((a).r,(b).r) - smul((a).i,(b).i) ); \
          (m).i = sround( smul((a).r,(b).i) + smul((a).i,(b).r) ); }while(0)

#   define DIVSCALAR(x,k) \
    (x) = sround( smul(  x, SAMP_MAX/k ) )

#   define C_FIXDIV(c,div) \
    do {    DIVSCALAR( (c).r , div);  \
        DIVSCALAR( (c).i  , div); }while (0)

#   define C_MULBYSCALAR( c, s ) \
    do{ (c).r =  sround( smul( (c).r , s ) ) ;\
        (c).i =  sround( smul( (c).i , s ) ) ; }while(0)

#else  /* not FIXED_POINT*/

#   define S_MUL(a,b) ( (a)*(b) )
#define C_MUL(m,a,b) \
    do{ (m).r = (a).r*(b).r - (a).i*(b).i;\
        (m).i = (a).r*(b).i + (a).i*(b).r; }while(0)
#   define C_FIXDIV(c,div) /* NOOP */
#   define C_MULBYSCALAR( c, s ) \
    do{ (c).r *= (s);\
        (c).i *= (s); }while(0)
#endif

#ifndef CHECK_OVERFLOW_OP
#  define CHECK_OVERFLOW_OP(a,op,b) /* noop */
#endif

#define  C_ADD( res, a,b)\
    do { \
        CHECK_OVERFLOW_OP((a).r,+,(b).r)\
        CHECK_OVERFLOW_OP((a).i,+,(b).i)\
        (res).r=(a).r+(b).r;  (res).i=(a).i+(b).i; \
    }while(0)
#define  C_SUB( res, a,b)\
    do { \
        CHECK_OVERFLOW_OP((a).r,-,(b).r)\
        CHECK_OVERFLOW_OP((a).i,-,(b).i)\
        (res).r=(a).r-(b).r;  (res).i=(a).i-(b).i; \
    }while(0)
#define C_ADDTO( res , a)\
    do { \
        CHECK_OVERFLOW_OP((res).r,+,(a).r)\
        CHECK_OVERFLOW_OP((res).i,+,(a).i)\
        (res).r += (a).r;  (res).i += (a).i;\
    }while(0)

#define C_SUBFROM( res , a)\
    do {\
        CHECK_OVERFLOW_OP((res).r,-,(a).r)\
        CHECK_OVERFLOW_OP((res).i,-,(a).i)\
        (res).r -= (a).r;  (res).i -= (a).i; \
    }while(0)


#ifdef FIXED_POINT
#  define KISS_FFT_COS(phase)  floor(.5+SAMP_MAX * cos (phase))
#  define KISS_FFT_SIN(phase)  floor(.5+SAMP_MAX * sin (phase))
#  define HALF_OF(x) ((x)>>1)
#elif defined(USE_SIMD)
#  define KISS_FFT_COS(phase) _mm_set1_ps( cos(phase) )
#  define KISS_FFT_SIN(phase) _mm_set1_ps( sin(phase) )
#  define HALF_OF(x) ((x)*_mm_set1_ps(.5))
#else
#  define KISS_FFT_COS(phase) (kiss_fft_scalar) cos(phase)
#  define KISS_FFT_SIN(phase) (kiss_fft_scalar) sin(phase)
#  define HALF_OF(x) ((x)*((kiss_fft_scalar).5))
#endif

#define  kf_cexp(x,phase) \
    do{ \
        (x)->r = KISS_FFT_COS(phase);\
        (x)->i = KISS_FFT_SIN(phase);\
    }while(0)


/* a debugging function */
#define pcpx(c)\
    KISS_FFT_DEBUG("%g + %gi\n",(double)((c)->r),(double)((c)->i))


#ifdef KISS_FFT_USE_ALLOCA
// define this to allow use of alloca instead of malloc for temporary buffers
// Temporary buffers are used in two case:
// 1. FFT sizes that have "bad" factors. i.e. not 2,3 and 5
// 2. "in-place" FFTs.  Notice the quotes, since kissfft does not really do an in-place transform.
#include <alloca.h>
#define  KISS_FFT_TMP_ALLOC(nbytes) alloca(nbytes)
#define  KISS_FFT_TMP_FREE(ptr)
#else
#define  KISS_FFT_TMP_ALLOC(nbytes) KISS_FFT_MALLOC(nbytes)
#define  KISS_FFT_TMP_FREE(ptr) KISS_FFT_FREE(ptr)
#endif


typedef struct {
    kiss_fft_scalar r;
    kiss_fft_scalar i;
}kiss_fft_cpx;

typedef struct kiss_fft_state* kiss_fft_cfg;

/* 
 *  kiss_fft_alloc
 *  
 *  Initialize a FFT (or IFFT) algorithm's cfg/state buffer.
 *
 *  typical usage:      kiss_fft_cfg mycfg=kiss_fft_alloc(1024,0,NULL,NULL);
 *
 *  The return value from fft_alloc is a cfg buffer used internally
 *  by the fft routine or NULL.
 *
 *  If lenmem is NULL, then kiss_fft_alloc will allocate a cfg buffer using malloc.
 *  The returned value should be free()d when done to avoid memory leaks.
 *  
 *  The state can be placed in a user supplied buffer 'mem':
 *  If lenmem is not NULL and mem is not NULL and *lenmem is large enough,
 *      then the function places the cfg in mem and the size used in *lenmem
 *      and returns mem.
 *  
 *  If lenmem is not NULL and ( mem is NULL or *lenmem is not large enough),
 *      then the function returns NULL and places the minimum cfg 
 *      buffer size in *lenmem.
 * */

kiss_fft_cfg KISS_FFT_API kiss_fft_alloc(int nfft,int inverse_fft,void * mem,size_t * lenmem);

/*
 * kiss_fft(cfg,in_out_buf)
 *
 * Perform an FFT on a complex input buffer.
 * for a forward FFT,
 * fin should be  f[0] , f[1] , ... ,f[nfft-1]
 * fout will be   F[0] , F[1] , ... ,F[nfft-1]
 * Note that each element is complex and can be accessed like
    f[k].r and f[k].i
 * */
void KISS_FFT_API kiss_fft(kiss_fft_cfg cfg,const kiss_fft_cpx *fin,kiss_fft_cpx *fout);

/*
 A more generic version of the above function. It reads its input from every Nth sample.
 * */
void KISS_FFT_API kiss_fft_stride(kiss_fft_cfg cfg,const kiss_fft_cpx *fin,kiss_fft_cpx *fout,int fin_stride);

/* If kiss_fft_alloc allocated a buffer, it is one contiguous 
   buffer and can be simply free()d when no longer needed*/
#define kiss_fft_free KISS_FFT_FREE

/*
 Cleans up some memory that gets managed internally. Not necessary to call, but it might clean up 
 your compiler output to call this before you exit.
*/
void KISS_FFT_API kiss_fft_cleanup(void);
	

/*
 * Returns the smallest integer k, such that k>=n and k has only "fast" factors (2,3,5)
 */
int KISS_FFT_API kiss_fft_next_fast_size(int n);

/* for real ffts, we need an even size */
#define kiss_fftr_next_fast_size_real(n) \
        (kiss_fft_next_fast_size( ((n)+1)>>1)<<1)



 
struct kiss_fftr_state {
  kiss_fft_cfg substate;
  kiss_fft_cpx *tmpbuf;
  kiss_fft_cpx *super_twiddles;
#ifdef USE_SIMD
  void *pad;
#endif
};

kiss_fftr_cfg kiss_fftr_alloc(int nfft, int inverse_fft, void *mem, size_t *lenmem) {
  KISS_FFT_ALIGN_CHECK(mem)

  int i;
  kiss_fftr_cfg st = NULL;
  size_t subsize = 0, memneeded;

  if (nfft & 1) {
    KISS_FFT_ERROR("Real FFT optimization must be even.");
    return NULL;
  }
  nfft >>= 1;

  kiss_fft_alloc(nfft, inverse_fft, NULL, &subsize);
  memneeded = sizeof(struct kiss_fftr_state) + subsize + sizeof(kiss_fft_cpx) * (nfft * 3 / 2);

  if (lenmem == NULL) {
    st = (kiss_fftr_cfg)KISS_FFT_MALLOC(memneeded);
  } else {
    if (*lenmem >= memneeded) st = (kiss_fftr_cfg)mem;
    *lenmem = memneeded;
  }
  if (!st) return NULL;

  st->substate = (kiss_fft_cfg)(st + 1); /*just beyond kiss_fftr_state struct */
  st->tmpbuf = (kiss_fft_cpx *)(((char *)st->substate) + subsize);
  st->super_twiddles = st->tmpbuf + nfft;
  kiss_fft_alloc(nfft, inverse_fft, st->substate, &subsize);

  for (i = 0; i < nfft / 2; ++i) {
    double phase = -3.14159265358979323846264338327 * ((double)(i + 1) / nfft + .5);
    if (inverse_fft) phase *= -1;
    kf_cexp(st->super_twiddles + i, phase);
  }
  return st;
}

void kiss_fftr(kiss_fftr_cfg st, const kiss_fft_scalar *timedata, kiss_fft_cpx *freqdata) {
  /* input buffer timedata is stored row-wise */
  int k, ncfft;
  kiss_fft_cpx fpnk, fpk, f1k, f2k, tw, tdc;

  if (st->substate->inverse) {
    KISS_FFT_ERROR("kiss fft usage error: improper alloc");
    return; /* The caller did not call the correct function */
  }

  ncfft = st->substate->nfft;

  /*perform the parallel fft of two real signals packed in real,imag*/
  kiss_fft(st->substate, (const kiss_fft_cpx *)timedata, st->tmpbuf);
  /* The real part of the DC element of the frequency spectrum in st->tmpbuf
   * contains the sum of the even-numbered elements of the input time sequence
   * The imag part is the sum of the odd-numbered elements
   *
   * The sum of tdc.r and tdc.i is the sum of the input time sequence.
   *      yielding DC of input time sequence
   * The difference of tdc.r - tdc.i is the sum of the input (dot product) [1,-1,1,-1...
   *      yielding Nyquist bin of input time sequence
   */

  tdc.r = st->tmpbuf[0].r;
  tdc.i = st->tmpbuf[0].i;
  C_FIXDIV(tdc, 2);
  CHECK_OVERFLOW_OP(tdc.r, +, tdc.i);
  CHECK_OVERFLOW_OP(tdc.r, -, tdc.i);
  freqdata[0].r = tdc.r + tdc.i;
  freqdata[ncfft].r = tdc.r - tdc.i;
#ifdef USE_SIMD
  freqdata[ncfft].i = freqdata[0].i = _mm_set1_ps(0);
#else
  freqdata[ncfft].i = freqdata[0].i = 0;
#endif

  for (k = 1; k <= ncfft / 2; ++k) {
    fpk = st->tmpbuf[k];
    fpnk.r = st->tmpbuf[ncfft - k].r;
    fpnk.i = -st->tmpbuf[ncfft - k].i;
    C_FIXDIV(fpk, 2);
    C_FIXDIV(fpnk, 2);

    C_ADD(f1k, fpk, fpnk);
    C_SUB(f2k, fpk, fpnk);
    C_MUL(tw, f2k, st->super_twiddles[k - 1]);

    freqdata[k].r = HALF_OF(f1k.r + tw.r);
    freqdata[k].i = HALF_OF(f1k.i + tw.i);
    freqdata[ncfft - k].r = HALF_OF(f1k.r - tw.r);
    freqdata[ncfft - k].i = HALF_OF(tw.i - f1k.i);
  }
}

void kiss_fftri(kiss_fftr_cfg st, const kiss_fft_cpx *freqdata, kiss_fft_scalar *timedata) {
  /* input buffer timedata is stored row-wise */
  int k, ncfft;

  if (st->substate->inverse == 0) {
    KISS_FFT_ERROR("kiss fft usage error: improper alloc");
    return; /* The caller did not call the correct function */
  }

  ncfft = st->substate->nfft;

  st->tmpbuf[0].r = freqdata[0].r + freqdata[ncfft].r;
  st->tmpbuf[0].i = freqdata[0].r - freqdata[ncfft].r;
  C_FIXDIV(st->tmpbuf[0], 2);

  for (k = 1; k <= ncfft / 2; ++k) {
    kiss_fft_cpx fk, fnkc, fek, fok, tmp;
    fk = freqdata[k];
    fnkc.r = freqdata[ncfft - k].r;
    fnkc.i = -freqdata[ncfft - k].i;
    C_FIXDIV(fk, 2);
    C_FIXDIV(fnkc, 2);

    C_ADD(fek, fk, fnkc);
    C_SUB(tmp, fk, fnkc);
    C_MUL(fok, tmp, st->super_twiddles[k - 1]);
    C_ADD(st->tmpbuf[k], fek, fok);
    C_SUB(st->tmpbuf[ncfft - k], fek, fok);
#ifdef USE_SIMD
    st->tmpbuf[ncfft - k].i *= _mm_set1_ps(-1.0);
#else
    st->tmpbuf[ncfft - k].i *= -1;
#endif
  }
  kiss_fft(st->substate, st->tmpbuf, (kiss_fft_cpx *)timedata);
}       