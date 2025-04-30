CONAN_FILE = conanfile.txt

# conan definitions
CONAN_INCLUDE_DIR_FLAG ?= -I
CONAN_LIB_DIR_FLAG ?= -L
CONAN_LIB_FLAG ?= -l
# Include Conan-generated make variables
CONANDEPS_FILE = build/conandeps.mk
-include $(CONANDEPS_FILE)

# Create framework flags
FRAMEWORK_FLAGS = $(addprefix -framework ,$(CONAN_FRAMEWORKS))
CXXFLAGS = -std=c++17 -Wall -Wextra
SOURCE = $(wildcard src/*.cpp)
BUILD_DIR = build
EXECUTABLE = wadconvert

# first target in the makefile is the default target 
# (if you run make without arguments)
all:
	clang++ $(CXXFLAGS) $(SOURCE) $(CONAN_INCLUDE_DIRS) $(CONAN_LIB_DIRS) \
			-o $(BUILD_DIR)/$(EXECUTABLE) \
			$(CONAN_LIBS) \
			$(FRAMEWORK_FLAGS)

conan-clean:
	@echo "Cleaning Conan generated files..."
	@rm -f $(BUILD_DIR)/conandeps.mk
	@rm -f $(BUILD_DIR)/conan*
	@rm -f $(BUILD_DIR)/*conan*
	@rm -rf $(BUILD_DIR)/generators
	@rm -rf .conan

clean: conan-clean
	@rm -f $(BUILD_DIR)/$(EXECUTABLE)
	@rm -f $(BUILD_DIR)/*.o

debug:
	@echo "SOURCE: $(SOURCE)"
	@echo "CONANDEPS_FILE: $(CONANDEPS_FILE)"
	@echo "CONAN_INCLUDE_DIRS: $(CONAN_INCLUDE_DIRS)"
	@echo "CONAN_LIB_DIRS: $(CONAN_LIB_DIRS)"
	@echo "CONAN_LIBS: $(CONAN_LIBS)"
	@echo "FRAMEWORK_FLAGS: $(FRAMEWORK_FLAGS)"

# ------------------------------------------------------------------
# Library installation and building with conan
# ------------------------------------------------------------------

install:
	$(info Installing needed libraries:)
	@if [ ! -f $(CONANDEPS_FILE) ]; then \
		echo "Dependencies not found. Generating..." && \
		conan profile detect --force; \
		conan install $(CONAN_FILE) --build=missing --output-folder build/; \
	else \
		echo ; \
		echo "Dependencies found. To force regeneration, run:"; \
		echo "conan install $(CONAN_FILE) --build=missing --output-folder build/"; \
		echo ; \
	fi
