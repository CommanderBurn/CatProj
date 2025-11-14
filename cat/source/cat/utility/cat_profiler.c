#include "cat/utility/cat_profiler.h"
#include "cat/utility/cat_console.h"
#include "cat/cat_platform.inl"
#include <string.h>

cat_implementation_begin;

#define CAT_PROFILER_MAX_ENTRIES 16
#define CAT_PROFILER_WINDOW_SIZE 64

typedef struct cat_profiler_entry
{
    char const* name;
    cat_time_t active_start;
    cat_time_t samples[CAT_PROFILER_WINDOW_SIZE];
    int index;
    int count;
    int active; //1 = currently timing
} cat_profiler_entry;

static cat_profiler_entry profiler_entries[CAT_PROFILER_MAX_ENTRIES];
static int profiler_count = 0;


static cat_profiler_entry* cat_profiler_get(char const* name)
{
    //check if a profiler with the same name already exists
    for (int i = 0; i < profiler_count; ++i)
    {
        if (strcmp(profiler_entries[i].name, name) == 0)
            return &profiler_entries[i];
    }

    //create new profiler
    if (profiler_count < CAT_PROFILER_MAX_ENTRIES)
    {
        cat_profiler_entry* e = &profiler_entries[profiler_count++];
        e->name = name;
        e->index = 0;
        e->count = 0;
        e->active = 0;
        return e;
    }

    return 0;
}


cat_impl bool cat_profiler_begin(char const* const name)
{
    cat_profiler_entry* e = cat_profiler_get(name);
    if (!e)
        return false;

    e->active_start = cat_platform_time();
    e->active = 1;
    return true;
}


cat_impl bool cat_profiler_end(char const* const name)
{
    cat_profiler_entry* e = cat_profiler_get(name);
    if (!e || !e->active)
        return false;

    cat_time_t end = cat_platform_time();
    cat_time_t duration = end - e->active_start;
    e->active = 0;

    e->samples[e->index] = duration;
    e->index = (e->index + 1) % CAT_PROFILER_WINDOW_SIZE;

    if (e->count < CAT_PROFILER_WINDOW_SIZE)
        e->count += 1;
    return true;
}


cat_impl bool cat_profiler_report(char const* const name)
{
    cat_profiler_entry* e = cat_profiler_get(name);
    if (!e)
        return false;

    if (e->count == 0)
    {
        printf("\nProfiler '%s': no samples.\n", name);
        return false;
    }

    cat_time_rate_t rate = cat_platform_time_rate();
    cat_time_t min = e->samples[0];
    cat_time_t max = e->samples[0];
    cat_time_t total = 0;

    for (int i = 0; i < e->count; ++i)
    {
        cat_time_t s = e->samples[i];
        if (s < min) min = s;
        if (s > max) max = s;
        total += s;
    }

    double avg_ms = (double)total / (double)e->count * 1000.0 / (double)rate;
    double min_ms = (double)min * 1000.0 / (double)rate;
    double max_ms = (double)max * 1000.0 / (double)rate;

    printf(
        "\nProfiler '%s':"
        "\n    samples=%d"
        "\n    avg = %.3f ms"
        "\n    min = %.3f ms"
        "\n    max = %.3f ms\n",
        name,
        e->count,
        avg_ms,
        min_ms,
        max_ms
    );
    return true;
}


cat_impl bool cat_profiler_report_all(void)
{
    printf("\n=== Profiler Report: All Sections ===\n");

    for (int i = 0; i < profiler_count; ++i)
        cat_profiler_report(profiler_entries[i].name);
    return true;
}


cat_noinl bool cat_profiler_test(void)
{
    printf("\n=== Profiler Test ===\n");

    cat_profiler_begin("Example Work");
    cat_platform_sleep(cat_platform_time_rate() / 4);
    cat_profiler_end("Example Work");

    cat_profiler_begin("Example Work");
    cat_platform_sleep(cat_platform_time_rate() / 2);
    cat_profiler_end("Example Work");

    cat_profiler_report("Example Work");
    return true;
}

cat_implementation_end;