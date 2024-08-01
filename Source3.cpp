#include <gst/gst.h>
#include <iostream>

int main(int argc, char* argv[]) {
    GstElement* pipeline, * src, * decodebin, * videoconvert, * sink;
    GstBus* bus;
    GstMessage* msg;
    GstStateChangeReturn ret;

    gst_init(&argc, &argv);

    pipeline = gst_pipeline_new("video-pipeline");
    src = gst_element_factory_make("filesrc", "source");
    decodebin = gst_element_factory_make("decodebin", "decoder");
    videoconvert = gst_element_factory_make("videoconvert", "videoconvert");
    sink = gst_element_factory_make("autovideosink", "videosink");

    if (!pipeline || !src || !decodebin || !videoconvert || !sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    g_object_set(src, "location", "C:\Users\Asus\Videos\InShot_20230708_022048920.mp4", NULL);

    g_object_set(sink, "host", "192.168.1.34", "port", 5060, NULL);

    gst_bin_add_many(GST_BIN(pipeline), src, decodebin, videoconvert, sink, NULL);
    gst_element_link(src, decodebin);
    g_signal_connect(decodebin, "pad-added", G_CALLBACK(on_pad_added), videoconvert);
    gst_element_link_many(videoconvert, sink, NULL);

    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Failed to start the pipeline.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    if (msg != NULL) {
        GError* err;
        gchar* debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            break;
        default:
            g_printerr("Unexpected message received.\n");
            break;
        }
        gst_message_unref(msg);
    }

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}

static void on_pad_added(GstElement* src, GstPad* new_pad, GstElement* videoconvert) {
    GstPad* sink_pad = gst_element_get_static_pad(videoconvert, "sink");
    if (gst_pad_is_linked(sink_pad)) {
        g_object_unref(sink_pad);
        return;
    }

    GstPadLinkReturn ret = gst_pad_link(new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED(ret)) {
        g_printerr("Failed to link pads\n");
    }

    g_object_unref(sink_pad);
}
