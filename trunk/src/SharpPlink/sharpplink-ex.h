#pragma once

#if defined _MSC_VER && _MSC_VER < 1900
typedef void *   DLL_DIRECTORY_COOKIE;
typedef void ** PDLL_DIRECTORY_COOKIE;
#endif

#if defined _MSC_VER && defined _M_AMD64 && _MSC_VER < 1600
static __forceinline 
unsigned char _addcarry_u64(unsigned char c_in, unsigned __int64 src1, unsigned __int64 src2, unsigned __int64 *sum_out)
{
  unsigned __int64 r = src1 + src2;
  unsigned __int64 r2 = r + (c_in ? 1 : 0);

  *sum_out = r2;

  if (r < src1 || r2 < r)
    return 1;
  else
    return 0;
}
#endif
