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

#ifndef _TH_UI_WIDGET_H_
#define _TH_UI_WIDGET_H_

#include <SDL.h>

typedef struct {
    int is_dragging;
    float min;
    float max;
    float now;
} SliderData;

void InitWidget();
void QuitWidget();
void ClearWidgets();
void HandleWidgetEvent(SDL_Event* event);
void TickWidgets(float dt);
void WidgetBegin();
int WidgetIsHovering();
void CalcButtonTextSize(char* str, float* w, float* h);
int WidgetButton(float x, float y, char* str, int disabled);
int WidgetOption(float x, float y, int* data);
int WidgetSlider(float x, float y, float w, float h, SliderData* data);
void WidgetEnd();

#endif
