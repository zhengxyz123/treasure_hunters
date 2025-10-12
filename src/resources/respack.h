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

#ifndef TH_RESOURCES_RESPACK_H_
#define TH_RESOURCES_RESPACK_H_

#include <stdint.h>
#include <stdio.h>

#define FNV1_32_INIT ((uint32_t)0x811c9dc5)
#define FNV1_32_PRIME ((uint32_t)0x01000193)

// please note this alignment, it should have the same value as `_pack_` in
// `tools/respack.py`
#pragma pack(8)

typedef struct RespackHeader{
    char magic[4];
    uint8_t version;
    uint16_t entry_count;
    uint32_t key_index_offset;
    uint32_t value_index_offset;
} RespackHeader;

typedef struct RespackEntry {
    uint32_t key_offset;
    uint8_t key_length;
    uint32_t key_hash;
    uint32_t value_offset;
    uint32_t value_length;
} RespackEntry;

#pragma pack()

typedef struct Respack {
    FILE* fp;
    RespackHeader header;
    RespackEntry* entries;
} Respack;

uint32_t fnv1a_32(char* str, uint32_t hval);
Respack* LoadRespack(char* filename);
int RespackHasItem(Respack* rpkg, char* key, size_t* index);
void* RespackGetItem(Respack* rpkg, char* key, size_t* length);
void FreeRespack(Respack* rpkg);

#endif
