#pragma once
#ifndef _CAT_PROFILER_H_
#define _CAT_PROFILER_H_

#include "cat/cat_platform.h"
#include "cat/utility/cat_time.h"

cat_interface_begin;


cat_decl bool cat_profiler_begin(char const* const name);

cat_decl bool cat_profiler_end(char const* const name);

cat_decl bool cat_profiler_report(char const* const name);

cat_decl bool cat_profiler_report_all(void);

cat_decl bool cat_profiler_test(void);


cat_interface_end;

#endif