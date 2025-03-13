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
    size_t len = 128, now = 0;
    char* setting_content = (char*)calloc(len, sizeof(char));
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        setting_content[now++] = ch;
        if (now == len) {
            len *= 2;
            setting_content =
                (char*)realloc(setting_content, len * sizeof(char));
        }
    }
    fclose(fp);
    cJSON* setting_json = cJSON_Parse(setting_content);
    cJSON* object = NULL;
    if (setting_json == NULL) {
        return;
    }
    if ((object = cJSON_GetObjectItem(setting_json, "auto_detect_size")) !=
        NULL) {
        if (cJSON_IsBool(object)) {
            global_setting.auto_detect_size = object->valueint;
        }
    }
    if ((object = cJSON_GetObjectItem(setting_json, "fullscreen")) != NULL) {
        if (cJSON_IsBool(object)) {
            global_setting.fullscreen = object->valueint;
        }
    }
    if ((object = cJSON_GetObjectItem(setting_json, "interface_size")) !=
        NULL) {
        if (cJSON_IsNumber(object)) {
            global_setting.interface_size = (float)object->valuedouble;
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
    if ((object = cJSON_GetObjectItem(setting_json, "slience")) != NULL) {
        if (cJSON_IsBool(object)) {
            global_setting.slience = object->valueint;
        }
    }
    free(setting_content);
    free(setting_file);
}

void SaveSetting() {
    char* setting_file = (char*)calloc(PATH_MAX, sizeof(char));
    strcpy(setting_file, global_app.exec_path);
    strcat(setting_file, "settings.json");
    FILE* fp = fopen(setting_file, "w");
    if (fp == NULL) {
        return;
    }
    cJSON* setting_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(
        setting_json, "auto_detect_size", global_setting.auto_detect_size
    );
    cJSON_AddBoolToObject(
        setting_json, "fullscreen", global_setting.fullscreen
    );
    cJSON_AddNumberToObject(
        setting_json, "interface_size", global_setting.interface_size
    );
    cJSON_AddNumberToObject(
        setting_json, "music_volume", global_setting.music_volume
    );
    cJSON_AddNumberToObject(
        setting_json, "sfx_volume", global_setting.sfx_volume
    );
    cJSON_AddBoolToObject(setting_json, "slience", global_setting.slience);
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
}
