//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#if defined(WIN32) || defined(WINNT)
#include <pshpack1.h>
#undef ATTRIBUTE_PACKED_UNALIGNED
#define ATTRIBUTE_PACKED_UNALIGNED ATTRIBUTE_ALIGNED(1)
#else
#undef ATTRIBUTE_PACKED_UNALIGNED
#define ATTRIBUTE_PACKED_UNALIGNED __attribute__((packed)) __attribute__((aligned(1)))
#endif
