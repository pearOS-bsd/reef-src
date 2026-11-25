// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-3-Clause
//
// Files, directories, links, attributes and memory.
#include "darwin.h"

static int byName(const void* a, const void* b) { return strcmp(*(char* const*)a, *(char* const*)b); }

int main(int argc, char** argv)
{
    char path[1024], other[1024], buffer[256];
    if (argc < 2)
        return 2;
    printf("%d arguments, %s\n", argc, strrchr(argv[0], '/') + 1);
    CHECK(chdir(argv[1]));
    CHECK(mkdir("d", 0755));
    CHECK(mkdir("d", 0755));
    CHECK(open("d/missing", O_RDONLY));
    int fd = open("d/file", O_RDWR | O_CREAT | O_EXCL, 0644);
    printf("created %d\n", fd >= 0);
    CHECK(open("d/file", O_RDWR | O_CREAT | O_EXCL, 0644));
    CHECK(write(fd, "hello world\n", 12));
    CHECK(lseek(fd, 0, 1));
    CHECK(lseek(fd, -6, 2));
    CHECK(read(fd, buffer, sizeof buffer));
    CHECK(pread(fd, buffer, 5, 0));
    CHECK(pwrite(fd, "HELLO", 5, 0));
    CHECK(ftruncate(fd, 8));
    struct stat status;
    CHECK(fstat(fd, &status));
    printf("mode %o size %lld links %d uid %d blocksize %d\n", status.st_mode, status.st_size, status.st_nlink, status.st_uid == getuid(), status.st_blksize);
    CHECK(fcntl(fd, 3) & 3);
    CHECK(fcntl(fd, 1));
    CHECK(fcntl(fd, 2, 1));
    CHECK(fcntl(fd, 1));
    CHECK(close(fd));
    CHECK(close(fd));
    CHECK(symlink("file", "d/link"));
    CHECK(readlink("d/link", buffer, sizeof buffer));
    CHECK(readlink("d/file", buffer, sizeof buffer));
    CHECK(lstat("d/link", &status));
    printf("link mode %o\n", status.st_mode & 0170000);
    CHECK(link("d/file", "d/hard"));
    CHECK(stat("d/hard", &status));
    printf("links %d\n", status.st_nlink);
    CHECK(open("d/link", O_RDONLY | O_NOFOLLOW));
    CHECK(open("d/file", O_RDONLY | O_DIRECTORY));
    CHECK(rename("d/hard", "d/renamed"));
    CHECK(access("d/renamed", 4));
    CHECK(access("d/renamed", 1));
    CHECK(chmod("d/renamed", 0500));
    CHECK(stat("d/renamed", &status));
    printf("mode %o\n", status.st_mode);
    CHECK(rmdir("d"));
    CHECK(unlink("d"));
    char* names[64];
    int count = 0;
    void* directory = opendir("d");
    for (struct dirent* entry; (entry = readdir(directory));) {
        char* name = malloc(entry->d_namlen + 8);
        snprintf(name, entry->d_namlen + 8, "%s %d", entry->d_name, entry->d_type);
        names[count++] = name;
    }
    CHECK(closedir(directory));
    qsort(names, count, sizeof names[0], byName);
    for (int i = 0; i < count; i++)
        printf("entry %s\n", names[i]);
    printf("cwd is the directory: %d\n", strcmp(getcwd(path, sizeof path), argv[1]) == 0);
    printf("realpath %d\n", realpath("d/link", path) != NULL && strcmp(strrchr(path, '/'), "/file") == 0);
    printf("realpath missing %d %d\n", realpath("d/none", path) == NULL, errno);
    CHECK(setxattr("d/file", "com.example.test", "value", 5, 0, 0));
    CHECK(getxattr("d/file", "com.example.test", buffer, sizeof buffer, 0, 0));
    CHECK(getxattr("d/file", "com.example.none", buffer, sizeof buffer, 0, 0));
    CHECK(listxattr("d/file", buffer, sizeof buffer, 0));
    CHECK(removexattr("d/file", "com.example.test", 0));
    CHECK(pathconf("d", 4));
    CHECK(pathconf("d", 5));
    for (int name = 0; name < 210; name++) {
        errno = 0;
        long value = sysconf(name);
        if (name != 2 && name != 5 && name != 200 && (value >= 0 || errno))
            printf("sysconf %d = %ld %d\n", name, value < 0 ? -1 : value, value < 0 ? errno : 0);
    }
    char* memory = mmap(NULL, 65536, 3, 0x1002, -1, 0);
    printf("mmap %d\n", memory != MAP_FAILED);
    memory[0] = 1;
    memory[65535] = 2;
    CHECK(mprotect(memory, 4096, 1));
    CHECK(munmap(memory, 65536));
    fd = open("d/file", O_RDONLY);
    memory = mmap(NULL, 4096, 1, 2, fd, 0);
    printf("file mapping %.8s\n", memory);
    CHECK(munmap(memory, 4096));
    int pipes[2];
    CHECK(pipe(pipes));
    CHECK(write(pipes[1], "abc", 3));
    CHECK(read(pipes[0], buffer, sizeof buffer));
    int copy = dup(pipes[1]);
    CHECK(copy >= 0);
    CHECK(dup2(copy, 60));
    CHECK(write(60, "xy", 2));
    CHECK(read(pipes[0], buffer, sizeof buffer));
    CHECK(isatty(pipes[0]));
    snprintf(other, sizeof other, "%s/d", argv[1]);
    CHECK(chdir(other));
    CHECK(unlink("file"));
    CHECK(unlink("link"));
    CHECK(unlink("renamed"));
    CHECK(chdir(".."));
    CHECK(rmdir("d"));
    return 0;
}
