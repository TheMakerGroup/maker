# This branch is deprecated.
>
> This branch is deprecated and **no longer maintained**.
>
> DO NOT use code here for any usage!

## Notice
- No new commits or pull requests will be accepted.
- This branch will be stored as deprecated/v1.2-dev.
- New branch v1.2-dev will replace it.

## Reason
Code about libuv is **hard to maintain**.

## Next Step
The new code will use **boost.process** to replace libuv.

Code here is only used to store.

### This is a tool to build by yaml.  

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