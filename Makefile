IMAGE ?= fapablazacl/cpp-archlinux:latest
DOCKER ?= docker
BUILD_CONTEXT := docker/cpp-archlinux
DOCKER_CONTEXT := $(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 -v $(CURDIR):/workspace $(IMAGE)

CONAN_PROFILE_DETECT := conan profile detect
CONAN_INSTALL_RELEASE := conan install . --build=missing  --settings=build_type=Release
CONAN_INSTALL_DEBUG := conan install . --build=missing  --settings=build_type=Debug
CMAKE_CONFIGURE_RELEASE := cmake --preset conan-release
CMAKE_CONFIGURE_DEBUG := cmake --preset conan-debug
CP_COMPILE_COMMANDS_JSON := cp build/Debug/compile_commands.json .
CLANG_FORMAT := find src -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" -o -name "*.hpp" -o -name "*.hh" -o -name "*.h" \) -print0 | xargs -0 -r clang-format -i
CLANG_TIDY := run-clang-tidy -p build/Debug/ -header-filter=.*
CPPCHECK := cppcheck --project=build/Debug/compile_commands.json
CTEST := ctest --test-dir build/Debug --output-on-failure

IWYU_FIXES := /tmp/iwyu.out
IWYU := iwyu_tool.py -p build/Debug
IWYU_GEN_FIXES := $(IWYU) > $(IWYU_FIXES)
IWYU_APPLY_FIXES := fix_includes.py < $(IWYU_FIXES)
IWYU_CLEAR_FIXES := rm $(IWYU_FIXES)

.PHONY: clean
.PHONY: format tidy cppcheck test
.PHONY: docker 
.PHONY: docker-format docker-tidy docker-cppcheck docker-test

configure:
	$(CONAN_INSTALL_RELEASE)
	$(CONAN_INSTALL_DEBUG)
	$(CMAKE_CONFIGURE_RELEASE)
	$(CMAKE_CONFIGURE_DEBUG)

format:
	$(CLANG_FORMAT)

tidy:
	$(CLANG_TIDY)

tidy-fix:
	$(CLANG_TIDY) -fix

cppcheck:
	$(CPPCHECK)

test:
	$(CTEST)

iwyu:
	$(IWYU)

iwyu-fix:
	$(IWYU_GEN_FIXES)
	$(IWYU_APPLY_FIXES)
	$(IWYU_CLEAR_FIXES)

docker:
	$(DOCKER) buildx build --platform=linux/amd64 -t $(IMAGE) $(BUILD_CONTEXT)
	$(DOCKER) run --rm -v $(CURDIR)/.conan2-docker:/root/.conan2 $(IMAGE) sh -c '$(CONAN_PROFILE_DETECT)'
	
docker-configure:
	$(DOCKER_CONTEXT) sh -c '$(CONAN_INSTALL_RELEASE)'
	$(DOCKER_CONTEXT) sh -c '$(CONAN_INSTALL_DEBUG)'
	$(DOCKER_CONTEXT) sh -c '$(CMAKE_CONFIGURE_RELEASE)'
	$(DOCKER_CONTEXT) sh -c '$(CMAKE_CONFIGURE_DEBUG)'

docker-format:
	$(DOCKER_CONTEXT) sh -c '$(CLANG_FORMAT)'

docker-tidy:
	$(DOCKER_CONTEXT) sh -c '$(CLANG_TIDY)'

docker-cppcheck:
	$(DOCKER_CONTEXT) sh -c '$(CPPCHECK)'

docker-test:
	$(DOCKER_CONTEXT) sh -c '$(CTEST)'
	
clean:
	@echo "Nothing to clean."
