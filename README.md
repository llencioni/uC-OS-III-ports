# uC/OS-III ports to LPC21xx and Ubuntu

uC/OS-III (version V3.08.01), ports to:
1) Linux Ubuntu (using POSIX)
2) LPC2129 compiled with Keil toolchain (BSP for MCB2000 board)

Besides, a POSIX API can be compiled to the Application Software:
>> make posix

Or, a regular uC/OS-III to the Application Software:
>> make

>> make all

----------------
1) Linux Ubuntu
----------------

1) Select (PORT_UC_III = LINUX) in app.c

2) Open a shell

3) $ make clean

4) $ make all (regular uC/OS-III API to the application SW) or make posix (POSIX API to the application SW)

5) Before running check if RTPRIO is set to infinity, through "ulimit -r". The answer should be "unlimited"

6) If not, set RTPRIO to "unlimited":

$ sudo bash

$ ulimit -r unlimited

$ ulimit -r

7) Run: "$ ./POSIX_OS-III"


------------------------
2) LPC2129 compiled with Keil toolchain (BSP for MCB2000 board)
------------------------

* just regular uC/OS-III to the application SW

1) Select (PORT_UC_III = LPC2129) in app.c

2) Open Keil uV4 to compile and download into HW MCB2000


