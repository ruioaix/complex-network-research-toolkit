src := src
resh := research
tlkt := toolkit
check := check

resh_sources := $(wildcard $(resh)/*/*.c)
resh_dirs := $(sort $(dir $(resh_sources)))
resh_dirsname := $(patsubst $(resh)/%/,%,$(resh_dirs))

#sources := $(filter-out $(resh)/*/common.c,$(resh_sources))
#sources := $(subst $(resh)/,,$(sources))
#sources += $(wildcard $(tlkt)/*.c)
#sources := $(subst .c,,$(sources))
#sources := $(subst /,-,$(sources))

#tdpotn :

.PHONY : all clean $(resh) $(tlkt) $(src) $(resh_dirsname) $(check) dist

all : $(tlkt) $(resh) $(src) $(check)

$(resh) $(tlkt) $(check) : $(src)
	@$(MAKE) -C $@ all

$(resh_dirsname) : $(src)
	@$(MAKE) -C $(resh) $@

$(src) :
	@$(MAKE) -C $@ all

clean :
	@$(RM) cnrt.tar.gz
	@for d in $(src) $(tlkt) $(resh) $(check); \
		do \
		$(MAKE) -C $$d clean; \
		done

dist : clean
	tar zcvf cnrt.tar.gz $(src) $(resh) $(tlkt) $(check) Makefile .git
