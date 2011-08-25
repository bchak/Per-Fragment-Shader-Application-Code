#ifndef FILEREADER_H
#define FILEREADER_H

// Function will read a text file into allocated char buffer
GLchar* filetobuf(const char *file)
{
    FILE *fptr;
    long length;
    GLchar *buf;

    fptr = fopen(file, "r"); // Open file for reading
    if (!fptr)
        return NULL;
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file
    length = ftell(fptr); // Find out how many bytes into the file we are
    buf = static_cast<GLchar*>(malloc(length+1)); // Allocate a buffer
    fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
    fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
    fclose(fptr); // Close the file
    buf[length] = 0;

    return buf;
}

#endif
