## Debian packaging

This directory contains the Debian package source for jailtime. To make this a
regular, non-native package, this sub-directory should not be included in the
original source. Use `make debsource` from the parent directory to build a
source tarball compliant with Debian policy. The contents of this directory
may then serve as a starting point for official packaging.
