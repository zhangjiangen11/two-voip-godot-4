/**************************************************************************/
/*  audio_effect_opus_chunked.h                                                */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef AUDIO_EFFECT_OPUS_CHUNKED_H
#define AUDIO_EFFECT_OPUS_CHUNKED_H

#define OVR_LIP_SYNC

#include <godot_cpp/classes/audio_effect.hpp>
#include <godot_cpp/classes/audio_effect_instance.hpp>
#include <godot_cpp/classes/audio_frame.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/node.hpp>

#include "opus.h"
#include "speex_resampler/speex_resampler.h"

#ifdef OVR_LIP_SYNC
#include "OVRLipSync.h"
#endif

namespace godot {

class AudioEffectOpusChunked;

class AudioEffectOpusChunkedInstance : public AudioEffectInstance {
	GDCLASS(AudioEffectOpusChunkedInstance, AudioEffectInstance);
	friend class AudioEffectOpusChunked;
	Ref<AudioEffectOpusChunked> base;

protected:
	static void _bind_methods() {;};

public:
	virtual void _process(const void *src_buffer, AudioFrame *p_dst_frames, int p_frame_count) override; 
	virtual bool _process_silence() const override { return true; }
};

class AudioEffectOpusChunked : public AudioEffect {
	GDCLASS(AudioEffectOpusChunked, AudioEffect)
	friend class AudioEffectOpusChunkedInstance;

    int audiosamplerate = 44100;
    int audiosamplesize = 881;
    int audiosamplechunks = 50;

	PackedVector2Array audiosamplebuffer;
	int chunknumber = -1;
	int bufferend = 0;
	int discardedchunks = 0;

    int opussamplerate = 48000;
    int opusframesize = 960;
    int opusbitrate = 24000;

    SpeexResamplerState* speexresampler = NULL;
	PackedVector2Array audioresampledbuffer;
    OpusEncoder* opusencoder = NULL;
	PackedByteArray opusbytebuffer;

    ovrLipSyncContext* pctx = NULL;
    ovrLipSyncFrame* pframe = NULL;
    PackedFloat32Array visemes; 

	void process(const AudioFrame *p_src_frames, AudioFrame *p_dst_frames, int p_frame_count);

protected:
	static void _bind_methods();

public:
	virtual Ref<AudioEffectInstance> _instantiate() override;

	void createencoder();
	void resetencoder(int Dreason=3);

	bool chunk_available();
	float chunk_max();
	float chunk_rms();
	void drop_chunk();
	PackedVector2Array read_chunk();
	PackedByteArray pop_opus_packet(const PackedByteArray& prefixbytes);
	PackedByteArray chunk_to_opus_packet(const PackedByteArray& prefixbytes, const PackedVector2Array& audiosamplebuffer, int begin);
    int chunk_to_lipsync(); 
	
    void set_opussamplerate(int lopussamplerate) { chunknumber = -1; opussamplerate = lopussamplerate; };
    int get_opussamplerate() { return opussamplerate; };
    void set_opusframesize(int lopusframesize) { chunknumber = -1; opusframesize = lopusframesize; };
    int get_opusframesize() { return opusframesize; };
    void set_opusbitrate(int lopusbitrate) { chunknumber = -1; opusbitrate = lopusbitrate; };
    int get_opusbitrate() { return opusbitrate; };
    void set_audiosamplerate(int laudiosamplerate) { chunknumber = -1; audiosamplerate = laudiosamplerate; };
    int get_audiosamplerate() { return audiosamplerate; };
    void set_audiosamplesize(int laudiosamplesize) { chunknumber = -1; audiosamplesize = laudiosamplesize; };
    int get_audiosamplesize() { return audiosamplesize; };
    void set_audiosamplechunks(int laudiosamplechunks) { chunknumber = -1; audiosamplechunks = laudiosamplechunks; };
    int get_audiosamplechunks() { return audiosamplechunks; };

	AudioEffectOpusChunked() {;};
	~AudioEffectOpusChunked() { resetencoder(17); };
};

}

#endif // AUDIO_EFFECT_OPUS_CHUNKED_H
