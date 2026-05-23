#include <uv.h>
#include <stdbool.h>

typedef struct shell_s {
    uv_loop_t* loop;
    uv_process_t process;
    uv_pipe_t stdin_pipe;
    bool exited;
    int64_t exit_status;
    int close_pending;
} shell_t;

#ifdef __cplusplus
extern "C"{
#endif
    shell_t* start_shell(uv_loop_t* loop);
    bool shell_exec(shell_t* shell, const char* command);
    void kill_shell(shell_t* shell);
#ifdef __cplusplus
    }
#endif