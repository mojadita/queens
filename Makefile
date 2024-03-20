CFLAGS ?= -O0 -g

targets      = queens knight
toclean     += $(targets)
mans         = $(targets:=.1.gz)
toclean     += $(mans)
pdfs         = $(targets:=.1.pdf)
toclean     += $(pdfs)

prefix      ?= /usr/local
exec_prefix ?= $(prefix)
bindir      ?= $(exec_prefix)/bin
libdir      ?= $(exec_prefix)/lib
datarootdir ?= $(prefix)/share
datadir     ?= $(datarootdir)
mandir      ?= $(datarootdir)/man
man1dir     ?= $(mandir)/man1

INSTALL     ?= install
RM          ?= rm -f
RMDIR       ?= rmdir
GROFF       ?= groff
ROFFOPTS    ?=
GZIP        ?= gzip
GZOPTS      ?= -v


own         ?= root
grp         ?= wheel
xmod        ?= 0555
fmod        ?= 0444

.PHONY: all clean install uninstall doc
.SUFFIXES: .1 .1.gz .1.pdf

all: $(targets)
clean:
	rm -f $(toclean)

doc: $(mans) $(pdfs)

install: $(targets) $(mans)
	$(INSTALL) -o $(own) -g $(grp) -m $(dmod) -d $(man1dir)
	$(INSTALL) -o $(own) -g $(grp) -m $(dmod) -d $(bindir)
	$(INSTALL) -o $(own) -g $(grp) -m $(xmod) $(targets) $(bindir)
	$(INSTALL) -o $(own) -g $(grp) -m $(xmod) $(mans) $(man1dir)

uninstall:
	-for i in $(targets); \
	do $(RM) $(bindir)/$$i; \
	done
	-for i in $(mans); \
	do $(RM) $(mandir)/$$i; \
	done
	-$(RMDIR) $(bindir)
	-$(RMDIR) $(man1dir)
	
.1.1.gz:
	$(GZIP) $(GZOPTS) < $< >$@

.1.1.pdf:
	$(GROFF) $(ROFFOPTS) -mdoc -Tpdf $< >$@
