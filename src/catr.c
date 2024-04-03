#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "modes.h"

long getFileSize(FILE *file) {
    if (!file) {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    return size;
}

int validateParameters(long start, long end, long fileSize) {
    // end == LONG_MAX means that the end position was not provided by the user
    // we continue and set it to the end of the file
    if (end == LONG_MAX) {
        end = fileSize;
    }

    if (start <= 0 || end < 0 || start > fileSize || end > fileSize || end < start) {
        return 0;
    }
    return 1;
}

void catrByChars(const char *filename, long start, long end, char **outputBuffer, size_t *outputSize) {
    *outputBuffer = NULL;
    *outputSize = 0;

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    long fileSize = getFileSize(file);
    if (fileSize < 0) {
        fprintf(stderr, "Error obtaining file size.\n");
        fclose(file);
        return;
    }

    if (!validateParameters(start, end, fileSize)) {
        fprintf(stderr, "Invalid start or end position %ld - %ld.\n", start, end);
        fclose(file);
        return;
    }

    long length = end - start + 1;
    *outputBuffer = (char *) malloc(length + 1);
    if (!*outputBuffer) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return;
    }

    fseek(file, start - 1, SEEK_SET);
    *outputSize = fread(*outputBuffer, 1, length, file);
    if (*outputSize < (size_t) length) {
        fprintf(stderr, "Read less data than expected.\n");
    }
    (*outputBuffer)[*outputSize] = '\0';

    fclose(file);
}

void catrByLines(const char *filename, long startLine, long endLine, char **outputBuffer, size_t *outputSize) {
    *outputBuffer = NULL;
    *outputSize = 0;

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    size_t bufferLength = 256;
    *outputBuffer = (char *) malloc(bufferLength);
    if (!*outputBuffer) {
        fprintf(stderr, "Memory allocation failed.\n");
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
                    fprintf(stderr, "Memory reallocation failed.\n");
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

void printUsage(const char *programName) {
    fprintf(stderr, "Usage: %s <file> <start> <length>\n", programName);
    fprintf(stderr, "       %s -s <file> <start> <end>\n", programName);
    fprintf(stderr, "       %s -l <file> <start line> <length>\n", programName);
    fprintf(stderr, "       %s -sl <file> <start line> <end line>\n", programName);
}

int parseArguments(int argc, char *argv[], const char **filename, long *start, long *end, int *mode) {
    char *endptr;

    if (argc < 3) {
        printUsage(argv[0]);
        return 0;
    }

    int argOffset = 0;
    *mode = CHAR_LENGTH_MODE;

    if (strcmp(argv[1], "-s") == 0) {
        *mode = CHAR_RANGE_MODE;
        argOffset = 1;
    } else if (strcmp(argv[1], "-l") == 0) {
        *mode = LINE_LENGTH_MODE;
        argOffset = 1;
    } else if (strcmp(argv[1], "-sl") == 0) {
        *mode = LINE_RANGE_MODE;
        argOffset = 1;
    }

    *filename = argv[1 + argOffset];

    *start = strtol(argv[2 + argOffset], &endptr, 10);
    if (*endptr != '\0' || *start <= 0) {
        fprintf(stderr, "Invalid start value.\n");
        return 0;
    }

    *end = LONG_MAX;
    if (argc > 3 + argOffset) {
        *end = strtol(argv[3 + argOffset], &endptr, 10);
        if (*endptr != '\0' || *end < 0) {
            fprintf(stderr, "Invalid end value.\n");
            return 0;
        }
    }

    if ((*end != LONG_MAX && *mode == CHAR_LENGTH_MODE) || *mode == LINE_LENGTH_MODE) {
        *end += *start - 1;
    }

    return 1;
}


void extract(const char *filename, long start, long end, int mode) {
    char *output = NULL;
    size_t outputSize = 0;

    if (mode == CHAR_LENGTH_MODE || mode == CHAR_RANGE_MODE) {
        catrByChars(filename, start, end, &output, &outputSize);
    } else if (mode == LINE_LENGTH_MODE || mode == LINE_RANGE_MODE) {
        catrByLines(filename, start, end, &output, &outputSize);
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
                fprintf(stderr, "Memory reallocation failed.\n");
                free(output);
                return;
            }
        }

        fwrite(output, 1, outputSize, stdout);
        free(output);
    } else {
        fprintf(stderr, "No output generated.\n");
    }
}


int main(int argc, char *argv[]) {
    const char *filename;
    long start, end;
    int mode = 0;

    if (!parseArguments(argc, argv, &filename, &start, &end, &mode)) {
        return 1;
    }

    extract(filename, start, end, mode);
    return 0;
}