# Printer Emulator

A simple POSIX-based ESC/POS printer emulator written in C.

This project creates a pseudo-terminal printer endpoint at `/tmp/pos_printer`, accepts raw ESC/POS byte streams, parses printer control sequences, logs parser state transitions, and renders the received output into an HTML file.

## Key features

- Emulates a point-of-sale printer using a pseudo-terminal via `openpty()`
- Supports ESC/POS command parsing for ESC, GS, DLE, and FS command families
- Handles print-mode state changes such as bold, underline, and double-width/double-height text
- Converts text and escape sequences into HTML output
- Writes parser state transitions to `log_file` for debugging
- Includes a sample test application that sends ESC/POS underline commands to the emulator

## Repository structure

- `src/` — emulator implementation source files
- `include/` — shared header files
- `bin/` — reserved for built binaries (not populated by default)
- `build/` — object files created by the build process
- `printer_test_app/` — sample test application that writes ESC/POS bytes into `/tmp/pos_printer`
- `print_output.html` — generated HTML output (created by emulator runtime)
- `log_file` — parser log output file (created by emulator runtime)

## Requirements

- Linux or POSIX-compatible environment
- `gcc`
- `make`

## Build

From the repository root:

```bash
make
```

This builds the emulator executable named `emulator`.

## Run

Start the emulator with root privileges:

```bash
sudo ./emulator
```

Once running, the emulator prints:

```text
Emulator started. Connect to the printer using /tmp/pos_printer
```

Send raw ESC/POS bytes to the printer endpoint, for example:

```bash
printf '\x1B\x2D\x80Hello World\x1B\x2D\x00' > /tmp/pos_printer
```

The emulator will process the commands and generate:

- `print_output.html` — rendered output
- `log_file` — parser state transitions and debug messages

Terminate the emulator with `Ctrl+C` to flush the final HTML output.

## Sample test application

A small sample app is provided in `printer_test_app/`.

To compile it manually:

```bash
gcc printer_test_app/main.c printer_test_app/test.c -o printer_test_app/test_app
```

Then run it while the emulator is active:

```bash
./printer_test_app/test_app
```

The sample sends an underline command and text to the emulator via `/tmp/pos_printer`.

## How it works

1. The emulator opens a pseudo-terminal pair using `openpty()`.
2. It creates a symbolic link at `/tmp/pos_printer` pointing to the slave side.
3. Data written to `/tmp/pos_printer` is read from the master side in `src/main.c`.
4. The parser in `src/emulator_parser.c` interprets ESC/POS sequences and updates printer state.
5. Printer state changes are handled in `src/emulator_printer.c` and rendered as HTML.
6. Parser transitions are logged by `src/emulator_logger.c`.

## Notes

- The emulator is a prototype and not production-ready.
- It is intended for testing and development of ESC/POS printer interactions.
- Not all ESC/POS commands are implemented; the focus is on basic formatting and user-defined character support.

## License

This project is licensed under the GNU General Public License version 3 (GPLv3).
See the included `LICENSE` file for the full license text.
