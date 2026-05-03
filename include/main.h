#pragma once

#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstring>
#include <filesystem>

#include "yaml-cpp/yaml.h"
#include "print.h"
#include "get.h"
#include "execute.h"
#include "cmd.h"

#ifdef _WIN32
#include <cstring>
#include <Windows.h>
#define OS_WINDOWS 1
#include <clocale>
#else
#include <strings.h>
#define OS_POSIX 1
#include <errno.h>
#include <locale.h>
#endif

//#define DEBUG