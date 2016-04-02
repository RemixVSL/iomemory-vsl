//-----------------------------------------------------------------------------
// |- Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_SPARC_CACHE__
#define __FIO_PORT_SPARC_CACHE__

#define FUSION_CACHE_LINE_SIZE  64 /* prtpicl -v -c cpu */
#define FUSION_HAS_PREFETCH      0

static inline void fusion_prefetch_t0(char *addr)
{
}

static inline void fusion_prefetch_nta(char *addr)
{
}

static inline void fusion_prefetch_page(void *addr)
{
}

#endif /* __FIO_PORT_SPARC_CACHE__ */
