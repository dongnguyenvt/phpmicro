/*
micro SAPI for PHP - php_micro_helper.h
micro helpers like dbgprintf

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
#include <stdint.h>

#include "php.h"

#include "php_micro.h"
#include "php_micro_fileinfo.h"


PHP_FUNCTION(micro_version) {
    array_init(return_value);
    zval zv;
    ZVAL_LONG(&zv, PHP_MICRO_VER_MAJ);
    zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &zv);
    ZVAL_LONG(&zv, PHP_MICRO_VER_MIN);
    zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &zv);
    ZVAL_LONG(&zv, PHP_MICRO_VER_PAT);
    zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &zv);
#ifdef PHP_MICRO_VER_APP
    ZVAL_STRING(&zv, PHP_MICRO_VER_APP);
    zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &zv);
#endif
}

PHP_FUNCTION(micro_open_self) {
    php_stream *stream = NULL;
    FILE *fp = VCWD_FOPEN(micro_get_filename(), "rb");
    stream = php_stream_fopen_from_file(fp, "rb");
    if (NULL == stream) {
        RETURN_FALSE;
    }
    php_stream_to_zval(stream, return_value);
}
