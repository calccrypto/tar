## A simple tar implementation

Copyright (c) 2015 Jason Lee @ calccrypto at gmail.com

Please see LICENSE file for license.

[![Build Status](https://travis-ci.com/calccrypto/tar.svg?branch=master)](https://travis-ci.com/calccrypto/tar)

This is only a simple implementation of the tar file format.
It can tar files and extract them. That is about it. Although
there are some other utility functions written, they are a very
small subset that are provided by GNU Tar. Everything was written
based on Wikipedia and the observed results of GNU Tar.

This only works on Linux (or Linux-like environment, such as cygwin)
due to the sheer number of POSIX header files being used. The minimum
C standard needed is C99.

The purpose of this is to be a tar library that can be used
inside other programs, so that programs don't have to call
or perform the tarring outside of the program (such as with
system, exec, or through a script)

To build:

    make      - creates libtar.a
    make exec - makes the commandline interface 'exec'
    make test - tests the commandline interface

Usage:

  The library consists of some core functions for basic funtionality,
  some utility functions that expand on functionality, and some internal
  functions that should not be called from outside the function.

  Core Functions    | Description
 -------------------|---------------------------
  tar_read          | Read from a tar file. Expects address to a null pointer.
  tar_write         | Write to a tar file. If a non-empty archive is also provided, the new files will be appended to the older data.
  tar_free          | Frees up memory used by existing archive instances.
 -------------------------
  Utility Functions | Description
 -------------------|-------------------------
  tar_ls            | Prints the contents of an archive. Verbosity level changes what is printed.
  tar_extract       | Extracts the contents of an archive. A filter list can be provided to only extract certain files.
  tar_update        | Scans through the current working directory and appends any files that are updates of archive entries.
  tar_remove        | Given a list of entries, removes those entries from the archive.
  tar_diff          | Checks for differences between entries in the archive and the current working directory.

  Many of these functions are just wrappers around internal functions.
  All functions that involve changing the data in a `struct tar_t *` will
  take in the address of the archive (`struct tar_t **`).

  If any function that modifies archive variables errors, it is most
  likely that the data held in the archive variable is no longer valid.

The commandline interface only has a handful of options, each of which
runs one or more of the public functions. Type `help` into the commandline
interface to see its usage.
