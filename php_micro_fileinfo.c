/*
micro SAPI for PHP - php_micro_filesize.c
filesize reproduction utilities for micro

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

#include "php.h"

#include "php_micro.h"
#include "php_micro_helper.h"

#include <stdint.h>
#include <fcntl.h>

#    include <sys/auxv.h>

const char *micro_get_filename(void);

// do we need uint64_t for sfx size?
static uint32_t _final_sfx_filesize = 0;
uint32_t _micro_get_sfx_filesize(void);
uint32_t micro_get_sfx_filesize(void) {
    return _final_sfx_filesize;
}

typedef struct _ext_ini_header_t {
    uint8_t magic[4];
    uint8_t len[4];
} ext_ini_header_t;

struct _ext_ini {
    size_t size;
    char *data;
} micro_ext_ini = {.size = 0, .data = NULL};

// shabby endian-independent check
#define checkmagic(var) \
    (var[0] != PHP_MICRO_INIMARK[0] || var[1] != PHP_MICRO_INIMARK[1] || var[2] != PHP_MICRO_INIMARK[2] || \
        var[3] != PHP_MICRO_INIMARK[3])

int micro_fileinfo_init(void) {
    int ret = 0;
    uint32_t len = 0;
    uint32_t sfx_filesize = _micro_get_sfx_filesize();
    const char *self_path = micro_get_filename();
    int fd = open(self_path, O_RDONLY);
    if (-1 == fd) {
        // TODO: tell failed here
        ret = errno;
        goto end;
    }
    struct stat stats;
    ret = stat(self_path, &stats);
    if (-1 == ret) {
        // TODO: tell failed here
        ret = errno;
        goto end;
    }
    size_t filesize = stats.st_size;
    if (filesize <= sfx_filesize) {
        fprintf(stderr, "no payload found.\n" PHP_MICRO_HINT, self_path);
        ret = FAILURE;
        goto end;
    }
#    define seekfile(x) \
        do { lseek(fd, x, SEEK_SET); } while (0)
#    define readfile(dest, size, red) \
        do { red = read(fd, dest, size); } while (0)
#    define closefile() \
        do { \
            if (-1 != fd) { \
                close(fd); \
            } \
        } while (0)
    ext_ini_header_t ext_ini_header = {0};
    if (filesize <= sfx_filesize + sizeof(ext_ini_header)) {
        ret = FAILURE;
        goto end;
    }
    // we may have extra ini configs.
    seekfile(sfx_filesize);
    uint32_t red = 0;
    readfile(&ext_ini_header, sizeof(ext_ini_header), red);
    if (sizeof(ext_ini_header) != red) {
        // cannot read file
        ret = errno;
        goto end;
    }

    if (checkmagic(ext_ini_header.magic)) {
        // bad magic, not an extra ini
        ret = SUCCESS;
        goto end;
    }
    // shabby ntohl
    len = (ext_ini_header.len[0] << 24) + (ext_ini_header.len[1] << 16) + (ext_ini_header.len[2] << 8) +
        ext_ini_header.len[3];
    dbgprintf("len is %d\n", len);
    if (filesize <= sfx_filesize + sizeof(ext_ini_header) + len) {
        // bad len, not an extra ini
        ret = SUCCESS;
        len = 0;
        goto end;
    }
    micro_ext_ini.data = malloc(len + 2);
    readfile(micro_ext_ini.data, len, red);
    if (len != red) {
        // cannot read file
        ret = errno;
        len = 0;
        free(micro_ext_ini.data);
        micro_ext_ini.data = NULL;
        goto end;
    }
    // two '\0's like hardcoden inis
    micro_ext_ini.data[len] = '\0';
    micro_ext_ini.data[len + 1] = '\0';
    dbgprintf("using ext ini %s\n", micro_ext_ini.data);
    micro_ext_ini.size = len + 1;
    len += sizeof(ext_ini_header_t);

end:
    _final_sfx_filesize = sfx_filesize + len;
    closefile();
    return ret;
#undef seekfile
#undef readfile
#undef closefile
}

/*
 *   _micro_get_sfx_filesize - get (real) sfx size using resource(win) / 2 stage build constant (others)
 */
uint32_t _micro_get_sfx_filesize(void) {
    static uint32_t _sfx_filesize = SFX_FILESIZE;
    return _sfx_filesize;
}

const char *micro_get_filename(void) {
    static char *self_filename = NULL;
    if (NULL == self_filename) {
        self_filename = malloc(PATH_MAX);
        (void)realpath((const char *)getauxval(AT_EXECFN), self_filename);
    }
    return self_filename;
}

size_t micro_get_filename_len(void) {
    static size_t _micro_filename_l = -1;
    if (-1 == _micro_filename_l) {
        _micro_filename_l = strlen(micro_get_filename());
    }
    return _micro_filename_l;
}


PHP_FUNCTION(micro_get_self_filename) {
    RETURN_STRING(micro_get_filename());
}

PHP_FUNCTION(micro_get_sfx_filesize) {
    RETURN_LONG(micro_get_sfx_filesize());
}
