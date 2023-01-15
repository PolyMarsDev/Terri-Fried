NAME        = Terri-fried

CXX         = g++

SOURCES     = $(wildcard *.cpp)

OBJ_DIR     = build/obj
OUT_DIR     = build/bin
OBJS        = $(patsubst %, $(OBJ_DIR)/%, $(SOURCES:.cpp=.o))
RES_FILES   = winres/TF-icon.res
MKDIR_P     = mkdir -p

FLAGS       = -O3 -std=c++20 
LIBS        = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf


.PHONY: directories

all: directories $(NAME)

directories: $(OBJ_DIR) $(OUT_DIR)

$(OBJ_DIR):
	$(MKDIR_P) $(OBJ_DIR)

$(OUT_DIR):
	$(MKDIR_P) $(OUT_DIR)


$(NAME): $(OBJS)
	$(CXX) $(FLAGS) -o $(OUT_DIR)/$(NAME) $(OBJS) $(RES_FILES) $(LIBS)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(FLAGS) -c $< -o $@
	
clean:
	@echo clean...
	@rm -fr build
