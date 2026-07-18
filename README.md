<div align="center">
<font size="6">
<b>
The Maker Tool
</b>
</font>
</div>

<font size="4">A YAML‑based build automation tool.</font>

> [!WARNING]
> This project is currently undergoing refactoring. Although backward compatibility is maintained, **please exercise caution** when using it.

---

## How to Use

1. Open your terminal – `cmd` or PowerShell on Windows, `bash`/`sh` on Linux.
2. Navigate to your source code directory.
3. Invoke the maker binary.
4. Specify the desired **action**.
5. Optionally, provide a **task name** (some actions do not require this step).

**Example:**

```cmd
maker.exe make default
```
```bash
maker make default
```

For details on writing the configuration file, refer to the [syntax guide](syntax-guide.md).

---

## Actions

| Action    | Description |
|-----------|-------------|
| `make`    | Build the project according to the tasks defined in `maker.yaml`. |
| `help`    | Display this help message. |
| `version` | Show version information. |

---

## Options

| Option                  | Description |
|-------------------------|-------------|
| `--force-legacy`        | Execute commands using the legacy mode. |

---

## Third‑Party Licenses

- **[yaml-cpp]** – MIT License (Copyright 2026 jbeder)  
  Source code: https://github.com/jbeder/yaml-cpp

---

## License

This project is published under the **GNU GPL v3 or later**.  
For alternative licensing terms (MIT), please see the `LICENSE-MIT` file.  
The full GNU GPL v3 license is available in the `LICENSE` file.