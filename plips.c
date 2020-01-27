#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plips_reader.h"
#include "linenoise.h"
#include "encodings/utf8.h"


int plips_eval(char *command){
    reader_str(command);
    return 0;
}
void completion(const char *buf, linenoiseCompletions *lc) {
    if (buf[0] == '(') {

        linenoiseAddCompletion(lc,"(define symbol form)");
        linenoiseAddCompletion(lc,"(define symbol '(1 2))");
        linenoiseAddCompletion(lc,"hello こんにちは 👨‍💻");
    }
}


char *hints(const char *buf, int *color, int *bold) {
    if (!strcasecmp(buf,"(define ")) {
        *color = 35;
        *bold = 0;
        return " symbol form)";
    }
    if (!strcasecmp(buf,"(display")) {
        *color = 35;
        *bold = 0;
        return " fmt &data)";
    }
    return NULL;
}
int main(int argc, char **argv) {
    char *line;
    char *prgname = argv[0];

    while(argc > 1) {
        argc--;
        argv++;
        if (!strcmp(*argv,"--multiline")) {
            linenoiseSetMultiLine(1);
            printf("Multi-line mode enabled.\n");
        } else if (!strcmp(*argv,"--keycodes")) {
            linenoisePrintKeyCodes();
            exit(0);
        } else {
            fprintf(stderr, "Usage: %s [--multiline] [--keycodes]\n", prgname);
            exit(1);
        }
    }


    linenoiseSetEncodingFunctions(
        linenoiseUtf8PrevCharLen,
        linenoiseUtf8NextCharLen,
        linenoiseUtf8ReadCode);


    /* Set the completion callback. This will be called every time the
     * user uses the <tab> key. */
    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);

    /* Load history from file. The history file is just a plain text file
     * where entries are separated by newlines. */
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

    /* Now this is the main loop of the typical linenoise-based application.
     * The call to linenoise() will block as long as the user types something
     * and presses enter.
     *
     * The typed string is returned as a malloc() allocated string by
     * linenoise, so the user needs to free() it. */

    while((line = linenoise("\033[32mPLIPS\x1b[0m> ")) != NULL) {
        /* Do something with the string. */
        if (line[0] != '\0' && line[0] != '/') {
            //printf("echo: '%s'\n", line);
            plips_eval(line);
            linenoiseHistoryAdd(line); /* Add to the history. */
            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        } else if (!strncmp(line,"/historylen",11)) {
            /* The "/historylen" command will change the history len. */
            int len = atoi(line+11);
            linenoiseHistorySetMaxLen(len);
        } else if (line[0] == '/') {
            printf("Unreconized command: %s\n", line);
        }
        free(line);
    }
    return 0;
}