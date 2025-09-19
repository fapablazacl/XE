IMAGE ?= fapablazacl/cpp
DOCKER ?= docker
BUILD_CONTEXT := docker/cpp

.PHONY: docker clean
.PHONY: format

docker:
	$(DOCKER) build -t $(IMAGE) $(BUILD_CONTEXT)

format:
	$(DOCKER) run --rm -v $(CURDIR):/workspace -w /workspace $(IMAGE) sh -c 'find src -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" -o -name "*.hpp" -o -name "*.hh" -o -name "*.h" \) -print0 | xargs -0 -r clang-format -i'

clean:
	@echo "Nothing to clean."
