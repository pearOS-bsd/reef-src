// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libxar: write archives (every compression and checksum), read them back, walk, properties, attributes, extraction, sub documents, damaged archives.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct __xar *xar_t; typedef struct __xar_file *xar_file_t; typedef struct __xar_iter *xar_iter_t; typedef struct __xar_subdoc *xar_subdoc_t;
xar_t xar_open(const char *, int); int xar_close(xar_t);
int xar_opt_set(xar_t, const char *, const char *); const char *xar_opt_get(xar_t, const char *);
xar_file_t xar_add_frombuffer(xar_t, xar_file_t, const char *, const char *, size_t);
xar_file_t xar_add_frompath(xar_t, xar_file_t, const char *, const char *); xar_file_t xar_add(xar_t, const char *);
xar_iter_t xar_iter_new(void); void xar_iter_free(xar_iter_t); xar_file_t xar_file_first(xar_t, xar_iter_t); xar_file_t xar_file_next(xar_iter_t);
int xar_prop_get(xar_file_t, const char *, const char **); int xar_prop_set(xar_file_t, const char *, const char *);
int xar_prop_create(xar_file_t, const char *, const char *); void xar_prop_unset(xar_file_t, const char *);
const char *xar_attr_get(xar_file_t, const char *, const char *); int xar_attr_set(xar_file_t, const char *, const char *, const char *);
const char *xar_get_path(xar_file_t); const char *xar_get_safe_path(xar_file_t);
const char *xar_get_type(xar_t, xar_file_t); unsigned long long xar_get_size(xar_t, xar_file_t); unsigned short xar_get_mode(xar_t, xar_file_t);
int xar_extract_tobuffersz(xar_t, xar_file_t, char **, size_t *); int xar_extract_tobuffer(xar_t, xar_file_t, char **);
int xar_extract_tofile(xar_t, xar_file_t, const char *); int xar_extract(xar_t, xar_file_t); int xar_verify(xar_t, xar_file_t);
xar_subdoc_t xar_subdoc_new(xar_t, const char *); xar_subdoc_t xar_subdoc_first(xar_t); xar_subdoc_t xar_subdoc_next(xar_subdoc_t);
const char *xar_subdoc_name(xar_subdoc_t); int xar_subdoc_prop_set(xar_subdoc_t, const char *, const char *);
int xar_subdoc_prop_get(xar_subdoc_t, const char *, const char **); int xar_subdoc_attr_set(xar_subdoc_t, const char *, const char *, const char *);
const char *xar_subdoc_attr_get(xar_subdoc_t, const char *, const char *); void xar_subdoc_remove(xar_subdoc_t);

static const char *s(const char *p) { return p ? p : "(null)"; }

static void list(const char *archive, const char *label)
{
    xar_t x = xar_open(archive, 0);
    printf("%s: open %s\n", label, x ? "ok" : "failed");
    if (!x)
        return;
    xar_iter_t i = xar_iter_new();
    for (xar_file_t f = xar_file_first(x, i); f; f = xar_file_next(i)) {
        const char *id = xar_attr_get(f, NULL, "id"), *uid = NULL, *encoding = xar_attr_get(f, "data/encoding", "style");
        xar_prop_get(f, "uid", &uid);
        char *b = NULL; size_t n = 0;
        int r = xar_extract_tobuffersz(x, f, &b, &n);
        printf("  id=%s path=%s safe=%s type=%s size=%llu mode=%o uid=%s enc=%s extract=%d len=%zu verify=%d", s(id), s(xar_get_path(f)), s(xar_get_safe_path(f)), s(xar_get_type(x, f)),
               xar_get_size(x, f), xar_get_mode(x, f), s(uid), s(encoding), r, n, xar_verify(x, f));
        if (r == 0 && n < 40) printf(" data=[%.*s]", (int)n, b);
        const char *note = NULL, *tag = NULL, *gone = NULL;
        if (xar_prop_get(f, "note", &note) == 0) printf(" note=[%s] lang=%s", s(note), s(xar_attr_get(f, "note", "lang")));
        if (xar_prop_get(f, "tag", &tag) == 0) printf(" tag=%s", s(tag));
        printf(" temp=%d\n", xar_prop_get(f, "temp", &gone));
        free(b);
    }
    xar_iter_free(i);
    printf("  subdocs:");
    for (xar_subdoc_t d = xar_subdoc_first(x); d; d = xar_subdoc_next(d)) {
        const char *v = NULL;
        xar_subdoc_prop_get(d, "a/b", &v);
        printf(" %s(a/b=%s, k=%s)", s(xar_subdoc_name(d)), s(v), s(xar_subdoc_attr_get(d, "a", "k")));
    }
    printf("\n  close=%d\n", xar_close(x));
}

static void writeArchive(const char *archive, const char *compression, const char *checksum, const char *fileChecksum, const char *dir)
{
    char path[1024], target[1024], big[8000];
    for (int k = 0; k < 8000; k++) big[k] = "abcdefgh ijkl\n"[k % 14];
    xar_t x = xar_open(archive, 1);
    printf("write %s %s %s: %s\n", compression, checksum, fileChecksum, x ? "ok" : "failed");
    xar_opt_set(x, "compression", compression); xar_opt_set(x, "toc-cksum", checksum); xar_opt_set(x, "file-chksum", fileChecksum);
    printf("  options %s %s %s %s\n", s(xar_opt_get(x, "compression")), s(xar_opt_get(x, "toc-cksum")), s(xar_opt_get(x, "file-chksum")), s(xar_opt_get(x, "nothing")));
    xar_file_t d = xar_add_frombuffer(x, NULL, "docs", NULL, 0);
    xar_file_t a = xar_add_frombuffer(x, d, "a & b <c>.txt", big, sizeof big);
    xar_add_frombuffer(x, d, "small", "hello", 5);
    xar_add_frombuffer(x, NULL, "empty", "", 0);
    xar_add_frombuffer(x, NULL, "../evil/..", "x", 1);
    snprintf(path, sizeof path, "%s/source.txt", dir);
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0640);
    write(fd, "from a path\n", 12); close(fd);
    xar_add(x, path);
    snprintf(target, sizeof target, "%s/link", dir);
    symlink("source.txt", target);
    xar_add_frompath(x, d, "link", target);
    xar_add_frompath(x, NULL, "folder", dir);
    xar_add_frompath(x, NULL, "missing", "/nonexistent/path");
    xar_prop_set(a, "note", "a \"quoted\" <note> & more");
    xar_prop_create(a, "tag", "one"); xar_prop_create(a, "tag", "two");
    xar_attr_set(a, "note", "lang", "en");
    xar_prop_set(a, "temp", "gone"); xar_prop_unset(a, "temp");
    xar_subdoc_t sd = xar_subdoc_new(x, "first");
    xar_subdoc_prop_set(sd, "a/b", "text & <markup>");
    xar_subdoc_attr_set(sd, "a", "k", "v'\"");
    xar_subdoc_t gone = xar_subdoc_new(x, "second");
    xar_subdoc_new(x, "third");
    xar_subdoc_remove(gone);
    printf("  close=%d\n", xar_close(x));
}

int main(int argc, char **argv)
{
    const char *dir = argv[1];
    char archive[1024];
    static const char *kinds[][3] = {{"gzip", "sha1", "sha1"}, {"gzip", "md5", "md5"}, {"bzip2", "sha256", "sha256"}, {"xz", "sha512", "sha512"},
                                     {"lzma", "none", "none"}, {"none", "sha1", "none"}, {"gzip", "unknown", "unknown"}};
    for (int k = 0; k < 7; k++) {
        snprintf(archive, sizeof archive, "%s/a%d.xar", dir, k);
        writeArchive(archive, kinds[k][0], kinds[k][1], kinds[k][2], dir);
        list(archive, kinds[k][0]);
    }
    // extraction
    snprintf(archive, sizeof archive, "%s/a0.xar", dir);
    xar_t x = xar_open(archive, 0);
    char out[1024];
    snprintf(out, sizeof out, "%s/out", dir); mkdir(out, 0755);
    chdir(out);
    xar_iter_t i = xar_iter_new();
    for (xar_file_t f = xar_file_first(x, i); f; f = xar_file_next(i)) printf("extract %s: %d\n", s(xar_get_path(f)), xar_extract(x, f));
    xar_iter_free(i);
    xar_file_t f = xar_file_first(x, xar_iter_new());
    snprintf(out, sizeof out, "%s/single", dir);
    printf("tofile %d\n", xar_extract_tofile(x, f, out));
    xar_close(x);
    char cmd[2048];
    snprintf(cmd, sizeof cmd, "cd %s/out && ls -lR | sed 's/^\\([-dl][rwx-]*\\) *[0-9]* [^ ]* *[^ ]* */\\1 /;s/[A-Z][a-z][a-z] *[0-9]* *[0-9:]* //' | sort; cat docs/small; echo; cat source.txt", dir);
    fflush(stdout); system(cmd);
    // damaged archives
    static const long places[] = {0, 4, 8, 16, 40, 60, 100, 200};
    for (int k = 0; k < 8; k++) {
        snprintf(cmd, sizeof cmd, "cp %s/a0.xar %s/bad.xar", dir, dir); system(cmd);
        int fd = open(strcat(strcpy(out, dir), "/bad.xar"), O_RDWR);
        lseek(fd, places[k], SEEK_SET); write(fd, "\xff\xfe", 2); close(fd);
        snprintf(archive, sizeof archive, "bad %ld", places[k]);
        list(out, archive);
    }
    snprintf(cmd, sizeof cmd, "cp %s/a0.xar %s/short.xar; truncate -s 30 %s/short.xar", dir, dir, dir); system(cmd);
    list(strcat(strcpy(out, dir), "/short.xar"), "short");
    list("/nonexistent.xar", "missing");
    snprintf(cmd, sizeof cmd, "cp %s/a0.xar %s/tail.xar; truncate -s -50 %s/tail.xar", dir, dir, dir); system(cmd);
    list(strcat(strcpy(out, dir), "/tail.xar"), "tail");
    printf("null: %p %d %p\n", (void *)xar_open(NULL, 0), xar_close(NULL), (void *)xar_add(NULL, "x"));
    return 0;
}
