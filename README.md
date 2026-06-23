<div align="center">
<font size="6">
The maker tool
</font>
</div>


<font size="4">This is a tool to build by yaml.</font> 

>[!WARNING]
> This project is under refactoring. Although with backward compatibility,
 BE ATTENTION when using it.

# How to use?

First, open your cmd/powershell(Windows) or bash/sh(Linux) shell.

Then, enter your source code folder.

Then, input the maker's binary file name.

Add the action.

Finally, input the task.(Some actions needn't this step.)

Example:

```cmd
maker.exe make default
```
```bash
maker make default
```

About how to write the config file, you can see [syntax guide](syntax-guide.md).

Actions:

-    make:         Make the project following the task in maker.yaml.

-    help:         Show this help.

-    version:      Show version info.

Option(s):

-    --force-legacy:      Using legacy mode to execute command.



## Third-Party Licenses  
- **[yaml-cpp]**: MIT License (Copyright 2025 jbeder) 
Source code at https://github.com/jbeder/yaml-cpp

This project is published as GNU GPL v3 or later.
MIT license, see LICENSE-MIT,  
GNU GPL v3 license, see LICENSE.