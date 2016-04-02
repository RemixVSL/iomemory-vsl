//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#if defined(WIN32) || defined(WINNT)
#include <poppack.h>
#undef ATTRIBUTE_PACKED_UNALIGNED
// NOTE: There is a similar error trap define in include/fio/dev/types.h.
#define ATTRIBUTE_PACKED_UNALIGNED ERROR ERROR_You_must_include_fio_pshpack1_h_before_and_fio_poppack_h_after_any_use_of_ATTRIBUTE_PACKED_UNALIGNED
#else
#undef ATTRIBUTE_PACKED_UNALIGNED
// NOTE: There is a similar error trap define in include/fio/dev/types.h.
#define ATTRIBUTE_PACKED_UNALIGNED ERROR ERROR_You_must_include_fio_pshpack1_h_before_and_fio_poppack_h_after_any_use_of_ATTRIBUTE_PACKED_UNALIGNED
#endif
