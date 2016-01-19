/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2016 Treasure Data Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <ctype.h>
#include <sys/utsname.h>

#include <fluent-bit/flb_kernel.h>
#include <fluent-bit/flb_utils.h>

/*
 * Routine taken from Monkey Project, Eduardo says it's ok ;)
 */
struct flb_kernel *flb_kernel_info()
{

    int a, b, c;
    int len;
    int pos;
    char *p, *t;
    char *tmp;
    struct utsname uts;
    struct flb_kernel *kernel;

    if (uname(&uts) == -1) {
        perror("uname");
        return NULL;
    }
    len = strlen(uts.release);

    /* Fixme: this don't support Linux Kernel 10.x.x :P */
    a = (*uts.release - '0');

    /* Second number */
    p = (uts.release) + 2;
    pos = mk_string_char_search(p, '.', len - 2);
    if (pos <= 0) {
        /* Some Debian systems uses a different notation, e.g: 3.14-2-amd64 */
        pos = mk_string_char_search(p, '-', len - 2);
        if (pos <= 0) {
            return NULL;
        }
    }

    tmp = mk_string_copy_substr(p, 0, pos);
    if (!tmp) {
        return NULL;
    }
    b = atoi(tmp);
    free(tmp);

    /* Last number (it needs filtering) */
    t = p = p + pos + 1;
    do {
        t++;
    } while (isdigit(*t));

    tmp = mk_string_copy_substr(p, 0, t - p);
    if (!tmp) {
        return NULL;
    }
    c = atoi(tmp);
    free(tmp);

    kernel = malloc(sizeof(struct flb_kernel));
    if (!kernel) {
        return NULL;
    }
    kernel->minor = a;
    kernel->major = b;
    kernel->patch = c;
    kernel->s_version.data = malloc(16);

    len = snprintf(kernel->s_version.data, 16, "%i.%i.%i", a, b, c);
    if (len == -1) {
        perror("snprintf");
        return NULL;
    }
    kernel->s_version.len = len;
    kernel->n_version = FLB_KERNEL_VERSION(a, b, c);

    flb_debug("Linux Kernel = %i.%i.%i", a, b, c);

    return kernel;
}
