[![Build](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/build.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/build.yaml)
[![Linter](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/linter.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/linter.yaml)
[![Unit Tests](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/unit-tests.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/unit-tests.yaml) 
[![Unit Tests with Valgrind](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/unit-tests-with-valgrind.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_tic_tac_toe/actions/workflows/unit-tests-with-valgrind.yaml)

# C Tic Tac Toe

A Tic Tac Toe game written in C. Includes a modular architecture, build system, unit tests, logging utilities, and support for multiple user inputs and displays.

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

Unity is used as the testing framework.
Python is required by Meson.

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

Set these variables before running the game to customize the configuration.

Example:
```
export users_amount=3
export display=cli
export input=keyboard
./build/main
```

## Authors

  - **Jakub Buczyński** - *C Tic Tac Toe* -
    [KubaTaba1uga](https://github.com/KubaTaba1uga)

## License

This project is licensed under the [MIT](LICENSE.md)
License - see the [LICENSE.md](LICENSE.md) file for
details


# C Project Template

Template for new c projects. Includes build system, testing, mocking, logging, readme, license, pipelines. 

1. [Getting Started](#Getting-Started)
2. [Prerequisites](#Prerequisites)
3. [Building](#Building)
4. [Tests](#Tests)
5. [Pipelines](#Pipelines)
6. [Common tasks](#Common-tasks)
7. [Authors](#Authors)
8. [License](#License)


## Getting Started

Clone project
```
git clone https://github.com/KubaTaba1uga/c_tic_tac_toe.git
```

Delete template git files
```
rm -rf c_tic_tac_toe/.git
```

Initiate new git files
```
cd c_tic_tac_toe
git init
```

Once you have initialized the repository, create a remote repository somewhere like GitHub.com.

Add the remote URL to your local git repository
```
git remote add origin <URL>
```

Commit at least one file

Push changes
```
git push -u origin master
```

I recommend installing perequesitest and confirming that all tests are passing before src/*, test/* are actually deleted.

Project's name or version can be changed in ./meson.build file.


## Prerequisites

- [Meson](https://mesonbuild.com/)
- [Ruby](https://www.ruby-lang.org/en/)
- [Python](https://www.python.org/)

Ruby is used by CMock.
Python is used by Meson.


## Building

Install perequesites by scripts
```
bash scripts/install_ruby.sh
bash scripts/install_meson.sh
```

If installing perequesites by scripts has failed, You need to install them manually.


Configure a build for an app and tests
```
meson setup build
```

Compile a build
```
meson compile -C build
```


## Tests

Run all tests
```
meson test -C build
```


### Style test

New tests units should be grouped by directories.
To understand more look on ./test directory.

Hierarchial build is very favoured by Meson, splitting test unit's into seperate directories allow keeping meson.builds short and simple.


## Pipelines

Some pipelines are configured out of the box:
- `build.yaml` compiles the project
- `linter.yaml` makes sure code is inline with .clang-format
- `unit-tests.yaml` runs unit tests
- `unit-tests-with-valgrind.yaml` checks for memory leaks in tests
- `merge-gatekeeper.yaml` makes sure that all pipelines ran successfully before merging pull request


## Common tasks

Setting up a build
```
bash scripts/setup_build.sh
```

Compiling a build
```
bash scripts/compile_build.sh
```

Formatting a code
```
bash scripts/format_code.sh
```

Running tests
```
bash scripts/run_tests.sh
```

Running tests with valgrind
```
bash scripts/run_valgrind_tests.sh
```


## Authors

  - **Jakub Buczyński** - *C Project Template* -
    [KubaTaba1uga](https://github.com/KubaTaba1uga)

## License

This project is licensed under the [MIT](LICENSE.md)
License - see the [LICENSE.md](LICENSE.md) file for
details
