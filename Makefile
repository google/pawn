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

# Source meta
version = 0.2
source_only_tgz = ../pawn_$(version).orig.tar.xz

# Directory with build artifacts. Should not contain spaces.
build_dir := $(this_dir)/.build

# Default flags (all targets)
override CPPFLAGS += "-iquote$(build_dir)"
override CXXFLAGS += -std=c++11
override LDFLAGS +=

# Feature: Debug build
ifndef DEBUG
override CPPFLAGS += \
	-DNDEBUG
override CXXFLAGS += \
	-O2 \
	-g0 -static
override LDFLAGS += \
	-Wl,--gc-sections \
	-Wl,--strip-all \
	-Wl,-Bstatic \
	-static-libstdc++
else
override CXXFLAGS += \
	-ggdb
override LDFLAGS +=
endif

# Feature: Verbose build
ifndef VERBOSE
V = @
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

.PHONY: all
all: pawn

.PHONY: help
help:
	@echo 'Make targets:'
	@echo '    clean            Remove all build artifacts'
	@echo '    deb              Build Debian package'
	@echo '    debclean         Remove packaging artifacts'
	@echo '    debsource        Create source tarball for packaging'
	@echo '    pawn             BIOS/firmware dumping tool'
	@echo
	@echo 'Feature flags:'
	@echo '    DEBUG            Build with debug symbols'
	@echo

.PHONY: clean
clean:
	$(V)rm -rf "$(build_dir)"

$(build_dir):
	$(V)mkdir -p "$(build_dir)/third_party/zynamics"
	$(V)ln -sf "$(this_dir)" "$(build_dir)/third_party/zynamics/pawn"

$(pawn_obj): | $(build_dir)

$(build_dir)/pawn: $(pawn_obj)
	@echo "  [Link]      $@"
	$(V)$(CXX) $(pawn_obj) \
	    $(CXXFLAGS) \
	    $(LDFLAGS) \
	    -o$@

pawn: $(build_dir)/pawn
	@echo 'Done, you can now run'
	@echo '    sudo $<'

# Implicit Rules
$(build_dir)/%.o: %.cc
	@echo "  [Compile]   $@"
	$(V)$(CXXCOMPILE) $(CXXFLAGS) -c -o$@ $<

$(source_only_tgz): clean
	@echo "  [Archive]   $@"
	$(V)tar -C "$(this_dir)" -caf "$@" \
		--transform=s,^,pawn-$(version)/, \
		--exclude=.build/* --exclude=.build \
		--exclude=.git/* --exclude=.git \
		--exclude=debian/* --exclude=debian \
		"--exclude=$@" \
		--exclude-vcs-ignores \
		.??* *

# Create a source tarball without the debian/ subdirectory
.PHONY: debsource
debsource: $(source_only_tgz)

# debuild signs the package iff DEBFULLNAME, DEBEMAIL and DEB_SIGN_KEYID are
# set. Note that if the GPG key includes an alias, it must match the latest
# entry in debian/changelog.
deb: debsource
	@echo "  [Debuild]   Building package"
	$(V)debuild

.PHONY: debclean
debclean: clean
	@echo "  [Deb-Clean] Removing artifacts"
	$(V)debuild -- clean
