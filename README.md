[![Build](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/build.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/build.yaml)
[![Linter](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/linter.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/linter.yaml)
[![Unit Tests](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/unit-tests.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/unit-tests.yaml) 
[![Unit Tests with Valgrind](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/unit-tests-with-valgrind.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/unit-tests-with-valgrind.yaml)

# C Tic Tac Toe

A Tic Tac Toe game written in C. Includes a modular architecture, build system, unit tests, logging utilities, and Support for multiple input methods (e.g., keyboard) and display types (e.g., CLI). The game supports multiple players and non-standard board sizes. For example, with 5 players, the board size automatically adjusts to 6x6.

1. [Getting Started](#Getting-Started)
2. [Prerequisites](#Prerequisites)
3. [Building](#Building)
4. [Tests](#Tests)
5. [Pipelines](#Pipelines)
6. [Common Tasks](#Common-Tasks)
7. [Environment Variables](#Environment-Variables)
8. [Authors](#Authors)
9. [License](#License)

## Getting Started

Clone the repository:

```
git clone https://github.com/KubaTaba1uga/c_tic_tac_toe.git
```

Build the project and run:

```
cd c_tic_tac_toe
meson setup build
meson compile -C build
```

Run the Tic Tac Toe game:

```
./build/main
```

## Prerequisites

- [Meson](https://mesonbuild.com/)
- [Ruby](https://www.ruby-lang.org/en/)
- [Python](https://www.python.org/)

Unity is used as the testing framework. Python is required by Meson.

## Building

Install prerequisites using the provided scripts:

```
bash scripts/install_ruby.sh
bash scripts/install_meson.sh
```

Configure and build the project:

```
meson setup build
meson compile -C build
```

## Tests

Run all unit tests:

```
meson test -C build
```

Run tests with Valgrind for memory leak checks:

```
bash scripts/run_valgrind_tests.sh
```

## Pipelines

The following CI pipelines are available:

- `build.yaml` compiles the project.
- `linter.yaml` ensures code conforms to .clang-format.
- `unit-tests.yaml` runs unit tests.
- `unit-tests-with-valgrind.yaml` checks for memory leaks in tests.
- `merge-gatekeeper.yaml` ensures all pipelines pass before merging pull requests.

## Common Tasks

Set up a build:

```
bash scripts/setup_build.sh
```

Compile the project:

```
bash scripts/compile_build.sh
```

Format the code:

```
bash scripts/format_code.sh
```

Run tests:

```
bash scripts/run_tests.sh
```

## Environment Variables

The following environment variables can be used to configure the game:

- `users_amount`: Specifies the number of users participating in the game. Default is 2.
- `display`: Defines the display type to use (e.g., `cli` for command-line interface). Default is `cli`.
- `input`: Specifies the input method (e.g., `keyboard`). Default is `keyboard`.

The game supports non-standard configurations. The board size dynamically adjusts based on the number of players:

- For 5 players, the board size is 6x6.

Set these variables before running the game to customize the configuration.

Example:

```
export users_amount=5
export display=cli
export input=keyboard
./build/main
```

## Authors

- **Jakub Buczyński** - *C Tic Tac Toe* - [KubaTaba1uga](https://github.com/KubaTaba1uga)

## License

This project is licensed under the [MIT](LICENSE.md) License - see the [LICENSE.md](LICENSE.md) file for details

