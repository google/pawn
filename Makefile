# Copyright 2014-2017 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Linux Makefile to build the pawn command line tool.

this_dir := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# Directory with build artifacts. Should not contain spaces.
build_dir := $(this_dir)/.build

# Default flags (all targets)
CPPFLAGS += "-iquote$(build_dir)"
CXXFLAGS += -std=c++11
LDFLAGS +=

# Feature: Debug build
ifndef DEBUG
CPPFLAGS += \
	-DNDEBUG
CXXFLAGS += \
	-O2 \
	-g0 -static
LDFLAGS += \
	-Wl,--gc-sections \
	-Wl,--strip-all \
	-Wl,-Bstatic \
	-static-libstdc++
else
CXXFLAGS += \
	-ggdb
LDFLAGS +=
endif

# Rules
CXXCOMPILE = $(CXX) $(CPPFLAGS)

pawn_sources := \
	chipset.cc \
	chipset_intel_ich8.cc \
	chipset_intel_ich9.cc \
	chipset_intel_ich10.cc \
	chipset_intel_6_series.cc \
	chipset_intel_7_series.cc \
	chipset_intel_8_series.cc \
	chipset_intel_9_series.cc \
	pawn.cc \
	pci.cc \
	physical_memory.cc
pawn_obj := $(addprefix $(build_dir)/,$(pawn_sources:.cc=.o))

all: pawn

help:
	@echo 'Make targets:'
	@echo '    clean            Remove object files'
	@echo '    distclean        Remove all build artifacts'
	@echo '    pawn             BIOS/firmware dumping tool'
	@echo
	@echo 'Feature flags:'
	@echo '    DEBUG            Build with debug symbols'
	@echo

clean:
	@find "$(build_dir)" -depth -name *.o -delete || true

distclean:
	@rm -rf "$(build_dir)"

$(build_dir):
	@mkdir -p "$(build_dir)/third_party/zynamics"
	@ln -sf "$(this_dir)" "$(build_dir)/third_party/zynamics/pawn"

$(pawn_obj): | $(build_dir)

$(build_dir)/pawn: $(pawn_obj)
	@echo "    [Link]    $@"
	@$(CXX) $(pawn_obj) \
	    $(CXXFLAGS) \
	    $(LDFLAGS) \
	    -o$@

pawn: $(build_dir)/pawn
	@echo 'Done, you can now run'
	@echo '    sudo $<'

# Implicit Rules
$(build_dir)/%.o: %.cc
	@echo "    [Compile] $@"
	@$(CXXCOMPILE) $(CXXFLAGS) -c -o$@ $<

.PHONY: all clean distclean help pawn

