#include <common/buffer.h>

#include <event/action.h>
#include <event/callback.h>
#include <event/event_system.h>

#include <io/pipe.h>

#include <xcodec/xcodec.h>
#include <xcodec/xcodec_decoder.h>
#include <xcodec/xcodec_decoder_pipe.h>
#include <xcodec/xcodec_encoder.h>

XCodecDecoderPipe::XCodecDecoderPipe(XCodec *codec)
: PipeSimple("/xcodec/decoder/pipe"),
  decoder_(codec)
{ }

XCodecDecoderPipe::~XCodecDecoderPipe()
{ }

bool
XCodecDecoderPipe::process(Buffer *out, Buffer *in)
{
	return (decoder_.decode(out, in));
}

bool
XCodecDecoderPipe::process_eos(void) const
{
	if (decoder_.encoder_->sent_eos_ &&
	    decoder_.encoder_->received_eos_ &&
	    decoder_.asked_.empty())
		return (true);
	return (false);
}
