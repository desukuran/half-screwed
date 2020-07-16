//mp3 support added by Killar

#include "hud.h"
#include "cl_util.h"
#include "mp3.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  48000

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
	if (pDecoder == NULL) {
		return;
	}

	ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);

	(void)pInput;
}

int CMP3::Initialize()
{
	CVAR_CREATE("mp3_debug", "0", 0);
	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format = DEVICE_FORMAT;
	deviceConfig.playback.channels = DEVICE_CHANNELS;
	deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
	deviceConfig.dataCallback = data_callback;
	deviceConfig.pUserData = &decoder;

	if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
		gEngfuncs.Con_Printf("MP3: Failed to open playback device.\n");
		ma_decoder_uninit(&decoder);
		return -3;
	}

	gEngfuncs.Con_Printf("CMP3::Init called!\n");

	return 1;
}

int CMP3::Shutdown()
{
	ma_device_uninit(&device);
	ma_decoder_uninit(&decoder);
	return 0;
}

int CMP3::StopMP3( void )
{
	ma_device_stop(&device);
	return 1;
}

int CMP3::PlayMP3( const char *pszSong )
{
	//Kill
	if (m_iIsPlaying)
		ma_device_stop(&device);

	char song[256];

	sprintf(song, "%s/%s", gEngfuncs.pfnGetGameDirectory(), pszSong);

	if (CVAR_GET_FLOAT > 0) {
		gEngfuncs.Con_Printf("MP3 Debug: Playing song... ");
		gEngfuncs.Con_Printf(song);
	}

	result = ma_decoder_init_file(song, NULL, &decoder);
	if (result != MA_SUCCESS) {
		gEngfuncs.Con_Printf("MP3: Song not found!\n");
		m_iIsPlaying = false;
		return -2;
	}

	//Set up the audio
	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format = decoder.outputFormat;
	deviceConfig.playback.channels = decoder.outputChannels;
	deviceConfig.sampleRate = decoder.outputSampleRate;
	deviceConfig.dataCallback = data_callback;
	deviceConfig.pUserData = &decoder;

	if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
		gEngfuncs.Con_Printf("MP3: Failed to open playback device.\n");
		ma_decoder_uninit(&decoder);
		m_iIsPlaying = false;
		return -3;
	}

	if (ma_device_start(&device) != MA_SUCCESS) {
		gEngfuncs.Con_Printf("MP3: Failed to start playback device.\n");
		ma_device_uninit(&device);
		ma_decoder_uninit(&decoder);
		m_iIsPlaying = false;
		return -4;
	}

	m_iIsPlaying = true;
}

int CMP3::PlayMP3NL( const char *pszSong )
{
	//TODO: Fix. For now. This doesn't loop anyway.
	PlayMP3(pszSong);
	return 0;
}