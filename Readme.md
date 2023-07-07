Advent of code 2019
===================

Solutions to Advent of Code 2019 in C, using GLib 2.0.

Install dependencies:

    # Fedora
    dnf install glib2-devel

    # Ubuntu
    apt install libglib2-dev

Run AoC solutions:

    # Prepare build
    meson setup build

    # Build
    ninja -C build [dayXX]

    # Run
    build/dayXX

    # Build test
    ninja -C build [testXX]

    # Test (if present)
    meson test -C build [dayXX]
