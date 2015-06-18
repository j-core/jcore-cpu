This directory contains the C-VHDL connector.


To simulate the cpu running the test rom run

$ make
$ ./cpu_ctb --stop-time=180us

If you want to save the save file, add the option --wave=wave.ghw


The cpu_ctb can adjust the read and write ACK delay for specific
memory addresses. See delays.cfg for an example delay config file with
explanation.

Run cpu_ctb with the -d option to use a delay config file

$ ./cpu_ctb -d delays.cfg --stop-time=180us
