IMAGE ?= fapablazacl/cpp-archlinux:latest
DOCKER ?= docker
BUILD_CONTEXT := docker/cpp-archlinux
DOCKER_CONTEXT := $(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace $(IMAGE)

.PHONY: docker clean
.PHONY: format
.PHONY: tidy

docker:
	$(DOCKER) buildx build --platform=linux/amd64 -t $(IMAGE) $(BUILD_CONTEXT)
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 $(IMAGE) sh -c 'conan profile detect'
	
configure:
	$(DOCKER_CONTEXT) sh -c 'conan install . --build=missing'
	$(DOCKER_CONTEXT) sh -c 'conan install . --build=missing --settings=build_type=Debug'
	$(DOCKER_CONTEXT) sh -c 'cmake --preset conan-release'
	$(DOCKER_CONTEXT) sh -c 'cmake --preset conan-debug'
	$(DOCKER_CONTEXT) sh -c 'cp build/Debug/compile_commands.json .'
	
format:
	$(DOCKER_CONTEXT) sh -c 'find src -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" -o -name "*.hpp" -o -name "*.hh" -o -name "*.h" \) -print0 | xargs -0 -r clang-format -i'

tidy:
	$(DOCKER_CONTEXT) sh -c 'run-clang-tidy -header-filter=.*'

clean:
	@echo "Nothing to clean."
