#include <uv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


uv_loop_t* loop = nullptr;
uv_pipe_t cmd_pipe;
uv_process_t child_proc;
