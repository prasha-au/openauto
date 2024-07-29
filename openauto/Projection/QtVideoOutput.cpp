/*
*  This file is part of openauto project.
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  openauto is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  openauto is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with openauto. If not, see <http://www.gnu.org/licenses/>.
*/

#include "aasdk/Common/Data.hpp"
#include "openauto/Projection/QtVideoOutput.hpp"
#include "OpenautoLog.hpp"
#include <QTimer>
// these are needed only for pretty printing of data, to be removed
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

namespace openauto
{
namespace projection
{

QtVideoOutput::QtVideoOutput(configuration::IConfiguration::Pointer configuration, QWidget* videoContainer)
    : VideoOutput(std::move(configuration))
    , videoContainer_(videoContainer)
{
    this->moveToThread(QApplication::instance()->thread());
    videoWidget_ = new QQuickWidget(videoContainer_);

    QGst::Quick::VideoSurface* surface = new QGst::Quick::VideoSurface;
    videoWidget_->rootContext()->setContextProperty(QLatin1String("videoSurface"), surface);
    videoWidget_->setSource(QUrl("qrc:/aa_video.qml"));
    videoWidget_->setResizeMode(QQuickWidget::SizeRootObjectToView);

    auto videoSink = surface->videoSink();

    GError* error = nullptr;
    std::string vidLaunchStr = "appsrc name=aasrc is-live=true block=false max-latency=100 do-timestamp=true stream-type=stream ! queue ! h264parse ! capssetter caps=\"video/x-h264,colorimetry=bt709\" ! ";
    vidLaunchStr += ToPipeline(findPreferredVideoDecoder());
    vidLaunchStr += " ! capsfilter caps=video/x-raw name=mycapsfilter";

    vidPipeline_ = gst_parse_launch(vidLaunchStr.c_str(), &error);
    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(vidPipeline_));
    gst_bus_add_watch(bus, (GstBusFunc)&QtVideoOutput::busCallback, this);
    gst_object_unref(bus);

    GstElement* sink = QGlib::RefPointer<QGst::Element>(videoSink);
    g_object_set(sink, "force-aspect-ratio", false, nullptr);
    g_object_set(sink, "sync", false, nullptr);
    g_object_set(sink, "async", false, nullptr);

    GstElement* capsFilter = gst_bin_get_by_name(GST_BIN(vidPipeline_), "mycapsfilter");
    gst_bin_add(GST_BIN(vidPipeline_), GST_ELEMENT(sink));
    gst_element_link(capsFilter, GST_ELEMENT(sink));

    vidSrc_ = GST_APP_SRC(gst_bin_get_by_name(GST_BIN(vidPipeline_), "aasrc"));
    gst_app_src_set_stream_type(vidSrc_, GST_APP_STREAM_TYPE_STREAM);

    connect(this, &QtVideoOutput::startPlayback, this, &QtVideoOutput::onStartPlayback, Qt::QueuedConnection);
    connect(this, &QtVideoOutput::stopPlayback, this, &QtVideoOutput::onStopPlayback, Qt::QueuedConnection);
}

QtVideoOutput::~QtVideoOutput()
{
    gst_object_unref(vidPipeline_);
    gst_object_unref(vidSrc_);
}


H264_Decoder QtVideoOutput::findPreferredVideoDecoder()
{
    for (H264_Decoder decoder : H264_Decoder_Priority_List) {
        GstElementFactory *decoder_factory = gst_element_factory_find (ToString(decoder));
        if(decoder_factory != nullptr){
            gst_object_unref(decoder_factory);
            OPENAUTO_LOG(info) << "[QtVideoOutput] Selecting the " << ToString(decoder) << " h264 decoder";
            return decoder;
        }
    }
    OPENAUTO_LOG(error) << "[QtVideoOutput] Couldn't find a decoder to use!";
    return H264_Decoder::unknown;
}

void QtVideoOutput::dumpDot()
{
    gst_debug_bin_to_dot_file(GST_BIN(vidPipeline_), GST_DEBUG_GRAPH_SHOW_VERBOSE, "pipeline");
    OPENAUTO_LOG(info) << "[QtVideoOutput] Dumped dot debug info";
}

gboolean QtVideoOutput::busCallback(GstBus*, GstMessage* message, gpointer*)
{
    gchar* debug;
    GError* err;
    gchar* name;

    switch(GST_MESSAGE_TYPE(message))
    {
    case GST_MESSAGE_ERROR:
        gst_message_parse_error(message, &err, &debug);
        OPENAUTO_LOG(info) << "[QtVideoOutput] Error " << err->message;
        g_error_free(err);
        g_free(debug);
        break;
    case GST_MESSAGE_WARNING:
        gst_message_parse_warning(message, &err, &debug);
        OPENAUTO_LOG(info) << "[QtVideoOutput] Warning " << err->message << " | Debug " << debug;
        name = (gchar*)GST_MESSAGE_SRC_NAME(message);
        OPENAUTO_LOG(info) << "[QtVideoOutput] Name of src " << (name ? name : "nil");
        g_error_free(err);
        g_free(debug);
        break;
    case GST_MESSAGE_EOS:
        OPENAUTO_LOG(info) << "[QtVideoOutput] End of stream";
        break;
    case GST_MESSAGE_STATE_CHANGED:
    default:
        break;
    }

    return TRUE;
}

bool QtVideoOutput::open()
{
    GstElement* capsFilter = gst_bin_get_by_name(GST_BIN(vidPipeline_), "mycapsfilter");
    GstPad* convertPad = gst_element_get_static_pad(capsFilter, "sink");
    gst_pad_add_probe(convertPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &QtVideoOutput::convertProbe, this, nullptr);
    gst_element_set_state(vidPipeline_, GST_STATE_PLAYING);

    return true;
}

GstPadProbeReturn QtVideoOutput::convertProbe(GstPad* pad, GstPadProbeInfo* info, void*)
{
    GstEvent* event = GST_PAD_PROBE_INFO_EVENT(info);
    if(GST_PAD_PROBE_INFO_TYPE(info) & GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM)
    {
        if(GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT)
        {
            GstCaps* caps  = gst_pad_get_current_caps(pad);
            if(caps != nullptr)
            {
                GstVideoInfo* vinfo = gst_video_info_new();
                gst_video_info_from_caps(vinfo, caps);
                OPENAUTO_LOG(info) << "[QtVideoOutput] Video Width: " << vinfo->width;
                OPENAUTO_LOG(info) << "[QtVideoOutput] Video Height: " << vinfo->height;
            }

            return GST_PAD_PROBE_REMOVE;
        }
    }

    return GST_PAD_PROBE_OK;
}

bool QtVideoOutput::init()
{
    OPENAUTO_LOG(info) << "[QtVideoOutput] init";
    emit startPlayback();

    return true;
}

void QtVideoOutput::write(uint64_t timestamp, const aasdk::common::DataConstBuffer& buffer)
{

    GstBuffer* buffer_ = gst_buffer_new_and_alloc(buffer.size);
    gst_buffer_fill(buffer_, 0, buffer.cdata, buffer.size);
    int ret = gst_app_src_push_buffer((GstAppSrc*)vidSrc_, buffer_);
    if(ret != GST_FLOW_OK)
    {
        OPENAUTO_LOG(info) << "[QtVideoOutput] push buffer returned " << ret << " for " << buffer.size << "bytes";
    }

}

void QtVideoOutput::onStartPlayback()
{
    if(videoContainer_ == nullptr)
    {
        OPENAUTO_LOG(info) << "[QtVideoOutput] No video container, setting projection fullscreen";
        videoWidget_->setFocus();
        videoWidget_->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        videoWidget_->showFullScreen();
    }
    else
    {
        OPENAUTO_LOG(info) << "[QtVideoOutput] Resizing to video container";
        videoWidget_->resize(videoContainer_->size());
    }
    videoWidget_->show();
    QTimer::singleShot(10000, this, SLOT(dumpDot()));
}

void QtVideoOutput::stop()
{
    emit stopPlayback();
}

void QtVideoOutput::onStopPlayback()
{
    OPENAUTO_LOG(info) << "[QtVideoOutput] stop.";
    gst_element_set_state(vidPipeline_, GST_STATE_PAUSED);
    videoWidget_->hide();
}

}
}
