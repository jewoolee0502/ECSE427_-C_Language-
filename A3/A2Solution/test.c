#include "pcb.h"
#include "interpreter.h"
#include "shell.h"
#include "shellmemory.h"
#include "kernel.h"
#include "cpu.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int x = FRAMESIZE;
int y = VARMEMSIZE;