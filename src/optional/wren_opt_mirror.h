#ifndef wren_opt_mirror_h
#define wren_opt_mirror_h

#include "wren_common.h"
#include "wren.h"

// This module defines the mirror infrastructure.
#if WREN_OPT_MIRROR

const char* wrenMirrorSource();
WrenForeignMethodFn wrenMirrorBindForeignMethod(WrenVM* vm,
                                                const char* className,
                                                bool isStatic,
                                                const char* signature);

#endif

#endif
