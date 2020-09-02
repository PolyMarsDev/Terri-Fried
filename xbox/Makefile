XBE_TITLE = Terri-Fried
GEN_XISO = $(XBE_TITLE).iso

SRCS =  $(wildcard $(CURDIR)/source/*.cpp) \
        $(wildcard $(CURDIR)/source/utils/*.cpp) \
        $(wildcard $(CURDIR)/source/gameobjects/*.cpp)

SHADER_OBJS =   source/shaders/sprite-frag.inl \
                source/shaders/guideline-frag.inl

NXDK_DIR = /opt/nxdk
NXDK_CXX = y
NXDK_SDL = y

%.inl: %.ps | fp20compiler
	@echo "[ FP20     ] $@"
	$(FP20COMPILER) $< > $@

include $(NXDK_DIR)/Makefile
