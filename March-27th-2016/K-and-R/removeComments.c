#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>

/* K&R C (Dennis M. Ritchie & Brian W. Kernighan)
 Exercise 1-23. Write a program to remove all comments from a C program. Don't forget to
 handle quoted strings and character constants properly. C comments don't nest.
*/

#define BUFFER_INIT_CAPACITY    128
#define COMMENT_TAG_PAIRS        2

int FileExists(const char* path) 
{
    if (!path) {
        return 0;
    }
    FILE* file = fopen(path, "r");
    if (!file) {
        return 0;
    }
    return 1;
}

int ClearTagsInFile(FILE* from,
    FILE* to,
    const char* startTag, 
    const char* endTag) 
{
    if (!from        ||
        !to            ||
        !startTag    || 
        !endTag) 
    {
        errno = EINVAL;
        return 1;
    }    
    
    const size_t COMMENT_START_TAG_LEN = strlen(startTag);
    const size_t COMMENT_END_TAG_LEN = strlen(endTag);

    size_t bufferCapacity = BUFFER_INIT_CAPACITY;
    char* buffer = (char*)malloc(sizeof(char) * BUFFER_INIT_CAPACITY);
    if (!buffer) {
        errno = ENOMEM;
        return 1;
    }

    size_t bufferCount = 0;
    int isEOF = 0;
    int inComment = 0;

    char c = 0;

    while (!isEOF)
    {
        bufferCount = 0;

        while (1) {        
            c = fgetc(from);
            if (c == EOF) {
                isEOF = 1;
                break;
            }
            if (bufferCount >= bufferCapacity - 1) 
            {
                // a safeguard in case someone accidentally changes bufferCapacity to 0
                bufferCapacity = (bufferCapacity > 0) ? bufferCapacity * 2 : BUFFER_INIT_CAPACITY;
                buffer = (char*)realloc(buffer, bufferCapacity);
                if (!buffer) {
                    errno = ENOMEM;
                    return 2;
                }
            }
            buffer[bufferCount] = c;
            ++bufferCount;
            
            if (c == '\n') break;
        }
        buffer[bufferCount] = 0;
        
        size_t i = 0;
        while (i < bufferCount) {
            if (inComment && 
                i <= bufferCount - COMMENT_END_TAG_LEN &&
                (strncmp(&buffer[i], endTag, COMMENT_END_TAG_LEN) == 0)) 
            {
                i += COMMENT_END_TAG_LEN;
                inComment = 0;
                fputc('\n', to);
            }
            else if (!inComment &&
                      i <= bufferCount - COMMENT_START_TAG_LEN &&
                      (strncmp(&buffer[i], startTag, COMMENT_START_TAG_LEN) == 0))
            {
                inComment = 1;
                i += COMMENT_START_TAG_LEN;
            }
            else {
                if (!inComment) fputc(buffer[i], to);
                ++i;
            }
        }
    }

    free(buffer);
    buffer = NULL;

    rewind(to);
    rewind(from);
    
    return 0;
}


int main(int argc, const char** argv) {

    if (argc < 3) {
        fprintf(stderr, "Usage: ./removeComments source destination.\n");
        return 1;
    }

    FILE* from = fopen(argv[1], "r");
    if (!from) {
        fprintf(stderr, "Error opening file \"%s\"", argv[1]);
        return 2;
    }

    if (FileExists(argv[2])) {
        fprintf(stderr, "Destination file with the same name already exists in the directory.\n");
        return 3;
    }

    FILE* to = fopen(argv[2], "w");
    if (!to) {
        fprintf(stderr, "Error creating a destination file.\n");
        return 4;
    }
    
    FILE* tmpf = tmpfile();
    if (!tmpf) {
        fprintf(stderr, "Error creating a temporary file.\n");
        return 5;
    }
    
    ClearTagsInFile(from, tmpf, "/*", "*/");
    ClearTagsInFile(tmpf, to, "//", "\n");

    fclose(tmpf);
    fclose(from);
    fclose(to);

    return 0;
} 
