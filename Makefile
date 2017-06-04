BUILDDIR := ./build-$(shell uname -s)-$(shell uname -m)

all: ${BUILDDIR}/Makefile
	@$(MAKE) -C ${BUILDDIR}
.PHONY: all

distclean:
	rm -r ${BUILDDIR}
.PHONY: distclean

${BUILDDIR}/Makefile:
	@mkdir -p ${BUILDDIR}
	@(cd ${BUILDDIR}; cmake ..)

ifeq ($(findstring distclean,$(MAKECMDGOALS)),)
$(MAKECMDGOALS): ${BUILDDIR}/Makefile
	@$(MAKE) -C ${BUILDDIR} $(MAKECMDGOALS)
.PHONY: $(MAKECMDGOALS)
endif
