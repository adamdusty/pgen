# CLI Usage

`pgen <subcommand> <args>`

## Subcommands

### gen

`pgen gen <destination> --template=<template-path> [<options>]`

| Option        | Description                                                                         |
| ------------- | ----------------------------------------------------------------------------------- |
| --template    | path to template file                                                               |
| --definitions | path to variable definition file (if not provided, `gen` will use interactive mode) |

Generates a directory from the template file at the `<destination>` path. If specified, template variable definitions will be read from the definition file. The definition file is expected to be in a yaml format with `key: value` pairs to identify a value for each template variable identifier.

### fd

`pgen fd <root> --output=<output-path> [<options>]`

| Option            | Description                                                    |
| ----------------- | -------------------------------------------------------------- |
| -o, --output      | path to save template file at                                  |
| -i, --interactive | interactively add template variables after template generation |

Generates a template file based on a directory. Recursively reads directories and files starting at `<root>`. Template variables can be defined and passed in a separate file, and also defined interactively after the directory has been serialized by passing the `--interactive` flag.
