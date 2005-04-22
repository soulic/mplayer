#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <math.h>
#include "aviheader.h"
#include "ms_hdr.h"
#include "muxer.h"
#include "ae.h"
#include "../config.h"

#ifdef HAVE_TOOLAME
#include "ae_toolame.h"
#endif

#ifdef HAVE_MP3LAME
#include "ae_lame.h"
#endif

#ifdef USE_LIBAVCODEC
#include "ae_lavc.h"
#endif

audio_encoder_t *new_audio_encoder(muxer_stream_t *stream, audio_encoding_params_t *params)
{
	int ris;
	if(! params)
		return NULL;
	
	audio_encoder_t *encoder = (audio_encoder_t *) calloc(1, sizeof(audio_encoder_t));
	memcpy(&encoder->params, params, sizeof(audio_encoding_params_t));
	encoder->stream = stream;
	
	switch(stream->codec)
	{
		case ACODEC_PCM:
			ris = mpae_init_pcm(encoder);
			break;
#ifdef HAVE_TOOLAME
		case ACODEC_TOOLAME:
			ris = mpae_init_toolame(encoder);
			break;
#endif
#ifdef USE_LIBAVCODEC
		case ACODEC_LAVC:
			ris = mpae_init_lavc(encoder);
			break;
#endif
#ifdef HAVE_MP3LAME
		case ACODEC_VBRMP3:
			ris = mpae_init_lame(encoder);
			break;
#endif
	}
	
	if(! ris)
	{
		free(encoder);
		return NULL;
	}
	encoder->bind(encoder, stream);
	encoder->decode_buffer = (int*)malloc(encoder->decode_buffer_size);
	if(! encoder->decode_buffer)
	{
		free(encoder);
		return NULL;
	}
	
	encoder->codec = stream->codec;
	return encoder;
}


