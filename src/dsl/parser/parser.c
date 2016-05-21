#include <dsl/parser/parser.h>
#include <dsl/utils.h>
#include <base/memory.h>
#include <stdio.h>
#include <string.h>

static int g_curr_line_nr = 0;

void set_curr_code_line_number(const int line_nr) {
    g_curr_line_nr = line_nr;
}

int get_curr_code_line_number() {
    return g_curr_line_nr;
}

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
                                     !is_note_sep(*codebuf) &&
                                     !is_sep_bar(*codebuf) &&
                                     !is_save_point(*codebuf) &&
                                     !is_technique_block_end(*codebuf)) {
        if (*codebuf == '\n') {
            g_curr_line_nr++;
        }
        codebuf++;
    }
    return codebuf;
}

const char *get_next_tlp_technique_block_begin(const char *codebuf) {
    const char *cp = codebuf;
    const char *cp_end = NULL;
    if (cp == NULL) {
        return NULL;
    }
    cp_end = cp + strlen(cp);
    while (cp != cp_end && !is_technique_block_begin(*cp)) {
        cp++;
    }
    return (cp != cp_end) ? cp : NULL;
}

const char *get_next_tlp_technique_block_end(const char *codebuf) {
    const char *cp = get_next_tlp_technique_block_begin(codebuf);
    const char *cp_end = NULL;
    int o = 1, temp = 0;
    if (cp == NULL || !is_technique_block_begin(*cp)) {
        return NULL;
    }
    cp_end = cp + strlen(cp);
    cp++;
    while (cp != cp_end && o > 0) {
        if (is_technique_block_begin(*cp)) {
            o++;
        } else if (is_technique_block_end(*cp)) {
            o--;
            if (o == 0) {
                continue;
            }
        } else if (is_string_delim(*cp)) {
            temp = get_curr_code_line_number(); // INFO(Santiago): dirty trick prologue.
            cp = skip_string_chunk(cp);
            if (cp == NULL) {
                return NULL;
            }
            if (is_technique_block_end(*cp)) {
                o--;
            }
            set_curr_code_line_number(temp); // INFO(Santiago): dirty trick epilogue.
            continue;
        }
        cp++;
    }
    return (cp != cp_end) ? cp : NULL;
}

const char *skip_string_chunk(const char *codebuf) {
    const char *cp = codebuf;
    const char *cp_end = NULL;
    if (cp == NULL) {
        return NULL;
    }
    if (!is_string_delim(*cp)) {
        return cp;
    }
    cp_end = cp + strlen(cp);
    cp++;
    while (cp != cp_end && !is_string_delim(*cp)) {
        if (is_string_escaper(*cp)) {
            cp++;
        }
        if (*cp == '\n') {
            g_curr_line_nr++;
        }
        cp++;
    }
    if (cp != cp_end) {
        cp++;
    }
    return (cp != cp_end) ? cp : NULL;
}

size_t get_next_tlp_technique_block_size(const char *codebuf) {
    const char *begin = get_next_tlp_technique_block_begin(codebuf);
    const char *end = get_next_tlp_technique_block_end(codebuf);
    return (begin != NULL && end != NULL) ? end - begin : 0;
}
