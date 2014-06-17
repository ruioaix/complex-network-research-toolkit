sources := $(wildcard *.c)
exec_bname := $(notdir $(basename $(sources)))
topexec := $(addprefix ../check-,$(exec_bname))

headers := -I ../src
libcnrt := ../src/libcnrt.a
extralibs := -lm -lcheck

.PHONY : all clean

all : $(exec_bname)

$(exec_bname) : % : %.o $(libcnrt)
	$(LINK.o) $^ $(extralibs) -o $@
	cp $@ $(patsubst %,../check-%,$@)

COMPILE.c += $(headers)

ifneq ($(MAKECMDGOALS),clean)
-include $(subst .c,.d,$(sources))
endif

%.d : %.c
	@$(CC) -M $(CPPFLAGS) $(headers) $< > $@.$$$$;                      \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;     \
	rm -f $@.$$$$

clean :
	@$(RM) *.o *.d $(exec_bname) $(topexec)
