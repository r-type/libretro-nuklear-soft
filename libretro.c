#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <libretro.h>

#include "libretro-core.h"

unsigned rwidth  = BASE_WIDTH;
unsigned rheight = BASE_HEIGHT;
#ifdef M16B
unsigned short int *retroscreen;
#else
unsigned int *retroscreen;
#endif
float retro_fps = 60.0;

retro_log_printf_t log_cb;

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;

retro_input_poll_t input_poll_cb;
retro_input_state_t input_state_cb;
extern int app_init(void);
extern int app_free(void);
extern int app_main(void);
extern int app_event(void);

void context_reset(void)
{
   fprintf(stderr, "Context reset!\n");
   app_init();
}

void context_destroy(void)
{
   fprintf(stderr, "Context destroy!\n");
   app_free();
}

void retro_init(void)
{

   struct retro_log_callback log;

   if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else
      log_cb = NULL;
}

void retro_deinit(void)
{
   context_destroy();
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   (void)port;
   (void)device;
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "Nuklear Soft Libretro";
   info->library_version  = "v1.18";
   info->need_fullpath    = false;
   info->valid_extensions = NULL; // Anything is fine, we don't care.
}


static void update_variables(void)
{
   struct retro_variable var = {
      .key = "nuklear_resolution",
   };

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      char *pch;
      char str[100];
      snprintf(str, sizeof(str), "%s", var.value);
      
      pch = strtok(str, "x");
      if (pch)
         rwidth = strtoul(pch, NULL, 0);
      pch = strtok(NULL, "x");
      if (pch)
         rheight = strtoul(pch, NULL, 0);

      fprintf(stderr, "[libretro-Nuklear]: Got size: %u x %u.\n", rwidth, rheight);

      struct retro_system_av_info ninfo;

      retro_get_system_av_info(&ninfo);

      environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &ninfo);

      if (log_cb)
         log_cb(RETRO_LOG_INFO, "ChangeAV: w:%d h:%d ra:%f.\n",
               ninfo.geometry.base_width, ninfo.geometry.base_height, ninfo.geometry.aspect_ratio);
   }

}

void retro_get_system_av_info(struct retro_system_av_info *info)
{

   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      update_variables();


   info->geometry.base_width  = rwidth;
   info->geometry.base_height = rheight;

   if (log_cb)
      log_cb(RETRO_LOG_INFO, "AV_INFO: width=%d height=%d\n",info->geometry.base_width,info->geometry.base_height);

   info->geometry.max_width   = MAX_WIDTH;
   info->geometry.max_height  = MAX_HEIGHT;

   if (log_cb)
      log_cb(RETRO_LOG_INFO, "AV_INFO: max_width=%d max_height=%d\n",info->geometry.max_width,info->geometry.max_height);

   info->geometry.aspect_ratio = (float)rwidth/rheight;

   if (log_cb)
      log_cb(RETRO_LOG_INFO, "AV_INFO: aspect_ratio = %f\n",info->geometry.aspect_ratio);

   info->timing.fps            = retro_fps;
   info->timing.sample_rate    = 44100.0;

   if (log_cb)
      log_cb(RETRO_LOG_INFO, "AV_INFO: fps = %f sample_rate = %f\n",info->timing.fps,info->timing.sample_rate);

}



void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   struct retro_variable variables[] = {
      {
         "nuklear_resolution",
         "Internal resolution; 320x240|360x480|480x272|512x384|512x512|640x240|640x448|640x480|720x576|800x600|960x720|1024x768|1024x1024|1280x720|1200x800|1280x960|1600x900|1600x1200|1920x1080|1920x1440|1920x1600|2048x2048",
      },

      { NULL, NULL },
   };

   bool no_rom = true;
   cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_rom);
   cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}


static unsigned frame_count;


void retro_run(void)
{
   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      update_variables();


   app_event();

   app_main();

   frame_count++;

   video_cb(retroscreen, rwidth, rheight, 0);
}

bool retro_load_game(const struct retro_game_info *info)
{
  // update_variables();
#ifdef M16B
   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
#else
   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
#endif

   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
#ifdef M16B
      fprintf(stderr, "RGB565 is not supported.\n");
#else
      fprintf(stderr, "XRGB8888 is not supported.\n");
#endif
      return false;
   }

   context_reset();

   fprintf(stderr, "Loaded game!\n");
   (void)info;
   return true;
}

void retro_unload_game(void)
{}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   (void)type;
   (void)info;
   (void)num;
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *data, size_t size)
{
   (void)data;
   (void)size;
   return false;
}

bool retro_unserialize(const void *data, size_t size)
{
   (void)data;
   (void)size;
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_reset(void)
{}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}

