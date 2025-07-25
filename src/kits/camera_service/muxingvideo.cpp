#include "kits/camera_service/muxingvideo.h"
#include <codecvt>
#include <iostream>
#include <qlogging.h>

#define SCALE_FLAGS SWS_BICUBIC
#define STREAM_DURATION 10.0
#define STREAM_FRAME_RATE 25

using namespace _Kits;
MuxingVideo::~MuxingVideo() {
  Release();
  m_initialized = false;
}

void MuxingVideo::finished() {
  if (!m_initialized) {
    return;
  }

  av_write_trailer(m_outformt_ctx);
  m_initialized = false;
  Release();
}

void MuxingVideo::Release() {
  if (nullptr != m_outformt_ctx &&
      !(m_outformt_ctx->oformat->flags & AVFMT_NOFILE)) {
    avio_close(m_outformt_ctx->pb);
  }

  for (auto &it : m_outputstream) {
    if (nullptr != it) {
      if (1 != it->rkflag) {
        if (nullptr != it->codecContext) {
          avcodec_free_context(&it->codecContext);
          it->codecContext = nullptr;
        }
      }
      if (nullptr != it->codecContext) {
        avcodec_close(it->codecContext);
        it->codecContext = nullptr;
      }
    }
  }
  m_outputstream.clear();
  if (nullptr != m_outformt_ctx) {
    avformat_free_context(m_outformt_ctx);
    m_outformt_ctx = nullptr;
  }
}

void MuxingVideo::writeVideo(int no, AVPacket &pkt, int rkflag) {
  if (nullptr == &pkt) {\
    qDebug("pkt is nullptr");
  }
  std::lock_guard<std::mutex> lock(m_mtx);
  if (nullptr == m_outputstream[no]) {
    qDebug("m_outputstream is nullptr");
    return;
  }
  if (1 != rkflag) {
    av_packet_rescale_ts(&pkt, m_outputstream[no]->codecContext->time_base,
                         m_outputstream[no]->stream->time_base);
    pkt.stream_index = m_outputstream[no]->stream->index;
  }
  int ret = av_interleaved_write_frame(m_outformt_ctx, &pkt);
  if (ret < 0) {
    qDebug("error while writing output packet: %d", ret);
    return;
  }
  return;
}

int MuxingVideo::Init(const char *filename, std::vector<EncodeVideo::InputInfo> inputinfo) {

  //     std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  //     // std::wstring wide_path = filename;
  //     // std::cout << wide_path << std::endl;
  // 	std::wstring wide_str = converter.from_bytes(filename);
  //    std::string utf8_path = converter.to_bytes(wide_str);
  //     std::cout << utf8_path << std::endl;
  // AVFormatContext *oc;
  if (inputinfo.size() > 1) {
    avformat_alloc_output_context2(&m_outformt_ctx, NULL, "mov", filename);
    if (!m_outformt_ctx) {
      qDebug("Could not deduce output format from file extension: using MPEG");
      avformat_alloc_output_context2(&m_outformt_ctx, NULL, "mpeg", filename);
    }
    if (nullptr == m_outformt_ctx) {
      return 1;
    }
  } else {
    // std::cout << filename << std::endl;
    avformat_alloc_output_context2(&m_outformt_ctx, NULL, "mp4", filename);
    if (!m_outformt_ctx) {
      qDebug("Could not deduce output format from file extension: null");
      avformat_alloc_output_context2(&m_outformt_ctx, NULL, NULL, filename);
    }
    if (nullptr == m_outformt_ctx) {
      return 1;
    }
  }
  AVDictionary *opt = NULL;
  const AVOutputFormat *fmt = m_outformt_ctx->oformat;
  if (fmt->video_codec != AV_CODEC_ID_NONE) {
    for (auto &it : inputinfo) {
      auto outstreamptr = std::make_shared<OutputStream>();
      if (0 == it.rkencode) {
        RKAddStream(outstreamptr, it);
      } else {
        AddStream(outstreamptr, it);
      }
      m_outputstream.emplace_back(std::move(outstreamptr));
    }
  }

  av_dump_format(m_outformt_ctx, 0, filename, 1);
  // std::cout <<"!!!!" << filename << std::endl;
  if (!(fmt->flags & AVFMT_NOFILE)) {
    int ret = avio_open(&m_outformt_ctx->pb, filename, AVIO_FLAG_WRITE);
    if (ret < 0) {
      qDebug("Could not open '%s': %d", filename, ret);
      return 1;
    }
  }

  int ret = avformat_write_header(m_outformt_ctx, NULL);
  if (ret < 0) {
    qDebug("avformat_write_header error: %d", ret);
    return 1;
  }
  m_initialized = true;
  return 0;
}

void MuxingVideo::AddStream(std::shared_ptr<OutputStream> optstream,
                            EncodeVideo::InputInfo &inputinfo) {

  optstream->encodecodec = const_cast<AVCodec *>(
      avcodec_find_encoder((AVCodecID)inputinfo.avcodeid));
  if (nullptr == optstream->encodecodec) {
    qDebug("Could not find encoder for %s",
                 avcodec_get_name((AVCodecID)inputinfo.avcodeid));
    return;
  }

  optstream->stream = avformat_new_stream(m_outformt_ctx, NULL);
  if (nullptr == optstream->stream) {
    qDebug("Could not allocate stream\n");
    return;
  }
  optstream->stream->id = m_outformt_ctx->nb_streams - 1;
  optstream->codecContext = avcodec_alloc_context3(optstream->encodecodec);
  if (nullptr == optstream->codecContext) {
    qDebug("Could not alloc an encoding context\n");
    return;
  }

  switch ((optstream->encodecodec)->type) {
  case AVMEDIA_TYPE_VIDEO: {
    optstream->codecContext->codec_id = (AVCodecID)inputinfo.avcodeid;
   //optstream->codecContext->bit_rate = 40000000;
    optstream->codecContext->width = inputinfo.width;
    optstream->codecContext->height = inputinfo.height;
    // optstream->stream->time_base = (AVRational){ 1, inputinfo.fps };
    optstream->stream->time_base.den = inputinfo.fps;
    optstream->stream->time_base.num = 1;
    optstream->codecContext->time_base = optstream->stream->time_base;
    optstream->codecContext->gop_size =
        inputinfo.fps /
        2; /* emit one intra frame every twelve frames at most */
    optstream->codecContext->pix_fmt = (AVPixelFormat)inputinfo.pixfmt;
    if (optstream->codecContext->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
      optstream->codecContext->max_b_frames = 2;
    }
    break;
  }
  default:
    break;
  }
  if (m_outformt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
    optstream->codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  }

  AVDictionary *opt = NULL;
  av_dict_copy(&opt, nullptr, 0);
  int ret =
      avcodec_open2(optstream->codecContext, optstream->encodecodec, &opt);
  // int ret = avcodec_open2(codecContext, encodecodec, nullptr);
  av_dict_free(&opt);
  if (ret < 0) {
    qDebug("Could not open video codec: %d", ret);
    return;
  }

  ret = avcodec_parameters_from_context(optstream->stream->codecpar,
                                        optstream->codecContext);
  if (ret < 0) {
    qDebug("Could not copy the stream parameters\n");
    return;
  }
}

void MuxingVideo::RKAddStream(std::shared_ptr<OutputStream> optstream,
                              EncodeVideo::InputInfo &inputinfo) {
  optstream->encodecodec = nullptr;
  optstream->rkflag = 1;
  optstream->stream = avformat_new_stream(m_outformt_ctx, NULL);
  if (nullptr == optstream->stream) {
    qDebug("Could not allocate stream");
    return;
  }
  optstream->stream->id = m_outformt_ctx->nb_streams - 1;
  optstream->codecContext = avcodec_alloc_context3(nullptr);
  if (nullptr == optstream->codecContext) {
    qDebug("Could not alloc an encoding context");
    return;
  }

  optstream->codecContext->codec_id = (AVCodecID)inputinfo.avcodeid;
  // optstream->codecContext->bit_rate = 400000;
  optstream->codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
  optstream->codecContext->width = inputinfo.width;
  optstream->codecContext->height = inputinfo.height;
  optstream->codecContext->codec_tag = 0;
  // optstream->stream->time_base = (AVRational){ 1, inputinfo.fps };
  optstream->stream->time_base.den = inputinfo.fps;
  optstream->stream->time_base.num = 1;
  optstream->codecContext->time_base = optstream->stream->time_base;
  // optstream->codecContext->gop_size      = inputinfo.fps / 2; /* emit one
  // intra frame every twelve frames at most */
  optstream->codecContext->pix_fmt = (AVPixelFormat)inputinfo.pixfmt;

  if (inputinfo.size > 0) {
    optstream->codecContext->extradata = inputinfo.data;
    optstream->codecContext->extradata_size = inputinfo.size;
    // std::cout << "AddStream1111 rk:" <<
    // optstream->codecContext->extradata_size <<":"  << "\r\n";
  }
  if (m_outformt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
    optstream->codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  }

  int ret = avcodec_parameters_from_context(optstream->stream->codecpar,
                                            optstream->codecContext);
  if (ret < 0) {
    qDebug("Could not copy the stream parameters\n");
    return;
  }
  return;
}
