#include "wren.h"

WrenForeignMethodFn foreignClassUserDataBindMethod(const char* signature);
void foreignClassUserDataBindClass(
    const char* className, WrenForeignClassMethods* methods);
