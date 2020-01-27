
plips: linenoise.c plips.c plips_reader.c encodings/utf8.c
	$(CC) -Wall -W -Os -g -o plips linenoise.c plips.c plips_reader.c encodings/utf8.c -lpcre -lczmq

clean:
	rm -f plips
