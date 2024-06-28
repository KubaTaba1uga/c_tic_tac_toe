[![Build](https://github.com/KubaTaba1uga/c_project_template/actions/workflows/build.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_project_template/actions/workflows/build.yaml)
[![Linter](https://github.com/KubaTaba1uga/c_project_template/actions/workflows/linter.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_project_template/actions/workflows/linter.yaml)
[![Unit Tests](https://github.com/KubaTaba1uga/c_project_template/actions/workflows/unit-tests.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_project_template/actions/workflows/unit-tests.yaml) 
[![Unit Tests with Valgrind](https://github.com/KubaTaba1uga/c_project_template/actions/workflows/unit-tests-with-valgrind.yaml/badge.svg)](https://github.com/KubaTaba1uga/c_project_template/actions/workflows/unit-tests-with-valgrind.yaml)

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
git clone https://github.com/KubaTaba1uga/c_project_template.git
```

Delete template git files
```
rm -rf c_project_template/.git
```

Initiate new git files
```
cd c_project_template
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
