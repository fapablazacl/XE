IMAGE ?= fapablazacl/cpp
IMAGE_XE ?= fapablazacl/xe
DOCKER ?= docker
BUILD_CONTEXT := docker/cpp
BUILD_CONTEXT_XE := docker/xe

.PHONY: docker clean
.PHONY: format
.PHONY: tidy

docker:
	$(DOCKER) build -t $(IMAGE) $(BUILD_CONTEXT)
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 $(IMAGE) sh -c 'conan profile detect'
	
configure:
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace $(IMAGE) sh -c 'conan install . --build=missing --output-folder=output-docker -c tools.cmake.cmaketoolchain:user_presets='
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace -w /workspace/output-docker $(IMAGE) sh -c 'cmake --preset conan-release'
	
format:
	$(DOCKER) run --rm -v $(CURDIR):/workspace -w /workspace $(IMAGE) sh -c 'find src -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" -o -name "*.hpp" -o -name "*.hh" -o -name "*.h" \) -print0 | xargs -0 -r clang-format -i'

tidy:
	$(DOCKER) run --rm -v $(CURDIR):/workspace -w /workspace/build/Debug $(IMAGE) sh -c 'run-clang-tidy ../../'

clean:
	@echo "Nothing to clean."
