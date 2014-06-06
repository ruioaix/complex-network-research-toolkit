sources := $(wildcard *.c)
toolkits := $(notdir $(basename $(sources)))
topexec := $(addprefix ../toolkit-,$(toolkits))

headers := -I ../src
libcnrt := ../src/libcnrt.a
extralibs := -lm

.PHONY : all clean

all : $(toolkits)

$(toolkits) : % : %.o $(libcnrt)
	$(LINK.o) $^ $(extralibs) -o $@
	cp $@ $(patsubst %,../toolkit-%,$@)

COMPILE.c += $(headers)

ifneq ($(MAKECMDGOALS),clean)
-include $(subst .c,.d,$(sources))
endif

%.d : %.c
	@$(CC) -M $(CPPFLAGS) $(headers) $< > $@.$$$$;                      \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;     \
	rm -f $@.$$$$

clean :
	@$(RM) *.o *.d $(toolkits) $(topexec)
