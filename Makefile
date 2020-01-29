GCC=gcc
GCCOPTS= -W -g  -DGNU_SOURCE
CLANG=/opt/wasi-sdk/bin/clang
CLANGOPTS= --target=wasm32-unknown-wasi -Wl,--export-all --sysroot /opt/wasi-sdk/share/wasi-sysroot/  -O3 -s

all:plips
plips: linenoise.c plips.c plips_val.c plips_reader.c encodings/utf8.c
	$(GCC) $(GCCOPTS) -o plips linenoise.c plips.c plips_val.c plips_reader.c encodings/utf8.c -lpcre -lczmq

# termios.h missing in the WASI world :(
#plips.wasm: linenoise.c plips.c plips_reader.c encodings/utf8.c
#	$(CLANG) $(CLANGOPTS) -o plips.wasm linenoise.c plips.c plips_reader.c encodings/utf8.c -lpcre -lczmq

clean:
	rm -f plips
