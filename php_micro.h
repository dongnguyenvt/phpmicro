/*
micro SAPI for PHP - php_micro.h
header for micro

Copyright 2020 Longyan
Copyright 2022 Yun Dou <dixyes@gmail.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef _PHP_MICRO_H
#define _PHP_MICRO_H

#define STRINGIZE(x)  STRINGIZE2(x)
#define STRINGIZE2(x) #x

#define PHP_MICRO_VER_MAJ 0
#define PHP_MICRO_VER_MIN 1
#define PHP_MICRO_VER_PAT 0
//#define PHP_MICRO_VER_APP "nope"
#ifdef PHP_MICRO_VER_APP
#    define PHP_MICRO_VER_STR \
        STRINGIZE(PHP_MICRO_VER_MAJ) "." STRINGIZE(PHP_MICRO_VER_MIN) "." STRINGIZE(PHP_MICRO_VER_PAT) "-" PHP_MICRO_VER_APP
#else
#    define PHP_MICRO_VER_STR \
        STRINGIZE(PHP_MICRO_VER_MAJ) "." STRINGIZE(PHP_MICRO_VER_MIN) "." STRINGIZE(PHP_MICRO_VER_PAT)
#endif

#define PHP_MICRO_SFX_FILESIZE_ID 12345
#    define PHP_MICRO_HINT_CMDC "cat %s mycode.php > mycode && chmod 0755 ./mycode"
#    define PHP_MICRO_HINT_CMDE "./mycode myarg1 myarg2"
#define PHP_MICRO_HINT \
    "micro SAPI for PHP" PHP_VERSION " v" PHP_MICRO_VER_STR "\n" \
    "Usage: concatenate this binary with any php code then execute it.\n" \
    "for example: if we have code as mycode.php, to concatenate them, execute:\n" \
    "    " PHP_MICRO_HINT_CMDC "\n" \
    "then execute it:\n" \
    "    " PHP_MICRO_HINT_CMDE "\n"

#    define MICRO_SFX_EXPORT __attribute__((visibility("default")))

#define PHP_MICRO_INIMARK     ((uint8_t[4]){0xfd, 0xf6, 0x69, 0xe6})
#define PHP_MICRO_INIENTRY(x) ("micro." #x)

#ifdef _DEBUG
extern int micro_debug;
#    define dbgprintf(...) \
        do { \
            if (micro_debug != 0) { \
                printf(__VA_ARGS__); \
            } \
        } while (0)
#else
#    define dbgprintf(...)
#endif

static inline const char *micro_slashize(const char *x) {
    size_t size = strlen(x);
    char *ret = malloc(size + 2);
    memcpy(ret, x, size);
    for (size_t i = 0; i < size; i++) {
        if ('\\' == ret[i]) {
            ret[i] = '/';
        }
    }
    if ('/' != ret[size - 1]) {
        ret[size] = '/';
        ret[size + 1] = '\0';
    } else {
        ret[size] = '\0';
    }
    // dbgprintf("slashed %s\n", ret);
    return ret;
}

#endif // _PHP_MICRO_H
