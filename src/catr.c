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

int validate_params(long start, long end, long fileSize) {
    // end == LONG_MAX means that the end position was not provided by the user
    // we continue and set it to the end of the file
    if (end == LONG_MAX) {
        end = fileSize;
    }

    if (end > fileSize) {
        end = fileSize;
    }

    if (start <= 0 || end < 0 || start > fileSize || !is_start_before_end(start, end)) {
        return 0;
    }
    return 1;
}

void catr_by_chars(const char *filename, long start, long end, char **outputBuffer, size_t *outputSize) {
    *outputBuffer = NULL;
    *outputSize = 0;

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("error opening file");
        return;
    }

    long fileSize = get_file_size(file);
    if (fileSize < 0) {
        fprintf(stderr, "error obtaining file size.\n");
        fclose(file);
        return;
    }

    if (!validate_params(start, end, fileSize)) {
        fprintf(stderr, "start char must be before end char.\n");
        fclose(file);
        return;
    }

    long length = end - start + 1;
    *outputBuffer = (char *) malloc(length + 1);
    if (!*outputBuffer) {
        fprintf(stderr, "memory allocation failed.\n");
        fclose(file);
        return;
    }

    fseek(file, start - 1, SEEK_SET);
    *outputSize = fread(*outputBuffer, 1, length, file);
    if (*outputSize < (size_t) length) {
        fprintf(stderr, "read less data than expected.\n");
    }
    (*outputBuffer)[*outputSize] = '\0';

    fclose(file);
}

void catr_by_lines(const char *filename, long startLine, long endLine, char **outputBuffer, size_t *outputSize) {
    *outputBuffer = NULL;
    *outputSize = 0;

    if (!is_start_before_end(startLine, endLine)) {
        fprintf(stderr, "start line must be before end line.\n");
        return;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("error opening file");
        return;
    }

    size_t bufferLength = 256;
    *outputBuffer = (char *) malloc(bufferLength);
    if (!*outputBuffer) {
        fprintf(stderr, "memory allocation failed.\n");
        fclose(file);
        return;
    }

    size_t currentSize = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    long currentLine = 1;

    while ((read = getline(&line, &len, file)) != -1) {
        if (currentLine >= startLine && (endLine == -1 || currentLine <= endLine)) {
            while (currentSize + read >= bufferLength) {
                bufferLength += len;
                char *tempBuffer = (char *) realloc(*outputBuffer, bufferLength);
                if (!tempBuffer) {
                    fprintf(stderr, "memory reallocation failed.\n");
                    free(line);
                    fclose(file);
                    free(*outputBuffer);
                    *outputBuffer = NULL;
                    return;
                }
                *outputBuffer = tempBuffer;
            }
            memcpy(*outputBuffer + currentSize, line, read);
            currentSize += read;
        }
        if (currentLine > endLine && endLine != -1) break;
        currentLine++;
    }
    free(line);
    fclose(file);

    *outputSize = currentSize;
    if (*outputSize > 0) {
        (*outputBuffer)[*outputSize] = '\0';
    } else {
        free(*outputBuffer);
        *outputBuffer = NULL;
    }
}

void print_usage(const char *programName) {
    fprintf(stderr, "Usage: %s <file> <start> <length>\n", programName);
    fprintf(stderr, "       %s -s <file> <start> <end>\n", programName);
    fprintf(stderr, "       %s -l <file> <start line> <length>\n", programName);
    fprintf(stderr, "       %s -sl <file> <start line> <end line>\n", programName);
}

int parse_args(int argc, char *argv[], const char **filename, long *start, long *end, int *mode) {
    char *endptr;

    if (argc == 1 || (argc <= 2 && ((strcmp(argv[1], "-h") == 0) || strcmp(argv[1], "--help") == 0))) {
        print_usage(argv[0]);
        return 0;
    }

    int argOffset = 1;
    *mode = CHAR_LENGTH_MODE;

    *filename = argv[argOffset];

    if (argc <= 2) {
        *start = 1;
        *end = LONG_MAX;
        return 1;
    }

    if (strcmp(argv[argOffset + 1], "-s") == 0) {
        *mode = CHAR_RANGE_MODE;
        argOffset += 1;
    } else if (strcmp(argv[argOffset + 1], "-l") == 0) {
        *mode = LINE_LENGTH_MODE;
        argOffset += 1;
    } else if (strcmp(argv[argOffset + 1], "-sl") == 0) {
        *mode = LINE_RANGE_MODE;
        argOffset += 1;
    } else {
        // unknown mode that is not the default mode
        if (argc > argOffset + 1 && argv[argOffset + 1][0] == '-') {
            fprintf(stderr, "unknown mode.\n");
            return 0;
        }
    }

    *start = 1; // default start value
    if (argc > argOffset + 1) {
        *start = strtol(argv[argOffset + 1], &endptr, 10);
        if (*endptr != '\0' || *start <= 0) {
            fprintf(stderr, "invalid start value.\n");
            return 0;
        }
    }

    *end = LONG_MAX; // default end value
    if (argc > argOffset + 2) {
        *end = strtol(argv[argOffset + 2], &endptr, 10);
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
    size_t outputSize = 0;

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("error opening file");
        return;
    }

    long fileSize = get_file_size(file);
    if (fileSize < 0) {
        fprintf(stderr, "error obtaining file size.\n");
        fclose(file);
        return;
    }

    if (end == LONG_MAX) {
        end = fileSize;
    }

    fclose(file);

    if (mode == CHAR_LENGTH_MODE || mode == CHAR_RANGE_MODE) {
        catr_by_chars(filename, start, end, &output, &outputSize);
    } else if (mode == LINE_LENGTH_MODE || mode == LINE_RANGE_MODE) {
        catr_by_lines(filename, start, end, &output, &outputSize);
    }

    if (output) {
        if (outputSize > 0 && output[outputSize - 1] != '\n') {
            char *newOutput = realloc(output, outputSize + 2); // +2 for '\n' and '\0'
            if (newOutput) {
                output = newOutput;
                output[outputSize] = '\n';
                output[outputSize + 1] = '\0';
                outputSize++;
            } else {
                fprintf(stderr, "memory reallocation failed.\n");
                free(output);
                return;
            }
        }

        fwrite(output, 1, outputSize, stdout);
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