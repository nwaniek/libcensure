CC         = gcc
TARGETNAME = libcensure
LIBMAJOR   = 1
LIBMINOR   = 0.0
VERSION    = ${LIBMAJOR}.${LIBMINOR}
LIBSONAME  = ${TARGETNAME}.so.${LIBMAJOR}
LIBFILENAME= ${TARGETNAME}.so.${LIBMAJOR}.${LIBMINOR}
WARNINGS   = -Wall -Wextra -Wpointer-arith -Wcast-qual -Wswitch-default       \
	     -Wcast-align -Wundef -Wno-empty-body -Wreturn-type -Wformat -W   \
	     -Wtrigraphs -Wno-unused-function -Wmultichar -Wparentheses       \
	     -Wchar-subscripts -Werror
INCS       = -I./src
CPPFLAGS   = -DVERSION=\"${VERSION}\"
CFLAGS     = -std=c99 -pedantic -fPIC -ffast-math ${INCS} ${WARNINGS} \
	     ${CPPFLAGS}
#CFLAGS    += -DDEBUG -ggdb
CFLAGS    += -O3
LDFLAGS    = -shared -Wl,-soname,${LIBSONAME}
SRCDIR     = src
SRC        = $(shell find $(SRCDIR) -name \*.c -type f -print)
OBJDIR     = build
OBJ        = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(patsubst %.c,%.o,$(SRC)))
DIRTREE    = $(OBJDIR) \
	     $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,\
	     $(shell find $(SRCDIR)/* -type d -print))

all: makedirs ${OBJ}
	@echo ' [LD] '${LIBFILENAME}
	@${CC} ${OBJ} ${LDFLAGS} -o ${LIBFILENAME}

-include ${OBJ:.o=.d}

${OBJDIR}/%.o: ${SRCDIR}/%.c
	@echo ' [CC] '$<
	@${CC} ${CFLAGS} -c -o $@ $<
	@${CC} -MM $< > $(patsubst %.o,%.d,$@)

makedirs:
	@mkdir -p ${DIRTREE}

clean:
	@rm -f ${OBJ} ${OBJ:.o=.d} ${LIBFILENAME}
	@rm -rf ${OBJDIR}
