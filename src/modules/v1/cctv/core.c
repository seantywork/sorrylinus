#include "sorrylinus/modules/v1/cctv/core.h"


pthread_t CCTV_ID;

int CCTV_STATUS = CCTV_READY;

char CCTV_MESSAGE[CCTV_THREAD_MSG_LEN] = {0};

void cctv_stream_open(char* result, char* conf, char* stream_key){


    CCTV_STREAM_ARG sarg;

    strcpy(sarg.stream_key, stream_key);


    if(CCTV_STATUS == CCTV_READY){

        pthread_create(&CCTV_ID, NULL, start_cctv_stream, (void*)&sarg);

    } else if (CCTV_STATUS == CCTV_STREAMING) {

        pthread_cancel(CCTV_ID);


    } else {




    }









}


void* start_cctv_stream(void* varg){

    CCTV_STREAM_ARG* cctv_stream_arg = (CCTV_STREAM_ARG*)varg;

    guint bus_watch_id;
    GMainLoop *loop = NULL;
    // GstElement *bin, *pipeline, *source, *convert1,*filter1,*convert2,*filter2,*convert3, *muxsink,*infer,*tiler,*transform,*sink,*source1,*filter_1, *filter_2, *convert_1,*source_fake,
    //*sink_fake ,*queue1;

    GstElement *bin, *pipeline, *source, *queue1, *convert1, *scale1, *enc1, *mux1, *queue2, *sink1;

    GstBus *bus;
    //GstCaps *caps1,*caps2,*caps_1,*caps_2;

    GstElement *filter1;
    GstCaps *caps1;

    GstStateChangeReturn ret;

    int argc = 1;
    char argv[1][12] = {
        "cctv-stream"
    };

    gst_init(&argc, &argv);

    loop = g_main_loop_new (NULL, FALSE);

    pipeline = gst_pipeline_new("soli-pipeline");

    source = gst_element_factory_make("v4l2src", "source");

    queue1 = gst_element_factory_make("queue","queue1");

    convert1 = gst_element_factory_make("videoconvert", "convert1");

    scale1 = gst_element_factory_make("videoscale","scale1");

    filter1 = gst_element_factory_make("capsfilter", "filter1");



    enc1 = gst_element_factory_make("x264enc", "enc1");


    mux1 = gst_element_factory_make("flvmux","mux1");



    queue2 = gst_element_factory_make("queue", "queue2");



    sink1 = gst_element_factory_make("rtmpsink","sink1");


    //gst_bin_add (GST_BIN (pipeline), source1);
    //gst_bin_add (GST_BIN (pipeline), filter_1);
    //gst_bin_add (GST_BIN (pipeline), filter_2);
    //gst_bin_add (GST_BIN (pipeline), convert_1);

    //gst_bin_add (GST_BIN (pipeline), tiler);

    if (!pipeline || !source || !queue1 || !convert1 || !scale1 
        || !filter1 || !enc1 || !mux1 || !queue2 || !sink1
    ){
        g_printerr ("one element could not be created. Exiting.\n");
        return -1;
    }



    gst_bin_add_many(GST_BIN(pipeline), source, queue1, convert1, scale1, filter1, enc1, mux1, queue2, sink1, NULL);


    if (!gst_element_link_many(source, queue1, convert1, scale1, filter1, enc1, mux1, queue2, sink1,NULL))
    {
        g_printerr ("elements could not be linked: 1. Exiting.\n");
        return -1;
    }

    caps1 = gst_caps_new_simple("video/x-raw",
        "width", G_TYPE_INT, 960,
        "height", G_TYPE_INT, 720,
        NULL);


    g_object_set(G_OBJECT(source), "device","/dev/video0", NULL);


    g_object_set(G_OBJECT(filter1), "caps", caps1, NULL);

    g_object_set(G_OBJECT(enc1), "speed-preset", ENC_SPEED_PRESET_ULTRAFAST, NULL);

    g_object_set(G_OBJECT(enc1), "tune", ENC_TUNE_ZEROLATENCY, NULL);

    g_object_set(G_OBJECT(enc1), "key-int-max",20, NULL);

    g_object_set(G_OBJECT(mux1), "streamable",1, NULL);

    g_object_set(G_OBJECT(sink1), "location", cctv_stream_arg->stream_key, NULL);


    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

    g_main_loop_run (loop);

    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);

    return NULL;  
}


gboolean bus_call (GstBus * bus, GstMessage * msg, gpointer data)
{

  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {

    case GST_MESSAGE_EOS:
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
      break;

    case GST_MESSAGE_ERROR:
      gchar *debug;
      GError *error;
      gst_message_parse_error (msg, &error, &debug);
      g_printerr ("ERROR from element %s: %s\n",
      GST_OBJECT_NAME (msg->src), error->message);
      if (debug)
      g_printerr ("Error details: %s\n", debug);
      g_free (debug);
      g_error_free (error);
      g_main_loop_quit (loop);
      break;

    default:
      break;
  }

  return TRUE;
}