CLANG_TIDY_BINARY = /usr/local/opt/llvm/bin/clang-tidy
CLANG_APPLY_REPLACEMENTS_BINARY = /usr/local/opt/llvm/bin/clang-apply-replacements
NPROCS = 4
BUILD_DIR =./build


.PHONY: clang-tidy
clang-tidy: 
	@./tools/run-clang-tidy.py -p "$(BUILD_DIR)" \
		-clang-tidy-binary $(CLANG_TIDY_BINARY) \
		-j$(NPROCS) \
		-header-filter=./src/*

.PHONY: clang-tidy-fix
clang-tidy-fix: 
	@./tools/run-clang-tidy.py -p "$(BUILD_DIR)" \
		-clang-tidy-binary $(CLANG_TIDY_BINARY) \
		-clang-apply-replacements-binary $(CLANG_APPLY_REPLACEMENTS_BINARY)
		-j$(NPROCS) -fix \
		-header-filter=.*