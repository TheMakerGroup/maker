### This is a tool to build by yaml.  
> This project uses Linux kernel–style versioning: 
  minor updates for regular changes, 
  major updates for legacy removal and compatibility breaks.
   
Sometimes the reason of major update is just because the 

big number of minor update.

The minor updates might include breaking changes,

but you can still use old style in future versions,

until a random update.

But the release note will show it.

## How to use?
<br>

First, open your cmd/powershell(Windows) or bash/sh(Linux) shell.
<br>
Then, enter your source code floder.
<br>
Then, input the maker's binary file name.
<br>
Add the action.
<br>
Finally, input the task.(Some actions needn't this step.)
<br>
Example:
<br>
```cmd
maker.exe make default
```
```bash
maker make default
```
<br>

About how to write the config file, you can see example at example/test.yml.
<br>
Actions:
- make : The action to make.
- -h   : Get help.
- -v   : Get version.
<br>

## Third-Party Licenses  
- **[yaml-cpp]**: MIT License (Copyright 2025 jbeder) 
Source code at https://github.com/jbeder/yaml-cpp
<br>
This project is published as GNU GPL v3 or later.
MIT license, see LICENSE-MIT,  
GNU GPL v3 license, see LICENSE.