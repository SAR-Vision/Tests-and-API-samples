# Linux build. The runner supplies KYFGLIB_ADAPTER_SUFFIX=A for adapter mode.
CXX ?= g++
CXXFLAGS ?= -O2 -g
KYFGLIB_ADAPTER_SUFFIX ?=
KAYA_LIB_DIR = $(if $(filter A,$(KYFGLIB_ADAPTER_SUFFIX)),$(or $(KAYA_VISION_POINT_2_LIB_PATH),$(KAYA_VISION_POINT_LIB_PATH)),$(KAYA_VISION_POINT_LIB_PATH))

.PHONY: all clean check-sdk FORCE
all: case_4729

check-sdk:
	@test -n "$(KAYA_VISION_POINT_INCLUDE_PATH)" || { echo "ERROR: set KAYA_VISION_POINT_INCLUDE_PATH"; exit 1; }
	@test -f "$(KAYA_VISION_POINT_INCLUDE_PATH)/KYFGLib.h" || { echo "ERROR: KYFGLib.h not found in KAYA_VISION_POINT_INCLUDE_PATH"; exit 1; }
	@test -n "$(KAYA_LIB_DIR)" || { echo "ERROR: set KAYA_VISION_POINT_LIB_PATH (or KAYA_VISION_POINT_2_LIB_PATH for adapter mode)"; exit 1; }

# Rebuild each invocation so changing the adapter or SDK never reuses a stale binary.
case_4729: case_4729.cpp FORCE | check-sdk
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -std=c++11 -pthread -I"$(KAYA_VISION_POINT_INCLUDE_PATH)" "$<" -o "$@" $(LDFLAGS) -L"$(KAYA_LIB_DIR)" -Wl,-rpath,"$(KAYA_LIB_DIR)" -Wl,-rpath,'$$ORIGIN' -lKYFGLib$(KYFGLIB_ADAPTER_SUFFIX) $(KYFLAGS) $(LDLIBS)

FORCE:

clean:
	$(RM) case_4729
