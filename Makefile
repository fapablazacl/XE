IMAGE ?= fapablazacl/cpp-archlinux:latest
DOCKER ?= docker
BUILD_CONTEXT := docker/cpp-archlinux

.PHONY: docker clean
.PHONY: format
.PHONY: tidy

docker:
	$(DOCKER) build -t $(IMAGE) $(BUILD_CONTEXT)
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 $(IMAGE) sh -c 'conan profile detect'
	
configure:
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace $(IMAGE) sh -c 'conan install . --build=missing'
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace $(IMAGE) sh -c 'conan install . --build=missing --settings=build_type=Debug'
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace $(IMAGE) sh -c 'cmake --preset conan-release'
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace $(IMAGE) sh -c 'cmake --preset conan-debug'
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace $(IMAGE) sh -c 'cp build/Debug/compile_commands.json .'
	
format:
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace $(IMAGE) sh -c 'find src -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" -o -name "*.hpp" -o -name "*.hh" -o -name "*.h" \) -print0 | xargs -0 -r clang-format -i'

tidy:
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace $(IMAGE) sh -c 'run-clang-tidy -header-filter=.*'

clean:
	@echo "Nothing to clean."
