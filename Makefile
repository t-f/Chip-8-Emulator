# reminder
# -- rule --
# target : prerequisite
# <TAB> command
#
# Automatic variables:
# $@ target name
# $< first prerequisite name
# $^ all prerequisites separated by a space

# --------- Variables ---------
TARGET := program
PLATFORM := -m32
OBJS := main.o opcodes.o
DEBUG :=
CFLAGS := -c $(DEBUG) -Wall
LFLAGS_WIN :=
LFLAGS_LNX :=
INCLUDE := include/
OBJDIR := obj
SRCDIR := src
# --- Options ---
.PHONY : windows linux clean
vpath %.c $(SRCDIR)/

# ----------- Rules -----------
# compile on windows (default)
windows: $(TARGET).exe

# compile on linux
linux: $(TARGET).out


$(TARGET).exe: $(addprefix $(OBJDIR)/,$(OBJS))
	@echo Linking...
	gcc $^ $(LFLAGS_WIN) -o $@
	@echo Linked

$(TARGET).out: $(addprefix $(OBJDIR)/,$(OBJS))
	@echo linking...
	gcc $^ $(LFLAGS_LNX) -o $@
	@echo Linked


$(addprefix $(OBJDIR)/,$(OBJS)): $(INCLUDE)/include.h

$(OBJDIR)/%.o : %.c
	gcc -I$(INCLUDE) $(CFLAGS) $< $(PLATFORM) -o $@


clean:
	rm -f $(OBJDIR)/*.o $(TARGET).out $(TARGET).exe
