# Contributing

Thank you for contributing to Printer Emulator.
This project emulates a POSIX-based ESC/POS printer endpoint, parses ESC/POS commands, and renders output to HTML.
Contributions that improve parser accuracy, printer state handling, documentation, and sample usage are welcome.

## Getting started

1. Clone the repository:

```bash
git clone <repository-url>
cd printer_emulator
```

2. Build the emulator:

```bash
make
```

3. Run the emulator with the required permissions:

```bash
sudo ./emulator
```

4. Send raw ESC/POS bytes to `/tmp/pos_printer` or use the sample app in `printer_test_app/`.

## What to contribute

- Bug fixes for ESC/POS command parsing and printer state management
- New ESC/POS command handlers and printer behavior improvements
- Test code in printer_test_app
- Better HTML rendering and output formatting
- Documentation updates and sample command streams
- Improvements to the pseudo-terminal and cleanup flow

## Pull request process

1. Open an issue first for big changes or new feature proposals.
2. Create a branch from `main`.
3. Build with `make` and verify that the emulator still compiles.
4. Test by running the emulator and reviewing `print_output.html` and `log_file`.
5. Update `README.md` if your change affects usage, outputs, or runtime behavior.
6. Submit a pull request with a clear description, motivation, and test steps.

## Coding guidelines

- Follow the existing C style in `src/` and `include/`.
- Prefer standard POSIX APIs and avoid unnecessary dependencies.
- Keep changes small and focused.
- Document new parser behavior and command handlers clearly.

## Reporting issues

When opening an issue, include:

- a short summary of the problem or feature request
- steps to reproduce
- expected behavior vs actual behavior
- relevant files such as `src/emulator_parser.c`, `src/emulator_printer.c`, or `src/main.c`

## Code of conduct

Please be respectful, constructive, and inclusive.
This project welcomes contributors of all backgrounds and experience levels.

If you encounter behavior that does not respect these expectations, report it through the repository issue tracker.

