#ifndef wren_opt_meta_h
#define wren_opt_meta_h

#include "wren_common.h"
#include "wren.h"

// This module defines the Meta class and its associated methods.
#if WREN_OPT_META

const char* wrenMetaSource();
WrenBindForeignMethodResult wrenMetaBindForeignMethod(WrenVM* vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature);

#endif

#endif
