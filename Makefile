src := src
resh := research
tlkt := toolkit

resh_sources := $(wildcard $(resh)/*/*.c)
resh_dirs := $(sort $(dir $(resh_sources)))
resh_dirsname := $(patsubst $(resh)/%/,%,$(resh_dirs))

#sources := $(filter-out $(resh)/*/common.c,$(resh_sources))
#sources := $(subst $(resh)/,,$(sources))
#sources += $(wildcard $(tlkt)/*.c)
#sources := $(subst .c,,$(sources))
#sources := $(subst /,-,$(sources))

tdpotn :

.PHONY : all clean $(resh) $(tlkt) $(src) $(resh_dirsname)

all : $(tlkt) $(resh) $(src)

$(resh) $(tlkt) : $(src)
	@$(MAKE) -C $@ all

$(resh_dirsname) : $(src)
	@$(MAKE) -C $(resh) $@

$(src) :
	@$(MAKE) -C $@ all

clean :
	@for d in $(src) $(tlkt) $(resh); \
		do \
		$(MAKE) -C $$d clean; \
		done
