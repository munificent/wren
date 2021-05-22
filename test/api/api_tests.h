#pragma once
#ifndef WREN_API_TESTS_H
#define WREN_API_TESTS_H

#include <stdio.h>
#include <string.h>

#include "wren.h"

#include "benchmark.h"
#include "call.h"
#include "call_calls_foreign.h"
#include "call_wren_call_root.h"
#include "error.h"
#include "get_variable.h"
#include "foreign_class.h"
#include "foreign_method_user_data.h"
#include "handle.h"
#include "lists.h"
#include "maps.h"
#include "new_vm.h"
#include "reset_stack_after_call_abort.h"
#include "reset_stack_after_foreign_construct.h"
#include "resolution.h"
#include "slots.h"
#include "user_data.h"

int APITest_Run(WrenVM* vm, const char* inTestName);

WrenBindForeignMethodResult APITest_bindForeignMethod(
    WrenVM* vm, const char* module, const char* className,
    bool isStatic, const char* signature);

WrenForeignClassMethods APITest_bindForeignClass(
    WrenVM* vm, const char* module, const char* className);


#endif //WREN_API_TESTS_H
