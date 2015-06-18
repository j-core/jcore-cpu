This directory is meant to hold tests that rely on the simulator as
opposed to tests contained in the testrom.

Right now there is only a test for the interrupts, and running it
involves running make in both the cpusim and this directory and then
running

$ ./cpu_ctb --stop-time=10us --wave=wave.ghw -i tests/interrupts.img

in the cpusim directory. The output should either end in "Test Passed"
or repeatedly print "Test failed. Result=N" where N is a number that
was written to r9 in the test to help track down what failed.

In future I'd like to both expand the tests to testing individual
parts of the cpu in isolation, for example the shifter, and automate
the tests by outputting TAP or some other testing protocol that
jenkins can read and report.


Have now added a test that tries to replicate the stack saving and
restoring of a kernel space syscall.

./cpu_ctb --stop-time=10us --wave=wave.ghw -i tests/rte.img
