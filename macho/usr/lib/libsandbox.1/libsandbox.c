// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// libsandbox: profiles are compiled and applied by the runtime (reef_sandbox_compile, reef_sandbox_apply in libSystem); this library keeps
// the parameter list, reads profile files and holds the compiled profile.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void *reef_sandbox_compile(const char *profile, char *const *parameters, size_t *size, char **error);
extern int reef_sandbox_apply(const void *compiled, size_t size, char **error);

// The parameters: key and value strings, one after the other, ended by a NULL.
typedef struct {
    char **items;
    size_t count, capacity;
} sandbox_params_t;

// A compiled profile.
typedef struct {
    void *reserved;
    void *compiled;
    size_t size;
} sandbox_profile_t;

sandbox_params_t *sandbox_create_params(void)
{
    return calloc(1, sizeof(sandbox_params_t));
}

int sandbox_set_param(sandbox_params_t *params, const char *key, const char *value)
{
    if (!params || !key || !value)
        return -1;
    char **items = params->items;
    if (params->count + 3 > params->capacity) {
        size_t capacity = params->capacity ? params->capacity * 2 : 16;
        items = realloc(items, capacity * sizeof(char *));
        if (!items)
            return -1;
        params->items = items;
        params->capacity = capacity;
    }
    items[params->count++] = strdup(key);
    params->items[params->count++] = strdup(value);
    params->items[params->count] = NULL;
    return 0;
}

void sandbox_free_params(sandbox_params_t *params)
{
    if (!params)
        return;
    for (size_t i = 0; i < params->count; i++)
        free(params->items[i]);
    free(params->items);
    free(params);
}

static sandbox_profile_t *compile(const char *text, const sandbox_params_t *params, char **error)
{
    sandbox_profile_t *profile = calloc(1, sizeof(sandbox_profile_t));
    if (!profile)
        return NULL;
    size_t size = 0;
    void *compiled = reef_sandbox_compile(text, params && params->count ? params->items : NULL, &size, error);
    if (!compiled) {
        free(profile);
        return NULL;
    }
    profile->compiled = compiled;
    profile->size = size;
    return profile;
}

sandbox_profile_t *sandbox_compile_string(const char *string, const sandbox_params_t *params, char **error)
{
    if (error)
        *error = NULL;
    return compile(string, params, error);
}

sandbox_profile_t *sandbox_compile_file(const char *path, const sandbox_params_t *params, char **error)
{
    FILE *file = path ? fopen(path, "r") : NULL;
    if (!file) {
        if (error)
            *error = strdup("cannot read the profile");
        return NULL;
    }
    char *text = malloc(0x1000);
    if (!text) {
        fclose(file);
        return NULL;
    }
    // Read everything: the buffer doubles whenever a read fills it up.
    size_t capacity = 0x1000, length = 0;
    char *end = text;
    size_t got = fread(end, 1, capacity - 1, file);
    while (got) {
        if (length + got + 1 == capacity) {
            capacity *= 2;
            char *larger = realloc(text, capacity);
            if (!larger) {
                free(text);
                fclose(file);
                return NULL;
            }
            text = larger;
        }
        length += got;
        end = text + length;
        got = fread(end, 1, capacity - length - 1, file);
    }
    fclose(file);
    *end = 0;
    if (error)
        *error = NULL;
    sandbox_profile_t *profile = compile(text, params, error);
    free(text);
    return profile;
}

sandbox_profile_t *sandbox_compile_named(const char *name, const sandbox_params_t *params, char **error)
{
    if (error)
        *error = strdup("named profiles are applied through sandbox_init");
    return NULL;
}

void sandbox_free_profile(sandbox_profile_t *profile)
{
    if (!profile)
        return;
    free(profile->reserved);
    free(profile->compiled);
    free(profile);
}

int sandbox_apply(sandbox_profile_t *profile)
{
    if (!profile || !profile->compiled)
        return -1;
    char *error = NULL;
    int result = reef_sandbox_apply(profile->compiled, profile->size, &error);
    if (error) {
        fprintf(stderr, "sandbox_apply: %s\n", error);
        free(error);
    }
    return result;
}
