#include <dsl/parser/parser.h>
#include <dsl/utils.h>
#include <base/memory.h>
#include <stdio.h>
#include <string.h>

long get_filesize(FILE *fp) {
    long fsize = 0;
    if (fp == NULL) {
        return 0;
    }
    fseek(fp, 0L, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return fsize;
}

char *get_codebuf_from_filepath(const char *filepath) {
    FILE *fp = NULL;
    char *codebuf = NULL;
    long fsize = 0;
    if (filepath == NULL) {
        return NULL;
    }
    fp = fopen(filepath, "rb");
    if (fp == NULL) {
        return NULL;
    }
    fsize = get_filesize(fp);
    if (fsize == 0) {
        fclose(fp);
        return NULL;
    }
    codebuf = (char *) getseg(fsize + 1);
    memset(codebuf, 0, fsize + 1);
    if (fread(codebuf, 1, fsize, fp) == -1) {
        free(codebuf);
        fclose(fp);
    }
    fclose(fp);
    return codebuf;
}

const char *get_next_tlp_command(const char *codebuf) {
    const char *codebuf_end = NULL;
    if (codebuf == NULL) {
        return NULL;
    }
    codebuf_end = codebuf + strlen(codebuf);
    if (codebuf == codebuf_end) {
        return NULL;
    }
    while (codebuf != codebuf_end && !is_note(*codebuf) &&
                                     !is_technique_tag_announce(*codebuf) &&
                                     !is_sep(*codebuf) &&
                                     !is_hammeron(*codebuf) &&
                                     !is_pulloff(*codebuf) &&
                                     !is_vibrato(*codebuf) &&
                                     !is_slidedown(*codebuf) &&
                                     !is_slideup(*codebuf)) {
        codebuf++;
    }
    return codebuf;
}

const char *get_next_tlp_technique_block(const char *codebuf) {
    return NULL;
}
