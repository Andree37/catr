#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "modes.h"

long get_file_size(FILE *file) {
    if (!file) {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    return size;
}

int is_start_before_end(long start, long end) {
    return start <= end;
}

int validate_params(long start, long end, long file_size) {
    // end == LONG_MAX means that the end position was not provided by the user
    // we continue and set it to the end of the file
    if (end == LONG_MAX) {
        end = file_size;
    }

    if (end > file_size) {
        end = file_size;
    }

    if (start <= 0 || end < 0 || start > file_size || !is_start_before_end(start, end)) {
        return 0;
    }
    return 1;
}

void catr_by_chars(const char *filename, long start, long end, char **output_buffer, size_t *output_size) {
    *output_buffer = NULL;
    *output_size = 0;

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("error opening file");
        return;
    }

    long file_size = get_file_size(file);
    if (file_size < 0) {
        fprintf(stderr, "error obtaining file size.\n");
        fclose(file);
        return;
    }

    if (!validate_params(start, end, file_size)) {
        fprintf(stderr, "start char must be before end char.\n");
        fclose(file);
        return;
    }

    long length = end - start + 1;
    *output_buffer = (char *) malloc(length + 1);
    if (!*output_buffer) {
        fprintf(stderr, "memory allocation failed.\n");
        fclose(file);
        return;
    }

    fseek(file, start - 1, SEEK_SET);
    *output_size = fread(*output_buffer, 1, length, file);
    if (*output_size < (size_t) length) {
        fprintf(stderr, "read less data than expected.\n");
    }
    (*output_buffer)[*output_size] = '\0';

    fclose(file);
}

void catr_by_lines(const char *filename, long start_line, long end_line, char **output_buffer, size_t *output_size) {
    *output_buffer = NULL;
    *output_size = 0;

    if (!is_start_before_end(start_line, end_line)) {
        fprintf(stderr, "start line must be before end line.\n");
        return;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("error opening file");
        return;
    }

    size_t buffer_length = 256;
    *output_buffer = (char *) malloc(buffer_length);
    if (!*output_buffer) {
        fprintf(stderr, "memory allocation failed.\n");
        fclose(file);
        return;
    }

    size_t current_size = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    long current_line = 1;

    while ((read = getline(&line, &len, file)) != -1) {
        if (current_line >= start_line && (end_line == -1 || current_line <= end_line)) {
            while (current_size + read >= buffer_length) {
                buffer_length += len;
                char *tempBuffer = (char *) realloc(*output_buffer, buffer_length);
                if (!tempBuffer) {
                    fprintf(stderr, "memory reallocation failed.\n");
                    free(line);
                    fclose(file);
                    free(*output_buffer);
                    *output_buffer = NULL;
                    return;
                }
                *output_buffer = tempBuffer;
            }
            memcpy(*output_buffer + current_size, line, read);
            current_size += read;
        }
        if (current_line > end_line && end_line != -1) break;
        current_line++;
    }
    free(line);
    fclose(file);

    *output_size = current_size;
    if (*output_size > 0) {
        (*output_buffer)[*output_size] = '\0';
    } else {
        free(*output_buffer);
        *output_buffer = NULL;
    }
}

void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s <file> <start> <length>\n", program_name);
    fprintf(stderr, "       %s -r <file> <start> <end>\n", program_name);
    fprintf(stderr, "       %s -l <file> <start line> <length>\n", program_name);
    fprintf(stderr, "       %s -rl <file> <start line> <end line>\n", program_name);
}

int parse_args(int argc, char *argv[], const char **filename, long *start, long *end, int *mode) {
    char *endptr;

    if (argc == 1 || (argc <= 2 && ((strcmp(argv[1], "-h") == 0) || strcmp(argv[1], "--help") == 0))) {
        print_usage(argv[0]);
        return 0;
    }

    int arg_offset = 1;
    *mode = CHAR_LENGTH_MODE;

    *filename = argv[arg_offset];

    if (argc <= 2) {
        *start = 1;
        *end = LONG_MAX;
        return 1;
    }

    if (strcmp(argv[arg_offset + 1], "-r") == 0) {
        *mode = CHAR_RANGE_MODE;
        arg_offset += 1;
    } else if (strcmp(argv[arg_offset + 1], "-l") == 0) {
        *mode = LINE_LENGTH_MODE;
        arg_offset += 1;
    } else if (strcmp(argv[arg_offset + 1], "-rl") == 0) {
        *mode = LINE_RANGE_MODE;
        arg_offset += 1;
    } else {
        // unknown mode that is not the default mode
        if (argc > arg_offset + 1 && argv[arg_offset + 1][0] == '-') {
            fprintf(stderr, "unknown mode.\n");
            return 0;
        }
    }

    *start = 1; // default start value
    if (argc > arg_offset + 1) {
        *start = strtol(argv[arg_offset + 1], &endptr, 10);
        if (*endptr != '\0' || *start <= 0) {
            fprintf(stderr, "invalid start value.\n");
            return 0;
        }
    }

    *end = LONG_MAX; // default end value
    if (argc > arg_offset + 2) {
        *end = strtol(argv[arg_offset + 2], &endptr, 10);
        if (*endptr != '\0' || *end < 0) {
            fprintf(stderr, "invalid end value.\n");
            return 0;
        }
    }

    if ((*mode == CHAR_LENGTH_MODE || *mode == LINE_LENGTH_MODE) && *end != LONG_MAX) {
        *end += *start - 1;
    }

    return 1;
}

void extract(const char *filename, long start, long end, int mode) {
    char *output = NULL;
    size_t output_size = 0;

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("error opening file");
        return;
    }

    long file_size = get_file_size(file);
    if (file_size < 0) {
        fprintf(stderr, "error obtaining file size.\n");
        fclose(file);
        return;
    }

    if (end == LONG_MAX) {
        end = file_size;
    }

    fclose(file);

    if (mode == CHAR_LENGTH_MODE || mode == CHAR_RANGE_MODE) {
        catr_by_chars(filename, start, end, &output, &output_size);
    } else if (mode == LINE_LENGTH_MODE || mode == LINE_RANGE_MODE) {
        catr_by_lines(filename, start, end, &output, &output_size);
    }

    if (output) {
        if (output_size > 0 && output[output_size - 1] != '\n') {
            char *new_output = realloc(output, output_size + 2); // +2 for '\n' and '\0'
            if (new_output) {
                output = new_output;
                output[output_size] = '\n';
                output[output_size + 1] = '\0';
                output_size++;
            } else {
                fprintf(stderr, "memory reallocation failed.\n");
                free(output);
                return;
            }
        }

        fwrite(output, 1, output_size, stdout);
        free(output);
    }
}

int main(int argc, char *argv[]) {
    const char *filename;
    long start, end;
    int mode = 0;

    if (!parse_args(argc, argv, &filename, &start, &end, &mode)) {
        return 1;
    }

    extract(filename, start, end, mode);
    return 0;
}