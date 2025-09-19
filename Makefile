IMAGE ?= fapablazacl/cpp
DOCKER ?= docker
BUILD_CONTEXT := docker/cpp

.PHONY: docker clean
.PHONY: format

docker:
	$(DOCKER) build -t $(IMAGE) $(BUILD_CONTEXT)

clean:
	@echo "Nothing to clean."

format:
	$(DOCKER) run --rm -v $(CURDIR):/workspace -w /workspace $(IMAGE) sh -c 'clang-format -i $$(find . -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))'
