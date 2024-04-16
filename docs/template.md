# Template Spec

## Format

Required to be valid yaml file

## Sections

- Templated variables
- Directories (optional)
- Files (optional)

### Templated Variables

- Identified as `variables`
- List of objects containing the following string values:
  - identifier
  - display_name (optional)
  - description (optional)
  - default_value (optional)

A templated variable is invalid if:

- It shares an identifier with another templated variable
- Its identifier contains any of the following characters`@` `{` `}`
- Any of the field values are not strings

### Directories

- Identified as `directories`
- Allows templated variables
- List of strings to be interpreted as relative paths

A directory entry is invalid if:

- It is not a string
- It contains any invalid path characters
- It contains any templated `variables` not defined in the variable section

### Files

- Identified as `files`
- Allows templated variables
- List of objects containing the following string values:
  - path
  - content

A file entry is invalid if:

- Either path or content field are not strings
- The path field contains any invalid path characters
- If either the path or content field contains any templated variables not defined in the `variables` section
