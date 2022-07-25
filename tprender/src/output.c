/* output.c - Output module frontend
 *
 * Copyright (C) 2007 Ivo Clarysse,  (C) 2012 Henner Zeller
 *
 * This file is part of GMediaRender.
 *
 * GMediaRender is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GMediaRender is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GMediaRender; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */
#define HAVE_CONFIG_H
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <glib.h>

#include "logging.h"

#include "output.h"
#include "tplayer_dec.h"

void output_dump_modules(void)
{

}

int output_init(const char *shortname)
{
	int count;
	int ret;

	ret = tplayer_dec_init();
	
	if(ret < 0)
	{
		Log_error("output", "tplayer_dec_init fail\n");
		return -1;
	}

	return 0;
}

static GMainLoop *main_loop_ = NULL;
static void exit_loop_sighandler(int sig) {
	tplayer_dec_uninit();
	printf("tplayer_dec_uninit\n");
	if (main_loop_) {
		// TODO(hzeller): revisit - this is not safe to do.
		g_main_loop_quit(main_loop_);
	}
}

int output_loop()
{
    /* Create a main loop that runs the default GLib main context */
    main_loop_ = g_main_loop_new(NULL, FALSE);

	signal(SIGINT, &exit_loop_sighandler);
	signal(SIGTERM, &exit_loop_sighandler);

    g_main_loop_run(main_loop_);

        return 0;
}

int output_add_options(GOptionContext *ctx)
{

	return 0;
}

void output_set_uri(const char *uri, output_update_meta_cb_t meta_cb) {

	tplay_dec_stop();
	tplay_dec_reset();
	tplay_dec_url(uri);
	printf("tplay_dec_url\n");
	tplay_dec_prepare_async();
	printf("tplay_dec_prepare_async\n");
	tplay_dec_start();
	printf("tplay_dec_start\n");
	
}
void output_set_next_uri(const char *uri) {
	printf("output_set_next_uri uri %s\n", uri);
}

int output_play(output_transition_cb_t transition_callback) {

	printf("output_play\n");
	return 0;
}

int output_pause(void) {
	printf("output_pause\n");
	return 0;
}

int output_stop(void) {
	printf("output_stop\n");
	return 0;
}

int output_seek(gint64 position_nanos) {
	printf("output_seek\n");
	return 0;
}

int output_get_position(gint64 *track_dur, gint64 *track_pos) {
	return 0;
}

int output_get_volume(float *value) {
	printf("output_set_volume\n");
	return 0;
}
int output_set_volume(float value) {
	printf("output_set_volume %f\n", value);

}
int output_get_mute(int *value) {
	printf("output_get_mute\n");
	return 0;
}
int output_set_mute(int value) {
	printf("output_set_mute %d\n", value);
	return 0;
}
