# Copyright 2014-2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
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
source_only_tgz = $(this_dir)/../../pawn_$(version).orig.tar.xz

# Feature: Verbose build
ifndef VERBOSE
V = @
endif

.PHONY: help
help:
	@echo 'Make targets:'
	@echo '    deb              Build Debian package'
	@echo '    debclean         Remove packaging artifacts'
	@echo '    debsource        Create source tarball for packaging'
	@echo '    updatesourcemeta Update copyright years'
	@echo
	@echo 'Feature flags:'
	@echo '    VERBOSE          Verbose build output'
	@echo

$(source_only_tgz):
	@echo "  [Archive]   $@"
	$(V)(cd "$(this_dir)/.." && tar -caf "$@" \
		--transform=s,^,pawn-$(version)/, \
		--exclude=build/* --exclude=build \
		--exclude=.git/* --exclude=.git \
		--exclude=debian/* --exclude=debian \
		"--exclude=$@" \
		--exclude-vcs-ignores \
		.??* *)

.PHONY: updatesourcemeta
updatesourcemeta:
	@echo "  [Update]    Version and copyright"
	@for i in \
		$(this_dir)/../debian/copyright \
		$(this_dir)/../debian/rules \
		$(this_dir)/../pawn/*.cc \
		$(this_dir)/../pawn/*.h \
		$(this_dir)/../pawn/CMakeLists.txt \
		$(this_dir)/../pawn/version.h.in \
		$(this_dir)/../tools/Makefile \
		$(this_dir)/../README.md; \
	do \
		sed -i \
			-e 's/\(Copyright [0-9]\+\)-[0-9]\+/\1-$(shell date +%Y)/' \
			$$i || true; \
	done

# Create a source tarball without the debian/ subdirectory
.PHONY: debsource
debsource: $(source_only_tgz)

# debuild signs the package iff DEBFULLNAME, DEBEMAIL and DEB_SIGN_KEYID are
# set. Note that if the GPG key includes an alias, it must match the latest
# entry in debian/changelog.
deb: debsource
	@echo "  [Debuild]   Building package"
	$(V)(cd $(this_dir)/.. && debuild)

.PHONY: debclean
debclean:
	@echo "  [Deb-Clean] Removing artifacts"
	$(V)(cd $(this_dir)/.. && debuild -- clean)
