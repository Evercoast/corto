/*
* **********************************************
* Copyright Evercoast, Inc. All Rights Reserved.
* **********************************************
* @Author: Ye Feng
* @Date:   2024-02-17 16:35:49
* @Last Modified by:   feng_ye
* @Last Modified time: 2024-02-17 16:48:56
*/

#include "corto_decoder_c.h"
#include "decoder.h"

using namespace crt;

void *Corto_CreateDecoder(int length, const unsigned char* data, Corto_DecoderInfo* decoderInfo)
{
    Decoder* decoder = new Decoder(length, data);
    
    decoderInfo->nvert = decoder->nvert;
    decoderInfo->nface = decoder->nface;   
    decoderInfo->hasNormal = decoder->data.count("normal") > 0 ? 1 : 0;
    
    return decoder;
}
 
void Corto_DestroyDecoder(void* decoder_handler)
{
    delete decoder_handler;
}

int Corto_DecodeMesh(void* decoder_handler, Corto_Vector3* vertices, uint32_t* indices, Corto_Vector3* normals, Corto_Color* colors, Corto_Vector2* texcoord)
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

    if (decoder->data.count("normal") > 0)
    {
        decoder->setNormals((float*)normals);
    }

    if (decoder->data.count("color") > 0)
    {
        decoder->setAttribute("color", (char*)colors, VertexAttribute::FLOAT);
    }

    if (decoder->data.count("uv") > 0)
    {
        decoder->setUvs((float*)texcoord);
    }

    decoder->decode();

    return decoder->nface;
}