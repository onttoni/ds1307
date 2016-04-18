CSOURCES=$(wildcard $(SOURCEDIR)/*.c)
CPPSOURCES=$(wildcard $(SOURCEDIR)/*.cpp)
ASMSOURCES=$(wildcard $(SOURCEDIR)/*.S)
OBJECTS=$(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%_cpp.o,$(CPPSOURCES))
OBJECTS+=$(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%_c.o,$(CSOURCES))
OBJECTS+=$(patsubst $(SOURCEDIR)/%.S,$(BUILDDIR)/%_S.o,$(ASMSOURCES))


.PHONY: $(BUILDDIR)


all:	$(BUILDDIR) libds1307.a


libds1307.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(BUILDDIR)/$@ $(OBJECTS)


$(BUILDDIR)/%_cpp.o: $(SOURCEDIR)/%.cpp
	$(CC) $(CXXFLAGS) $< -o $@


$(BUILDDIR)/%_c.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@


$(BUILDDIR)/%_S.o: $(SOURCEDIR)/%.S
	$(CC) $(ASFLAGS) $< -o $@


$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

clean:
	@rm -fr $(BUILDDIR)

