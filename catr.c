#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
    if (start <= 0 || end < 0 || start > fileSize || end > fileSize || start > end) {
        fprintf(stderr, "Error: Invalid start or end parameters.\n");
        return 0;
    }
    return 1;
}

void catr(const char *filename, long start, long end, char **outputBuffer, size_t *outputSize) {
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

    if (start <= 0 || start > fileSize) {
        fprintf(stderr, "Start position out of bounds.\n");
        fclose(file);
        return;
    }

    if (end <= 0 || end > fileSize) {
        end = fileSize;
    }

    if (end < start) {
        fprintf(stderr, "End position is before the start position.\n");
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
    (*outputBuffer)[*outputSize] = '\0'; // ensuring buffer is null-terminated

    fclose(file);
}

void printUsage(const char *programName) {
    fprintf(stderr, "Usage: %s <file> <start> <length>\n", programName);
    fprintf(stderr, "       %s -s <file> <start> <end>\n", programName);
}

int parseArguments(int argc, char *argv[], const char **filename, long *start, long *end) {
    char *endptr;

    if (argc < 3) {
        printUsage(argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "-s") == 0) {
        // check for at least "<file> <start>" params
        if (argc < 4) {
            printUsage(argv[0]);
            return 0;
        }

        *filename = argv[2];
        errno = 0;
        *start = strtol(argv[3], &endptr, 10);
        if (errno != 0 || *endptr != '\0' || *start <= 0) {
            fprintf(stderr, "Invalid start value.\n");
            return 0;
        }

        if (argc == 5) {
            errno = 0;
            *end = strtol(argv[4], &endptr, 10);
            if (errno != 0 || *endptr != '\0' || *end < 0) {
                fprintf(stderr, "Invalid end value.\n");
                return 0;
            }
        } else {
            *end = -1; // read until end of file
        }
    } else {
        // default mode: <file> <start> [<length>]
        if (argc > 4) {
            printUsage(argv[0]);
            return 0;
        }

        *filename = argv[1];
        errno = 0;
        *start = strtol(argv[2], &endptr, 10);
        if (errno != 0 || *endptr != '\0' || *start <= 0) {
            fprintf(stderr, "Invalid start value.\n");
            return 0;
        }

        if (argc == 4) {
            long length = strtol(argv[3], &endptr, 10);
            if (errno != 0 || *endptr != '\0' || length < 0) {
                fprintf(stderr, "Invalid length value.\n");
                return 0;
            }
            *end = *start + length - 1;
        } else {
            *end = -1; // read until end of file
        }
    }
    return 1;
}


void extract(const char *filename, long start, long end) {
    if (!filename || start < 1) {
        fprintf(stderr, "Invalid input parameters.\n");
        return;
    }

    char *output = NULL;
    size_t outputSize = 0;
    // if end is -1, pass 0 to catr -> read until end of file
    catr(filename, start, end == -1 ? 0 : end, &output, &outputSize);

    if (output) {
        if (outputSize > 0) {
            printf("%s", output);
            if (output[outputSize - 1] != '\n') {
                printf("\n");
            }
        } else {
            fprintf(stderr, "The extracted data is empty.\n");
        }
        free(output);
    } else {
        fprintf(stderr, "Failed to extract data.\n");
    }
}

int main(int argc, char *argv[]) {
    const char *filename;
    long start, end;

    if (!parseArguments(argc, argv, &filename, &start, &end)) {
        return 1;
    }
    extract(filename, start, end);
    return 0;
}