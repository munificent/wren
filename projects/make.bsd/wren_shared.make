# Alternative GNU Make project makefile autogenerated by Premake

ifndef config
  config=release_64bit
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild

SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

# Configurations
# #############################################

RESCOMP = windres
INCLUDES += -I../../src/include -I../../src/vm -I../../src/optional
FORCE_INCLUDE +=
ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
LIBS += -lm
LDDEPS +=
LINKCMD = $(CC) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
define PREBUILDCMDS
endef
define PRELINKCMDS
endef
define POSTBUILDCMDS
endef

ifeq ($(config),release_64bit)
TARGETDIR = ../../lib
TARGET = $(TARGETDIR)/libwren.so
OBJDIR = obj/64bit/Release/wren_shared
DEFINES += -DNDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -O2 -fPIC -std=c99
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -O2 -fPIC
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -shared -s

else ifeq ($(config),release_32bit)
TARGETDIR = ../../lib
TARGET = $(TARGETDIR)/libwren.so
OBJDIR = obj/32bit/Release/wren_shared
DEFINES += -DNDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m32 -O2 -fPIC -std=c99
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m32 -O2 -fPIC
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib32 -m32 -shared -s

else ifeq ($(config),release_64bit-no-nan-tagging)
TARGETDIR = ../../lib
TARGET = $(TARGETDIR)/libwren.so
OBJDIR = obj/64bit-no-nan-tagging/Release/wren_shared
DEFINES += -DNDEBUG -DWREN_NAN_TAGGING=0
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -O2 -fPIC -std=c99
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -O2 -fPIC
ALL_LDFLAGS += $(LDFLAGS) -shared -s

else ifeq ($(config),debug_64bit)
TARGETDIR = ../../lib
TARGET = $(TARGETDIR)/libwren_d.so
OBJDIR = obj/64bit/Debug/wren_shared
DEFINES += -DDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -fPIC -g -std=c99
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -fPIC -g
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -shared

else ifeq ($(config),debug_32bit)
TARGETDIR = ../../lib
TARGET = $(TARGETDIR)/libwren_d.so
OBJDIR = obj/32bit/Debug/wren_shared
DEFINES += -DDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m32 -fPIC -g -std=c99
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m32 -fPIC -g
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib32 -m32 -shared

else ifeq ($(config),debug_64bit-no-nan-tagging)
TARGETDIR = ../../lib
TARGET = $(TARGETDIR)/libwren_d.so
OBJDIR = obj/64bit-no-nan-tagging/Debug/wren_shared
DEFINES += -DDEBUG -DWREN_NAN_TAGGING=0
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -fPIC -g -std=c99
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -fPIC -g
ALL_LDFLAGS += $(LDFLAGS) -shared

endif

# Per File Configurations
# #############################################


# File sets
# #############################################

GENERATED :=
OBJECTS :=

GENERATED += $(OBJDIR)/wren_compiler.o
GENERATED += $(OBJDIR)/wren_core.o
GENERATED += $(OBJDIR)/wren_debug.o
GENERATED += $(OBJDIR)/wren_opt_meta.o
GENERATED += $(OBJDIR)/wren_opt_mirror.o
GENERATED += $(OBJDIR)/wren_opt_random.o
GENERATED += $(OBJDIR)/wren_primitive.o
GENERATED += $(OBJDIR)/wren_utils.o
GENERATED += $(OBJDIR)/wren_value.o
GENERATED += $(OBJDIR)/wren_vm.o
OBJECTS += $(OBJDIR)/wren_compiler.o
OBJECTS += $(OBJDIR)/wren_core.o
OBJECTS += $(OBJDIR)/wren_debug.o
OBJECTS += $(OBJDIR)/wren_opt_meta.o
OBJECTS += $(OBJDIR)/wren_opt_mirror.o
OBJECTS += $(OBJDIR)/wren_opt_random.o
OBJECTS += $(OBJDIR)/wren_primitive.o
OBJECTS += $(OBJDIR)/wren_utils.o
OBJECTS += $(OBJDIR)/wren_value.o
OBJECTS += $(OBJDIR)/wren_vm.o

# Rules
# #############################################

all: $(TARGET)
	@:

$(TARGET): $(GENERATED) $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	$(PRELINKCMDS)
	@echo Linking wren_shared
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning wren_shared
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(GENERATED)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(GENERATED)) rmdir /s /q $(subst /,\\,$(GENERATED))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild: | $(OBJDIR)
	$(PREBUILDCMDS)

ifneq (,$(PCH))
$(OBJECTS): $(GCH) | $(PCH_PLACEHOLDER)
$(GCH): $(PCH) | prebuild
	@echo $(notdir $<)
	$(SILENT) $(CC) -x c-header $(ALL_CFLAGS) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
$(PCH_PLACEHOLDER): $(GCH) | $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) touch "$@"
else
	$(SILENT) echo $null >> "$@"
endif
else
$(OBJECTS): | prebuild
endif


# File Rules
# #############################################

$(OBJDIR)/wren_opt_meta.o: ../../src/optional/wren_opt_meta.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/wren_opt_mirror.o: ../../src/optional/wren_opt_mirror.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/wren_opt_random.o: ../../src/optional/wren_opt_random.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/wren_compiler.o: ../../src/vm/wren_compiler.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/wren_core.o: ../../src/vm/wren_core.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/wren_debug.o: ../../src/vm/wren_debug.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/wren_primitive.o: ../../src/vm/wren_primitive.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/wren_utils.o: ../../src/vm/wren_utils.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/wren_value.o: ../../src/vm/wren_value.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/wren_vm.o: ../../src/vm/wren_vm.c
	@echo $(notdir $<)
	$(SILENT) $(CC) $(ALL_CFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(PCH_PLACEHOLDER).d
endif