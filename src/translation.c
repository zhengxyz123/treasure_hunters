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

#include "translation.h"
#include "global.h"
#include "resources/respack.h"
#include "ui/text/ttf.h"
#include <stdlib.h>

cJSON *lang_pack, *fallback_pack;
extern GameApp game_app;

cJSON* LoadTranslation(char* lang) {
    char* filename = calloc(32, sizeof(char));
#if defined(TH_FALLBACK_TO_BITMAP_FONT)
    snprintf(filename, 32, "i18n/%s.json", "en_us");
#else
    snprintf(filename, 32, "i18n/%s.json", lang);
#endif
    size_t size;
    void* content = RespackGetItem(game_app.assets_pack, filename, &size);
    free(filename);
    return cJSON_ParseWithLength(content, size);
}

void InitTranslation() {
    fallback_pack = LoadTranslation("en_us");
}

void QuitTranslation() {
    cJSON_Delete(lang_pack);
    cJSON_Delete(fallback_pack);
}

void TranslationSetLanguage(char* lang) {
#if !defined(TH_FALLBACK_TO_BITMAP_FONT)
    lang_pack = LoadTranslation(lang);
    if (strcmp(lang, "zh_cn") == 0) {
        ReloadFont(FONTFACE_NOTOCJK_SC);
    } else if (strcmp(lang, "zh_tw") == 0) {
        ReloadFont(FONTFACE_NOTOCJK_TC);
    } else if (strcmp(lang, "zh_hk") == 0) {
        ReloadFont(FONTFACE_NOTOCJK_HK);
    } else {
        ReloadFont(FONTFACE_NOTOCJK_JP);
    }
#endif
}

int TranslationHasItem(char* key) {
    return cJSON_HasObjectItem(lang_pack, key) ||
           cJSON_HasObjectItem(fallback_pack, key);
}

char* TransaltionGetText(char* key) {
    if (!cJSON_HasObjectItem(lang_pack, key)) {
        if (!cJSON_HasObjectItem(fallback_pack, key)) {
            return key;
        } else {
            cJSON* obj = cJSON_GetObjectItem(fallback_pack, key);
            return cJSON_GetStringValue(obj);
        }
    } else {
        cJSON* obj = cJSON_GetObjectItem(lang_pack, key);
        return cJSON_GetStringValue(obj);
    }
}
