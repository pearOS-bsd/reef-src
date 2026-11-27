// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
//
// libsqlite3: a database in memory through the host's library: statements, bound values, the printf family and the variadic
// configuration calls (their arguments are copied and passed on).
#include <stdio.h>
#include <stdarg.h>

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef struct sqlite3_str sqlite3_str;
int sqlite3_open(const char *, sqlite3 **);
int sqlite3_close(sqlite3 *);
int sqlite3_exec(sqlite3 *, const char *, int (*)(void *, int, char **, char **), void *, char **);
int sqlite3_prepare_v2(sqlite3 *, const char *, int, sqlite3_stmt **, const char **);
int sqlite3_bind_int(sqlite3_stmt *, int, int);
int sqlite3_bind_text(sqlite3_stmt *, int, const char *, int, void (*)(void *));
int sqlite3_step(sqlite3_stmt *);
int sqlite3_column_int(sqlite3_stmt *, int);
const unsigned char *sqlite3_column_text(sqlite3_stmt *, int);
int sqlite3_finalize(sqlite3_stmt *);
const char *sqlite3_libversion(void);
char *sqlite3_mprintf(const char *, ...);
char *sqlite3_vmprintf(const char *, va_list);
char *sqlite3_snprintf(int, char *, const char *, ...);
char *sqlite3_vsnprintf(int, char *, const char *, va_list);
sqlite3_str *sqlite3_str_new(sqlite3 *);
void sqlite3_str_appendf(sqlite3_str *, const char *, ...);
char *sqlite3_str_finish(sqlite3_str *);
int sqlite3_config(int, ...);
int sqlite3_db_config(sqlite3 *, int, ...);
int sqlite3_test_control(int, ...);
void sqlite3_log(int, const char *, ...);
void sqlite3_free(void *);

static char *viaVmprintf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    char *s = sqlite3_vmprintf(format, ap);
    va_end(ap);
    return s;
}

static int row(void *unused, int count, char **values, char **names)
{
    printf("row %d: %s=%s %s=%s\n", count, names[0], values[0], names[1], values[1]);
    return 0;
}

int main(void)
{
    printf("version nonempty %d\n", sqlite3_libversion()[0] != 0);
    sqlite3 *db = 0;
    printf("open %d\n", sqlite3_open(":memory:", &db));
    printf("exec %d\n", sqlite3_exec(db, "create table t(a integer, b text); insert into t values (1, 'one'), (2, 'two');", 0, 0, 0));
    printf("select %d\n", sqlite3_exec(db, "select a, b from t order by a", row, 0, 0));
    sqlite3_stmt *st = 0;
    printf("prepare %d\n", sqlite3_prepare_v2(db, "insert into t values (?, ?)", -1, &st, 0));
    sqlite3_bind_int(st, 1, 3);
    sqlite3_bind_text(st, 2, "three", -1, 0);
    printf("step %d\n", sqlite3_step(st));
    sqlite3_finalize(st);
    sqlite3_prepare_v2(db, "select b from t where a = 3", -1, &st, 0);
    printf("step %d text %s\n", sqlite3_step(st), sqlite3_column_text(st, 0));
    sqlite3_finalize(st);

    char *m = sqlite3_mprintf("%d-%s-%q-%05.1f", 42, "x", "it's", 3.14159);
    printf("mprintf %s\n", m);
    sqlite3_free(m);
    m = viaVmprintf("%s/%d/%lld", "v", 7, 1234567890123LL);
    printf("vmprintf %s\n", m);
    sqlite3_free(m);
    char buf[16];
    sqlite3_snprintf(sizeof buf, buf, "%s %d", "truncated-long-text", 5);
    printf("snprintf %s\n", buf);
    sqlite3_str *str = sqlite3_str_new(db);
    sqlite3_str_appendf(str, "%d+%d", 1, 2);
    sqlite3_str_appendf(str, "=%s", "three");
    m = sqlite3_str_finish(str);
    printf("str %s\n", m);
    sqlite3_free(m);

    printf("db_config %d\n", sqlite3_db_config(db, 1002, 1, (int *)0));     // SQLITE_DBCONFIG_ENABLE_FKEY
    printf("test_control %d\n", sqlite3_test_control(5, 0));
    sqlite3_log(1, "logged %d %s", 1, "message");
    printf("config after init %d\n", sqlite3_config(12) != 0);              // SQLITE_CONFIG_MULTITHREAD, refused: the library is initialised
    printf("close %d\n", sqlite3_close(db));
    return 0;
}
