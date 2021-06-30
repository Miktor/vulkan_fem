CLANG_TIDY_BINARY = /usr/local/opt/llvm/bin/clang-tidy
CLANG_APPLY_REPLACEMENTS_BINARY = /usr/local/opt/llvm/bin/clang-apply-replacements
NPROCS = 4
BUILD_DIR =./build

.PHONY: clean
clean: 
	rm -rf "$(BUILD_DIR)"


.PHONY: clang-tidy
clang-tidy: 
	@./tools/run-clang-tidy.py -p "$(BUILD_DIR)" \
		-clang-tidy-binary $(CLANG_TIDY_BINARY) \
		-j$(NPROCS)

.PHONY: clang-tidy-fix
clang-tidy-fix: 
	@./tools/run-clang-tidy.py -p "$(BUILD_DIR)" \
		-clang-tidy-binary $(CLANG_TIDY_BINARY) \
		-clang-apply-replacements-binary $(CLANG_APPLY_REPLACEMENTS_BINARY) \
		-j$(NPROCS) -fix -format


.PHONY: iwyu
iwyu: 
	iwyu_tool.py -p .

.PHONY: iwyu-fix
iwyu-fix: 
	iwyu_tool.py -p . | fix_includes.py
