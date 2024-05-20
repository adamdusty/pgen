# Template Spec

## Format

Required to be valid yaml file

## Sections

- Templated variables (optional if no entries expect templated variables)
- Directories (optional)
- Files (optional)

### Templated Variables

- Identified as `variables`
- List of objects containing the following fields which are interpreted as strings:
  - identifier
  - display_name (optional)
  - description (optional)
  - default_value (optional)

A templated variable is invalid if:

- It shares an identifier with another templated variable
- Its identifier contains any of the following characters`@` `{` `}`

### Directories

- Identified as `directories`
- List of strings to be interpreted as relative paths

A directory entry is invalid if:

- It contains any invalid path characters
- It contains any templated `variables` not defined in the variable section

### Files

- Identified as `files`
- List of objects containing the following string values:
  - path
  - content (optional)

A file entry is invalid if:

- Either path or content field are not strings
- The path field contains any invalid path characters
- If either the path or content field contains any templated variables not defined in the `variables` section
