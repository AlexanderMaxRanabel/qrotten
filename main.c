#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <tar.h>

#define BLOCK_SIZE 512

int main(int argc, char *argv[])
{
    int tarfile;
    char buffer[BLOCK_SIZE];
    struct stat sb;
    int size;
    struct tar_header *header;

    if (argc != 2) {
        printf("Usage: qrotten <filename.tar>\n");
        return 1;
    }

    tarfile = open(argv[1], O_RDONLY);
    if (tarfile == -1) {
        perror("Error opening file");
        return 1;
    }

    while (read(tarfile, buffer, BLOCK_SIZE) == BLOCK_SIZE) {
        header = (struct tar_header *) buffer;
        size = (int) strtol(header->size, NULL, 8);
        if (size > 0) {
            stat(header->name, &sb);
            if (!S_ISDIR(sb.st_mode)) {
                int outfile = open(header->name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (outfile == -1) {
                    perror("Error opening file");
                    return 1;
                }

                int remain = size;
                while (remain > 0) {
                    int block = (remain < BLOCK_SIZE) ? remain : BLOCK_SIZE;
                    read(tarfile, buffer, block);
                    write(outfile, buffer, block);
                    remain -= block;
                }
                close(outfile);
            }
        }
    }

    close(tarfile);
    return 0;
}
