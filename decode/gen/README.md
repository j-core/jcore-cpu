# decode/gen

This is a code generator that creates parts of the CPU's instruction
decoder.

It uses a java library called
[vMAGIC](http://sourceforge.net/projects/vmagic/) to generate VHDL
code. The generator itself is written in
[Clojure](http://clojure.org/), a lisp-like language that runs on the
JVM and thus can easily use vMAGIC.

## Inputs

The spreadsheet "SH-2 Instruction Set.ods" contains rows for each
instruction. Instructions that take multiple clock cycles to execute
have multiple rows. The data in the spreadsheet was derived from the
preexisting decode.v.

## Usage

To build and run the code generator, you need to install
[leiningen](http://leiningen.org/) and the JDK. With those installed
you should be able to run "lein run" in this directory and it will
download further dependencies, compile them and run the generator. The
generator reads the "SH-2 Instruction Set.ods" file in this directory
and overwrites the files decode_pkg.vhd, decode.vhd, decode_table.vhd
and decode_table_compressed.vhd in the cpu_core directory.

As an alternative, the generator and all its dependencies could be
packaged as a jar file, and then you should only need java installed
to run the generator.

## Decoder Style

Three types of decoders are generated. All types are generated when
the tool is run and the type that is actually used is chosen using
VHDL configurations (see decode/decode_config.vhd).

For the ROM-based decoder there is an additional option, --rom-width
or -w, which selects the width of the ROM holding the microcode. Only
the width 64 and 72 are supported right now and the default is 72.

lein run -- -w 64

## Regen Option

Pass the option -r when running to watch the ods file and regenerate
the decoder whenever the file is saved using the rest of the options.

lein run -- -r