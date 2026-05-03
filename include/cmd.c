#include <uv.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cmd.h"

static void on_shell_exit(uv_process_t* process, int64_t exit_status, int term_signal) {
    shell_t* shell = (shell_t*)process->data;
    shell->exited = true;
    shell->exit_status = exit_status;
    (void)term_signal; // to avoid warning
}

static void on_write(uv_write_t* req, int status) {
    free(req->data);
    free(req);
    (void)status; // to avoid warning
}

// inner function, do not edit
static void on_close(uv_handle_t* handle) {
    shell_t* shell = (shell_t*)handle->data;
    shell->close_pending--;
}

shell_t* start_shell(uv_loop_t* loop) {
    if (!loop) return NULL;

    shell_t* shell = (shell_t*)malloc(sizeof(shell_t));
    if (!shell) return NULL;
    memset(shell, 0, sizeof(shell_t));
    shell->loop = loop;
    shell->exited = false;
    shell->close_pending = 0;

    int r = uv_pipe_init(loop, &shell->stdin_pipe, 0);
    if (r != 0) {
        free(shell);
        return NULL;
    }

    // init stdio
    uv_stdio_container_t stdio[3];
    stdio[0].flags = UV_CREATE_PIPE | UV_WRITABLE_PIPE;
    stdio[0].data.stream = (uv_stream_t*)&shell->stdin_pipe;
    stdio[1].flags = UV_INHERIT_FD;
    stdio[1].data.fd = 1;
    stdio[2].flags = UV_INHERIT_FD;
    stdio[2].data.fd = 2;

    uv_process_options_t options = { 0 };
    options.exit_cb = on_shell_exit;
    options.stdio = stdio;
    options.stdio_count = 3;

#ifdef _WIN32
    static char* win_args[] = { "cmd.exe", "/q", "/d", "/k", "prompt $",NULL};
    options.file = "cmd.exe";
    options.args = win_args;
    options.flags = UV_PROCESS_WINDOWS_HIDE;
#else
    static char* nix_args[] = { "/bin/bash", "-s", NULL };
    options.file = "/bin/bash";
    options.args = nix_args;
#endif

    r = uv_spawn(loop, &shell->process, &options);
    if (r != 0) {
        uv_close((uv_handle_t*)&shell->stdin_pipe, NULL);
        free(shell);
        return NULL;
    }
    shell->process.data = shell;

    return shell;
}

bool shell_exec(shell_t* shell, const char* command) {
    if (!shell) return true;

    if (shell->exited) {
        kill_shell(shell);
        return true;
    }

    // add extra line
#ifdef _WIN32
    // here judge if failed, not show unexpected new line and add CRLF
    const char* newline = "||exit & set /p=<nul\r\n";
#else
    // same in Windows
    const char* newline = "||exit; echo -n\n";
#endif
    unsigned int cmd_len = (unsigned int)strlen(command);
    unsigned int newline_len = (unsigned int)strlen(newline);
    char* full_cmd = (char*)malloc(cmd_len + newline_len + 1);
    if (!full_cmd) return false;

    memcpy(full_cmd, command, cmd_len);
    memcpy(full_cmd + cmd_len, newline, newline_len);
    full_cmd[cmd_len + newline_len] = '\0';

    // prepare write to shell
    uv_write_t* write_req = (uv_write_t*)malloc(sizeof(uv_write_t));
    if (!write_req) {
        free(full_cmd);
        return false;
    }
    write_req->data = full_cmd; // save pointer

    uv_buf_t buf = uv_buf_init(full_cmd, cmd_len + newline_len);

    int r = uv_write(write_req, (uv_stream_t*)&shell->stdin_pipe, &buf, 1, on_write);
    if (r != 0) {
        free(full_cmd);
        free(write_req);
        return false;
    }

    while (uv_loop_alive(shell->loop)) {
        uv_run(shell->loop, UV_RUN_ONCE);

        // clean mess when shell exit
        if (shell->exited) {
            kill_shell(shell);
            return true;
        }
        break;
    }

    if (shell->exited) {
        kill_shell(shell);
        return true;
    }

    return false;
}

void kill_shell(shell_t* shell) {
    if (!shell) return;

    // close stdin pipe
    uv_handle_t* pipe_handle = (uv_handle_t*)&shell->stdin_pipe;
    if (!uv_is_closing(pipe_handle)) {
        pipe_handle->data = shell;
        shell->close_pending++;
        uv_close(pipe_handle, on_close);
    }

    // close handle
    uv_handle_t* proc_handle = (uv_handle_t*)&shell->process;
    if (!uv_is_closing(proc_handle)) {
        uv_process_kill(&shell->process, SIGTERM);
        proc_handle->data = shell;
        shell->close_pending++;
        uv_close(proc_handle, on_close);
    }

    while (shell->close_pending > 0) {
        uv_run(shell->loop, UV_RUN_ONCE);
    }

    free(shell);
}