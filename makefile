####################################################################
# Makefile for running uC/OS on Linux Ubuntu 
#
# By lencioni@ita.br
#
# NOTES: 
#  1) originaly a port for uC/OS-III to run on Linux Ubuntu
#  2) it does not run on MacOS! (I used just to check the makefile)
#
####################################################################

# Check if OS is Linux or Mac
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    C_PATH=/home/luizlencioni/Repository/uC-OS-III-ports
endif
ifeq ($(UNAME_S),Darwin)
    C_PATH=/Users/luizrlencioni/MestradoITA/msc_luiz-lencioni/code/Example-uC-OS-III
endif

# Source files list
C_SOURCE=\
$(C_PATH)/uCOS-III/os_cfg_app.c \
$(C_PATH)/uCOS-III/os_core.c \
$(C_PATH)/uCOS-III/os_dbg.c \
$(C_PATH)/uCOS-III/os_flag.c \
$(C_PATH)/uCOS-III/os_mem.c \
$(C_PATH)/uCOS-III/os_msg.c \
$(C_PATH)/uCOS-III/os_mutex.c \
$(C_PATH)/uCOS-III/os_prio.c \
$(C_PATH)/uCOS-III/os_q.c \
$(C_PATH)/uCOS-III/os_sem.c \
$(C_PATH)/uCOS-III/os_stat.c \
$(C_PATH)/uCOS-III/os_task.c \
$(C_PATH)/uCOS-III/os_tick.c \
$(C_PATH)/uCOS-III/os_time.c \
$(C_PATH)/uCOS-III/os_tmr.c \
$(C_PATH)/uCOS-III/os_var.c \
$(C_PATH)/Ports/GCC-Ubuntu/os_cpu_c.c \
$(C_PATH)/uC-LIB/lib_ascii.c \
$(C_PATH)/uC-LIB/lib_math.c \
$(C_PATH)/uC-LIB/lib_mem.c \
$(C_PATH)/uC-LIB/lib_str.c \
$(C_PATH)/Ports/GCC-Ubuntu/uC-CPU/cpu_c.c \
$(C_PATH)/Ports/GCC-Ubuntu/uC-CPU/cpu_core.c \
$(C_PATH)/App/app.c \
$(C_PATH)/App/api-wrapper.c \
$(C_PATH)/App/posix-pthread.c \
$(C_PATH)/Ports/GCC-Ubuntu/os_app_hooks.c

# Header files directories
H_DIR=\
-I"$(C_PATH)/uCOS-III" \
-I"$(C_PATH)/Ports/GCC-Ubuntu" \
-I"$(C_PATH)/Ports/GCC-Ubuntu/uC-CPU" \
-I"$(C_PATH)/uC-LIB" \
-I"$(C_PATH)/Config" \
-I"$(C_PATH)/App"

# Object files (automatically generated from source files)
OBJ=$(C_SOURCE:.c=.o)

# POSIX libraries (to run on Linux)
LIBS=-lpthread

# Compiler options
CC_FLAGS=\
-O0			\
-g3			\
-Wall		\
-c			\
-fmessage-length=0

############################################################################
# Compilation and Linking
###########################################################################

# All Target
all: POSIX_OS-III

# PC target
POSIX_OS-III:	$(OBJ)
	@ echo 'Building binary using GCC: $@'
	gcc $^ -o $@ $(LIBS)
	@ echo 'Finished building binary: $@'
	@ echo ' '

# Building files	
%.o: %.c
	@echo 'Building file: $<'
	gcc $(H_DIR) -o $@ $< $(CC_FLAGS) $(LIBS)
	@echo ' '

# Other Targets
clean:
	rm -rf *.exe *.o *.out POSIX_OS-III

.PHONY: all clean

