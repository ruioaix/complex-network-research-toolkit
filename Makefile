lib := lib
resh := research
tlkt := toolkit
checkdir := tests

resh_sources := $(wildcard $(resh)/*/*.c)
resh_dirs := $(sort $(dir $(resh_sources)))
resh_dirsname := $(patsubst $(resh)/%/,%,$(resh_dirs))

tdpotn :
onion :
lib :
all :
test :

.PHONY : all clean $(resh) $(tlkt) $(lib) $(resh_dirsname) $(checkdir) dist check

all : $(lib) $(checkdir) $(tlkt) $(resh) 

$(resh) $(tlkt) : $(lib)
	@$(MAKE) -C $@ all

$(checkdir) : $(lib) $(resh)
	@$(MAKE) -C $@ all

$(resh_dirsname) : $(lib)
	@$(MAKE) -C $(resh) $@

$(lib) :
	@$(MAKE) -C $@ all

clean :
	@$(RM) cnrt*.tar.gz *.log
	@for d in $(lib) $(tlkt) $(resh) $(checkdir); \
		do \
		$(MAKE) -C $$d clean; \
		done

time = `date +%Y%m%d_%H%M%S`
dist : 
	tar zcvf cnrt_$(time).tar.gz $(lib) $(resh) $(tlkt) $(checkdir) Makefile .git

check : $(checkdir)
	@./tests/check_lib_research
