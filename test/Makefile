sources := $(wildcard *.c)
libsources := $(filter-out main.c,$(wildcard *.c))
libobj := $(subst .c,.o,$(libsources))
exec := test

headers := -I ../src 
libcnrt := ../src/libcnrt.a
extralibs := -lm -lcheck -lssl -lcrypto

.PHONY : all clean 

all : $(exec)

$(exec) : main.o $(libobj) $(libcnrt) ../research/tdpotn/common.o
	$(LINK.o) $^ $(extralibs) -o $@

COMPILE.c += $(headers)

ifneq ($(MAKECMDGOALS),clean)
-include $(subst .c,.d,$(sources))
endif

%.d : %.c
	$(CC) -M $(CPPFLAGS) $(headers) $< > $@.$$$$;                      \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;     \
	rm -f $@.$$$$

clean :
	@$(RM) *.o *.d $(exec) 
