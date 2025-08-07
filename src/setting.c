/*
  Copyright (c) 2025 zhengxyz123

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "setting.h"
#include "cjson/cJSON.h"
#include "global.h"
#include <stdio.h>
#include <string.h>
#if defined(__LINUX__)
    #include <unistd.h>
#elif defined(__WIN32__)
    #include <io.h>
    #define F_OK 0
    #define access _access
#endif

extern GameApp global_app;
extern Setting global_setting;

void InitSetting() {
#if !defined(__PSP__)
    char* setting_file = (char*)calloc(PATH_MAX, sizeof(char));
    strcpy(setting_file, global_app.exec_path);
    strcat(setting_file, "settings.json");
    if (access(setting_file, F_OK) == -1) {
        return;
    }
    FILE* fp = fopen(setting_file, "r");
    if (fp == NULL) {
        return;
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* data = calloc(fsize + 1, sizeof(char));
    fread(data, fsize, 1, fp);
    data[fsize] = '\0';
    fclose(fp);
    cJSON* setting_json = cJSON_Parse(data);
    cJSON* object = NULL;
    if (setting_json == NULL) {
        return;
    }
    if ((object = cJSON_GetObjectItem(setting_json, "fullscreen")) != NULL) {
        if (cJSON_IsBool(object)) {
            global_setting.fullscreen = object->valueint;
        }
    }
    if ((object = cJSON_GetObjectItem(setting_json, "music_volume")) != NULL) {
        if (cJSON_IsNumber(object)) {
            global_setting.music_volume = object->valueint;
        }
    }
    if ((object = cJSON_GetObjectItem(setting_json, "sfx_volume")) != NULL) {
        if (cJSON_IsNumber(object)) {
            global_setting.sfx_volume = object->valueint;
        }
    }
    if ((object = cJSON_GetObjectItem(setting_json, "mute_when_unfocused")) !=
        NULL) {
        if (cJSON_IsBool(object)) {
            global_setting.mute_when_unfocused = object->valueint;
        }
    }
    free(data);
    free(setting_file);
#endif
}

void SaveSetting() {
#if !defined(__PSP__)
    char* setting_file = (char*)calloc(PATH_MAX, sizeof(char));
    strcpy(setting_file, global_app.exec_path);
    strcat(setting_file, "settings.json");
    FILE* fp = fopen(setting_file, "w");
    if (fp == NULL) {
        return;
    }
    cJSON* setting_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(
        setting_json, "fullscreen", global_setting.fullscreen
    );
    cJSON_AddNumberToObject(
        setting_json, "music_volume", global_setting.music_volume
    );
    cJSON_AddNumberToObject(
        setting_json, "sfx_volume", global_setting.sfx_volume
    );
    cJSON_AddBoolToObject(
        setting_json, "mute_when_unfocused", global_setting.mute_when_unfocused
    );
    size_t len = 128;
    char* json_string = (char*)calloc(len, sizeof(char));
    while (!cJSON_PrintPreallocated(setting_json, json_string, len, 0)) {
        len *= 2 * sizeof(char);
        json_string = (char*)realloc(json_string, len);
    }
    cJSON_Delete(setting_json);
    fwrite(json_string, sizeof(char), strlen(json_string), fp);
    fwrite("\n", sizeof(char), 1, fp);
    fclose(fp);
    free(json_string);
    free(setting_file);
#endif
}
