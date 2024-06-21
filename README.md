# pgen

A project generation utility

## Documentation

[cli](doc/cli.md)  
[templates](docs/template.md)

## Build/Install

```shell
cmake --preset release
cmake --build --preset release
cmake --install build/release
```

## Usage

```shell
pgen gen --template <template-path> [--definitions <defs-path>] <destination>
pgen fd --output <output-path> [-i, --interactive] <root>
```

## Commands

### gen

```pgen gen --template <template-path> [--definitions <defs-path>] <destination>```

Generates a project at directory `destination` from template at `template-path`.
If `--definitions` is not passed, the utility will prompt the user for variable definitions before generating the project.
If a path is passed via `--definitions`, the utility will attempt to parse variable defintions from the file.
The variable definitions file is expected to be a simple yaml file where keys match variable identifiers from the template.

### fd

```pgen fd --output <output-path> [-i, --interactive] <root>```

Generates a template file at `output-path` from the directory `root`. If the `--interactive` flag is passed
the utility will prompt the user for variable information before creating the template. Otherwise, the template is
created with no variables.
