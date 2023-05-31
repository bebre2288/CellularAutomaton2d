// flag.h -- command-line flag parsing
//
//   Inspired by Go's flag module: https://pkg.go.dev/flag
//
#ifndef FLAG_H_
#define FLAG_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

// TODO: add support for -flag=x syntax
// TODO: *_var function variants
// void flag_bool_var(bool *var, const char *name, bool def, const char *desc);
// void flag_bool_uint64(int *var, const char *name, bool def, const char *desc);
// etc.
// WARNING! *_var functions may break the flag_name() functionality

char *flag_name(void *val);
bool *flag_bool(const char *name, bool def, const char *desc);
int *flag_uint64(const char *name, int def, const char *desc);
int *flag_int(const char *name, int def, const char *desc);
char **flag_str(const char *name, const char *def, const char *desc);
bool flag_parse(int argc, char **argv);
int flag_rest_argc(void);
char **flag_rest_argv(void);
void flag_print_error(FILE *stream);
void flag_print_options();

#endif // FLAG_H_

//////////////////////////////

#ifdef FLAG_IMPLEMENTATION

typedef enum {
    FLAG_BOOL = 0,
    FLAG_INT,
    FLAG_STR,
    COUNT_FLAG_TYPES,
} Flag_Type;

static_assert(COUNT_FLAG_TYPES == 3, "Exhaustive Flag_Value definition");
typedef union {
    char *as_str;
    bool as_bool;
    int as_size;
} Flag_Value;

typedef enum {
    FLAG_NO_ERROR = 0,
    FLAG_ERROR_UNKNOWN,
    FLAG_ERROR_NO_VALUE,
    FLAG_ERROR_INVALID_NUMBER,
    FLAG_ERROR_INTEGER_OVERFLOW,
    FLAG_ERROR_INVALID_SIZE_SUFFIX,
    COUNT_FLAG_ERRORS,
} Flag_Error;

typedef struct {
    Flag_Type type;
    char *name;
    char *desc;
    Flag_Value val;
    Flag_Value def;
} Flag;

#ifndef FLAGS_CAP
#define FLAGS_CAP 256
#endif

typedef struct {
    Flag flags[FLAGS_CAP];
    int flags_count;

    Flag_Error flag_error;
    char *flag_error_name;

    int rest_argc;
    char **rest_argv;
} Flag_Context;

static Flag_Context flag_global_context;

Flag *flag_new(Flag_Type type, const char *name, const char *desc)
{
    Flag_Context *c = &flag_global_context;

    assert(c->flags_count < FLAGS_CAP);
    Flag *flag = &c->flags[c->flags_count++];
    memset(flag, 0, sizeof(*flag));
    flag->type = type;
    // NOTE: I won't touch them I promise Kappa
    flag->name = (char*) name;
    flag->desc = (char*) desc;
    return flag;
}

char *flag_name(void *val)
{
    Flag *flag = (Flag*) ((char*) val - offsetof(Flag, val));
    return flag->name;
}

int *flag_int(const char *name, int def, const char *desc)
{
    Flag *flag = flag_new(FLAG_INT, name, desc);
    flag->val.as_size = def;
    flag->def.as_size = def;
    return &flag->val.as_size;
}

char **flag_str(const char *name, const char *def, const char *desc)
{
    Flag *flag = flag_new(FLAG_STR, name, desc);
    flag->val.as_str = (char*) def;
    flag->def.as_str = (char*) def;
    return &flag->val.as_str;
}

static char *flag_shift_args(int *argc, char ***argv)
{
    assert(*argc > 0);
    char *result = **argv;
    *argv += 1;
    *argc -= 1;
    return result;
}

int flag_rest_argc(void)
{
    return flag_global_context.rest_argc;
}

char **flag_rest_argv(void)
{
    return flag_global_context.rest_argv;
}

bool flag_parse(int argc, char **argv)
{
    Flag_Context *c = &flag_global_context;

    flag_shift_args(&argc, &argv);

    while (argc > 0) {
        char *flag = flag_shift_args(&argc, &argv);

        if (*flag != '-') {
            // NOTE: pushing flag back into args
            c->rest_argc = argc + 1;
            c->rest_argv = argv - 1;
            return true;
        }

        if (strcmp(flag, "--") == 0) {
            // NOTE: but if it's the terminator we don't need to push it back
            c->rest_argc = argc;
            c->rest_argv = argv;
            return true;
        }

        // NOTE: remove the dash
        flag += 1;

        bool found = false;
        for (int i = 0; i < c->flags_count; ++i) {
            if (strcmp(c->flags[i].name, flag) == 0) {
                static_assert(COUNT_FLAG_TYPES == 3, "Exhaustive flag type parsing");
                switch (c->flags[i].type) {
                case FLAG_BOOL: {
                    c->flags[i].val.as_bool = true;
                }
                break;

                case FLAG_STR: {
                    if (argc == 0) {
                        c->flag_error = FLAG_ERROR_NO_VALUE;
                        c->flag_error_name = flag;
                        return false;
                    }
                    char *arg = flag_shift_args(&argc, &argv);
                    c->flags[i].val.as_str = arg;
                }
                break;

                case FLAG_INT: {
                    if (argc == 0) {
                        c->flag_error = FLAG_ERROR_NO_VALUE;
                        c->flag_error_name = flag;
                        return false;
                    }
                    char *arg = flag_shift_args(&argc, &argv);
                    char *endptr;
                    // TODO: replace strtoull with a custom solution
                    // That way we can get rid of the dependency on errno and static_assert
                    int result = strtoull(arg, &endptr, 10);
                    c->flags[i].val.as_size = result;
                }
                break;

                case COUNT_FLAG_TYPES:
                default: {
                    assert(0 && "unreachable");
                    exit(69);
                }
                }

                found = true;
            }
        }

        if (!found) {
            c->flag_error = FLAG_ERROR_UNKNOWN;
            c->flag_error_name = flag;
            return false;
        }
    }

    c->rest_argc = argc;
    c->rest_argv = argv;
    return true;
}
void flag_print_options(){
    Flag_Context *c = &flag_global_context;
    for (size_t i = 0; i < c->flags_count; ++i) {
        Flag *flag = &c->flags[i];

        printf("    -%s\n", flag->name);
        printf("        %s\n", flag->desc);
        static_assert(COUNT_FLAG_TYPES == 3, "Exhaustive flag type defaults printing");
        switch (c->flags[i].type) {
        case FLAG_INT:
            printf("        Default: %d\n", flag->def.as_size);
            break;
        case FLAG_STR:
            if (flag->def.as_str) {
                printf("        Default: %s\n", flag->def.as_str);
            }
            break;
        default:
            assert(0 && "unreachable");
            exit(69);
        }
    }
}
#endif
// Copyright 2021 Alexey Kutepov <reximkut@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
