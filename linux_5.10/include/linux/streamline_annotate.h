/**
 * Copyright (c) 2014-2016, Arm Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef STREAMLINE_ANNOTATE_H
#define STREAMLINE_ANNOTATE_H

/*
 *  User-space only macros:
 *  ANNOTATE_DEFINE            Deprecated and no longer used
 *  ANNOTATE_SETUP             Execute at the start of the program before other ANNOTATE macros are called
 *  ANNOTATE_DELTA_COUNTER     Define a delta counter for use
 *  ANNOTATE_ABSOLUTE_COUNTER  Define an absolute counter for use
 *  ANNOTATE_COUNTER_VALUE     Emit a counter value
 *  CAM_TRACK                  Create a new custom activity map track
 *  CAM_JOB                    Add a new job to a CAM track, use gator_get_time() to obtain the time in nanoseconds
 *  CAM_VIEW_NAME              Name the custom activity map view
 *
 *  User-space and Kernel-space macros:
 *  ANNOTATE(str)                                String annotation
 *  ANNOTATE_CHANNEL(channel, str)               String annotation on a channel
 *  ANNOTATE_COLOR(color, str)                   String annotation with color
 *  ANNOTATE_CHANNEL_COLOR(channel, color, str)  String annotation on a channel with color
 *  ANNOTATE_END()                               Terminate an annotation
 *  ANNOTATE_CHANNEL_END(channel)                Terminate an annotation on a channel
 *  ANNOTATE_NAME_CHANNEL(channel, group, str)   Name a channel and link it to a group
 *  ANNOTATE_NAME_GROUP(group, str)              Name a group
 *  ANNOTATE_VISUAL(data, length, str)           Image annotation with optional string
 *  ANNOTATE_MARKER()                            Marker annotation
 *  ANNOTATE_MARKER_STR(str)                     Marker annotation with a string
 *  ANNOTATE_MARKER_COLOR(color)                 Marker annotation with a color
 *  ANNOTATE_MARKER_COLOR_STR(color, str)        Marker annotation with a string and color
 *
 *  Channels and groups are defined per thread. This means that if the same
 *  channel number is used on different threads they are in fact separate
 *  channels. A channel can belong to only one group per thread. This means
 *  channel 1 cannot be part of both group 1 and group 2 on the same thread.
 *
 *  NOTE: Kernel annotations are not supported in interrupt context.
 */

/* ESC character, hex RGB (little endian) */
#define ANNOTATE_RED    0x0000ff1b
#define ANNOTATE_BLUE   0xff00001b
#define ANNOTATE_GREEN  0x00ff001b
#define ANNOTATE_PURPLE 0xff00ff1b
#define ANNOTATE_YELLOW 0x00ffff1b
#define ANNOTATE_CYAN   0xffff001b
#define ANNOTATE_WHITE  0xffffff1b
#define ANNOTATE_LTGRAY 0xbbbbbb1b
#define ANNOTATE_DKGRAY 0x5555551b
#define ANNOTATE_BLACK  0x0000001b

#define ANNOTATE_COLOR_CYCLE 0x00000000
#define ANNOTATE_COLOR_T1    0x00000001
#define ANNOTATE_COLOR_T2    0x00000002
#define ANNOTATE_COLOR_T3    0x00000003
#define ANNOTATE_COLOR_T4    0x00000004

#ifdef __KERNEL__  /* Start of kernel-space macro definitions */

#include <linux/module.h>

void gator_annotate(const char* str);
void gator_annotate_channel(int channel, const char* str);
void gator_annotate_color(int color, const char* str);
void gator_annotate_channel_color(int channel, int color, const char* str);
void gator_annotate_end(void);
void gator_annotate_channel_end(int channel);
void gator_annotate_name_channel(int channel, int group, const char* str);
void gator_annotate_name_group(int group, const char* str);
void gator_annotate_visual(const char* data, unsigned int length, const char* str);
void gator_annotate_marker(void);
void gator_annotate_marker_str(const char* str);
void gator_annotate_marker_color(int color);
void gator_annotate_marker_color_str(int color, const char* str);

#define ANNOTATE_INVOKE(func, args) \
    func##_ptr = symbol_get(gator_##func); \
    if (func##_ptr) { \
        func##_ptr args; \
        symbol_put(gator_##func); \
    } \

#define ANNOTATE(str) do { \
    void (*annotate_ptr)(const char*); \
    ANNOTATE_INVOKE(annotate, (str)); \
    } while(0)

#define ANNOTATE_CHANNEL(channel, str) do { \
    void (*annotate_channel_ptr)(int, const char*); \
    ANNOTATE_INVOKE(annotate_channel, (channel, str)); \
    } while(0)

#define ANNOTATE_COLOR(color, str) do { \
    void (*annotate_color_ptr)(int, const char*); \
    ANNOTATE_INVOKE(annotate_color, (color, str)); \
    } while(0)

#define ANNOTATE_CHANNEL_COLOR(channel, color, str) do { \
    void (*annotate_channel_color_ptr)(int, int, const char*); \
    ANNOTATE_INVOKE(annotate_channel_color, (channel, color, str)); \
    } while(0)

#define ANNOTATE_END() do { \
    void (*annotate_end_ptr)(void); \
    ANNOTATE_INVOKE(annotate_end, ()); \
    } while(0)

#define ANNOTATE_CHANNEL_END(channel) do { \
    void (*annotate_channel_end_ptr)(int); \
    ANNOTATE_INVOKE(annotate_channel_end, (channel)); \
    } while(0)

#define ANNOTATE_NAME_CHANNEL(channel, group, str) do { \
    void (*annotate_name_channel_ptr)(int, int, const char*); \
    ANNOTATE_INVOKE(annotate_name_channel, (channel, group, str)); \
    } while(0)

#define ANNOTATE_NAME_GROUP(group, str) do { \
    void (*annotate_name_group_ptr)(int, const char*); \
    ANNOTATE_INVOKE(annotate_name_group, (group, str)); \
    } while(0)

#define ANNOTATE_VISUAL(data, length, str) do { \
    void (*annotate_visual_ptr)(const char*, unsigned int, const char*); \
    ANNOTATE_INVOKE(annotate_visual, (data, length, str)); \
    } while(0)

#define ANNOTATE_MARKER() do { \
    void (*annotate_marker_ptr)(void); \
    ANNOTATE_INVOKE(annotate_marker, ()); \
    } while(0)

#define ANNOTATE_MARKER_STR(str) do { \
    void (*annotate_marker_str_ptr)(const char*); \
    ANNOTATE_INVOKE(annotate_marker_str, (str)); \
    } while(0)

#define ANNOTATE_MARKER_COLOR(color) do { \
    void (*annotate_marker_color_ptr)(int); \
    ANNOTATE_INVOKE(annotate_marker_color, (color)); \
    } while(0)

#define ANNOTATE_MARKER_COLOR_STR(color, str) do { \
    void (*annotate_marker_color_str_ptr)(int, const char*); \
    ANNOTATE_INVOKE(annotate_marker_color_str, (color, str)); \
    } while(0)

#else  /* Start of user-space macro definitions */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum gator_annotate_counter_class {
  ANNOTATE_DELTA = 1,
  ANNOTATE_ABSOLUTE,
  ANNOTATE_ACTIVITY
};

enum gator_annotate_display {
  ANNOTATE_AVERAGE = 1,
  ANNOTATE_ACCUMULATE,
  ANNOTATE_HERTZ,
  ANNOTATE_MAXIMUM,
  ANNOTATE_MINIMUM
};

enum gator_annotate_series_composition {
  ANNOTATE_STACKED = 1,
  ANNOTATE_OVERLAY,
  ANNOTATE_LOG10
};

enum gator_annotate_rendering_type {
  ANNOTATE_FILL = 1,
  ANNOTATE_LINE,
  ANNOTATE_BAR
};

void gator_annotate_setup(void);
uint64_t gator_get_time(void);
void gator_annotate_fork_child(void);
void gator_annotate_flush(void);
void gator_annotate_str(const uint32_t channel, const char *const str);
void gator_annotate_color(const uint32_t channel, const uint32_t color, const char *const str);
void gator_annotate_name_channel(const uint32_t channel, const uint32_t group, const char *const str);
void gator_annotate_name_group(const uint32_t group, const char *const str);
void gator_annotate_visual(const void *const data, const uint32_t length, const char *const str);
void gator_annotate_marker(const char *const str);
void gator_annotate_marker_color(const uint32_t color, const char *const str);
void gator_annotate_counter(const uint32_t id, const char *const title, const char *const name, const int per_cpu, const enum gator_annotate_counter_class counter_class, const enum gator_annotate_display display, const char *const units, const uint32_t modifier, const enum gator_annotate_series_composition series_composition, const enum gator_annotate_rendering_type rendering_type, const int average_selection, const int average_cores, const int percentage, const size_t activity_count, const char *const *const activities, const uint32_t *const activity_colors, const uint32_t cores, const uint32_t color, const char *const description);
void gator_annotate_counter_value(const uint32_t core, const uint32_t id, const uint32_t value);
void gator_annotate_activity_switch(const uint32_t core, const uint32_t id, const uint32_t activity, const uint32_t tid);
void gator_cam_track(const uint32_t view_uid, const uint32_t track_uid, const uint32_t parent_track, const char *const name);
void gator_cam_job(const uint32_t view_uid, const uint32_t job_uid, const char *const name, const uint32_t track, const uint64_t start_time, const uint64_t duration, const uint32_t color, const uint32_t primary_dependency, const size_t dependency_count, const uint32_t *const dependencies);
void gator_cam_job_start(const uint32_t view_uid, const uint32_t job_uid, const char *const name, const uint32_t track, const uint64_t time, const uint32_t color);
void gator_cam_job_set_dependencies(const uint32_t view_uid, const uint32_t job_uid, const uint64_t time, const uint32_t primary_dependency, const size_t dependency_count, const uint32_t *const dependencies);
void gator_cam_job_stop(const uint32_t view_uid, const uint32_t job_uid, const uint64_t time);
void gator_cam_view_name(const uint32_t view_uid, const char *const name);

#define ANNOTATE_DEFINE extern int gator_annotate_unused

#define ANNOTATE_SETUP gator_annotate_setup()

#define ANNOTATE(str) gator_annotate_str(0, str)
#define ANNOTATE_CHANNEL(channel, str) gator_annotate_str(channel, str)
#define ANNOTATE_COLOR(color, str) gator_annotate_color(0, color, str)
#define ANNOTATE_CHANNEL_COLOR(channel, color, str) gator_annotate_color(channel, color, str)
#define ANNOTATE_END() gator_annotate_str(0, NULL)
#define ANNOTATE_CHANNEL_END(channel) gator_annotate_str(channel, NULL)
#define ANNOTATE_NAME_CHANNEL(channel, group, str) gator_annotate_name_channel(channel, group, str)
#define ANNOTATE_NAME_GROUP(group, str) gator_annotate_name_group(group, str)
#define ANNOTATE_VISUAL(data, length, str) gator_annotate_visual(data, length, str)
#define ANNOTATE_MARKER() gator_annotate_marker(NULL)
#define ANNOTATE_MARKER_STR(str) gator_annotate_marker(str)
#define ANNOTATE_MARKER_COLOR(color) gator_annotate_marker_color(color, NULL)
#define ANNOTATE_MARKER_COLOR_STR(color, str) gator_annotate_marker_color(color, str)
#define ANNOTATE_DELTA_COUNTER(id, title, name) gator_annotate_counter(id, title, name, 0, ANNOTATE_DELTA, ANNOTATE_ACCUMULATE, NULL, 1, ANNOTATE_STACKED, ANNOTATE_FILL, 0, 0, 0, 0, NULL, NULL, 0, ANNOTATE_COLOR_CYCLE, NULL)
#define ANNOTATE_ABSOLUTE_COUNTER(id, title, name) gator_annotate_counter(id, title, name, 0, ANNOTATE_ABSOLUTE, ANNOTATE_MAXIMUM, NULL, 1, ANNOTATE_STACKED, ANNOTATE_FILL, 0, 0, 0, 0, NULL, NULL, 0, ANNOTATE_COLOR_CYCLE, NULL)
#define ANNOTATE_COUNTER_VALUE(id, value) gator_annotate_counter_value(0, id, value)
#define CAM_TRACK(view_uid, track_uid, parent_track, name) gator_cam_track(view_uid, track_uid, parent_track, name)
#define CAM_JOB(view_uid, job_uid, name, track, start_time, duration, color) gator_cam_job(view_uid, job_uid, name, track, start_time, duration, color, -1, 0, 0)
#define CAM_JOB_DEP(view_uid, job_uid, name, track, start_time, duration, color, dependency) { \
    uint32_t __dependency = dependency; \
    gator_cam_job(view_uid, job_uid, name, track, start_time, duration, color, -1, 1, &__dependency); \
}
#define CAM_JOB_DEPS(view_uid, job_uid, name, track, start_time, duration, color, dependency_count, dependencies) gator_cam_job(view_uid, job_uid, name, track, start_time, duration, color, -1, dependency_count, dependencies)
#define CAM_JOB_START(view_uid, job_uid, name, track, time, color) gator_cam_job_start(view_uid, job_uid, name, track, time, color)
#define CAM_JOB_SET_DEP(view_uid, job_uid, time, dependency) { \
    uint32_t __dependency = dependency; \
    gator_cam_job_set_dependencies(view_uid, job_uid, time, -1, 1, &__dependency); \
}
#define CAM_JOB_SET_DEPS(view_uid, job_uid, time, dependency_count, dependencies) gator_cam_job_set_dependencies(view_uid, job_uid, time, -1, dependency_count, dependencies)
#define CAM_JOB_STOP(view_uid, job_uid, time) gator_cam_job_stop(view_uid, job_uid, time)
#define CAM_VIEW_NAME(view_uid, name) gator_cam_view_name(view_uid, name)

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_ */
#endif /* STREAMLINE_ANNOTATE_H */
