# PGEN

Personal utility for generating projects.

## Todo

- [ ] Validate user variables on template generation from directory (var names can't have filesystem illegal characters)
- [ ] Validate template on read
- [ ] Error checking/handling during rendering
- [ ] Option to remove directory if generation fails (can't be default in case directory already exists)
- [x] Add support to generate template from existing directory (Make creating template easier hopefully)
- [ ] Use templates from network locations
- [ ] Data for variable descriptions/documentation in template
- [x] Use proper rendering engine for rendering templates with dynamic variables
- [ ] Add support back for toml (Support both toml and json?)

## Build & install from source

Requirements:

```cmake
cmake --preset release
cmake --build build/release
cmake --install build/release
```

## Usage

Use the gen subcommand to generate a project at a destination from a project template.

`pgen gen destination template_path`  
`pgen gen ~/dev/new_proj ~/pgen/templates/cpp-exe.json`

Use the fd subcommand to generate a project template from a directory. You'll be prompted to declare the variables expected in the template.
The default output is to stdout, but can be redirected to create a file via the `-o|--output` option.

`pgen fd directory (-o|--output) output_path`  
`pgen fd ~/dev/proj_templ_dir -o ~/pgen/templates/template.json`  
`pgen fd ~/dev/proj_templ_dir --output ~/pgen/templates/template.json`
