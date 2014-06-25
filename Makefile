src := src
resh := research
tlkt := toolkit
checkdir := test

resh_sources := $(wildcard $(resh)/*/*.c)
resh_dirs := $(sort $(dir $(resh_sources)))
resh_dirsname := $(patsubst $(resh)/%/,%,$(resh_dirs))

#sources := $(filter-out $(resh)/*/common.c,$(resh_sources))
#sources := $(subst $(resh)/,,$(sources))
#sources += $(wildcard $(tlkt)/*.c)
#sources := $(subst .c,,$(sources))
#sources := $(subst /,-,$(sources))

test :
tdpotn :

.PHONY : all clean $(resh) $(tlkt) $(src) $(resh_dirsname) $(checkdir) dist check

all : $(src) $(checkdir) $(tlkt) $(resh) 

$(resh) $(tlkt) $(checkdir) : $(src)
	@$(MAKE) -C $@ all

$(resh_dirsname) : $(src)
	@$(MAKE) -C $(resh) $@

$(src) :
	@$(MAKE) -C $@ all

clean :
	@$(RM) cnrt.tar.gz *.log
	@for d in $(src) $(tlkt) $(resh) $(checkdir); \
		do \
		$(MAKE) -C $$d clean; \
		done

dist : clean
	tar zcvf cnrt.tar.gz $(src) $(resh) $(tlkt) $(checkdir) Makefile .git

check : $(checkdir)
	@./test/test

