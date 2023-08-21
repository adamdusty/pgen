# PGEN

Personal utility for generating projects.

## Todo

- [x] Error checking/handling during template reading
- [x] Error checking/handling during rendering
- [x] Option to remove directory if generation fails
- [ ] Add support to generate template from existing directory (Make creating template easier hopefully)
- [ ] Add support to read files with variable definitions instead of entering them one by one in terminal
- [ ] Download/use templates from github
  - Decide where to store templates and how executable should know about them
- [ ] Data for variable descriptions/documentation in template
- [x] Add support for proper rendering with some functionality for dynamic templates
  - Currently if you want an uppercase and lowercase version of the same variable the user has to enter both. It would be better if the template designer could just designate a variable and manipulate in the template.
- [x] Add support for post and pre render commands
- [x] Add clear warning when enabling commands

## Build & install from source

Requirements:

- [fmt](https://github.com/fmtlib/fmt) 10.0.0
- [argprage](https://github.com/p-ranav/argparse) master
- [tomlplusplus](https://github.com/marzer/tomlplusplus) master

```cmake
cmake --preset release
cmake --build build
cmake --install build/release
```

## Usage

`pgen destination [-t|--template] "template_path" (-c|--commands)`  
`pgen ~/dev/new_proj -t ~/pgen/templates/cpp-exe.toml`

Use the -c or --commands flag to enable any commands defined in the template. Don't enable commands for templates you don't trust or understand. Potentially harmful.

`pgen ~/dev/new_proj -t ~/pgen/templates/cpp-exe.toml -c`  
`pgen ~/dev/new_proj -t ~/pgen/templates/cpp-exe.toml --commands`
