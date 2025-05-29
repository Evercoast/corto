/*
* **********************************************
* Copyright Evercoast, Inc. All Rights Reserved.
* **********************************************
* @Author: Ye Feng
* @Date:   2024-02-17 16:35:49
* @Last Modified by:   feng_ye
* @Last Modified time: 2024-06-17 10:17:20
*/

#include <memory>
// PREVENT HIJACKING??? IT SOMEHOW DOES WORK IN IOS
void* operator new(std::size_t size) {
    return std::malloc(size);
}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}

#include "corto_decoder_c.h"
#include "decoder.h"

using namespace crt;


void *Corto_CreateDecoder(int length, const unsigned char* data, Corto_DecoderInfo* decoderInfo)
{
    
    try {
        
        Decoder* decoder = new Decoder(length, data);
        
        decoderInfo->nvert = decoder->nvert;
        decoderInfo->nface = decoder->nface;
        decoderInfo->hasNormal = decoder->hasAttr("normal") ? 1 : 0;
        
        return decoder;
    }
    catch (...) {
        return nullptr;
    }
}
 
void Corto_DestroyDecoder(void* decoder_handler)
{
    try
    {
        
        
        // Turns out C++ cannot properly delete a void pointer. You have to cast it to something C++-ish
        Decoder* decoder = (Decoder*)decoder_handler;
        delete decoder;
    }
    catch (...)
    {
        return;
    }
}

int Corto_DecodeMesh(void* decoder_handler, Corto_Vector3* vertices, uint32_t* indices, Corto_Vector3* normals, Corto_Color* colors, Corto_Vector2* texcoord)
{
    try
    {
        Decoder* decoder = (Decoder*)decoder_handler;
        
        if (!decoder || decoder->nface == 0)
        {
            return -1;
        }
        else
        {
            decoder->setIndex(indices);
        }
        
        if (decoder->nvert > 0)
        {
            decoder->setPositions((float*)vertices);
        }
        
        if (decoder->hasAttr("normal"))
        {
            decoder->setNormals((float*)normals);
        }
        
        if (decoder->hasAttr("color"))
        {
            decoder->setAttribute("color", (char*)colors, VertexAttribute::FLOAT);
        }
        
        if (decoder->hasAttr("uv"))
        {
            decoder->setUvs((float*)texcoord);
        }
        
        decoder->decode();
        
        return decoder->nface;
    }
    catch(...)
    {
        return 0;
    }
}
