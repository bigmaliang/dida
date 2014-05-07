LOCALBASE = ../
include $(LOCALBASE)Make.env
include $(BASEDIR)Make.env

SUBDIR = js css

all: $(SUBDIR)
	@$(MULTIMAKE) $(SUBDIR)

clean:
	@$(MULTIMAKE) -m clean $(SUBDIR)

install:
