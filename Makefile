IMAGE ?= fapablazacl/cpp
DOCKER ?= docker
BUILD_CONTEXT := docker/cpp

.PHONY: docker clean
.PHONY: format

docker:
	$(DOCKER) build -t $(IMAGE) $(BUILD_CONTEXT)

format:
	$(DOCKER) run --rm -v $(CURDIR):/workspace -w /workspace $(IMAGE) sh -c 'clang-format -i src/*'

clean:
	@echo "Nothing to clean."
