
#include "sorrylinus/modules/v1/cctv/core.h"


pthread_t CCTV_ID;

int CCTV_STATUS = CCTV_READY;

char CCTV_MESSAGE[CCTV_THREAD_MSG_LEN] = {0};

void cctv_stream_toggle(char* result, char* stream_key){


    CCTV_STREAM_ARG sarg;

    sarg.stream_key = stream_key;

    int was_ready = 0;
    int was_streaming = 0;

    memset(CCTV_MESSAGE, 0, CCTV_THREAD_MSG_LEN);

    printf("stream toggle: %s\n", sarg.stream_key);

    if(CCTV_STATUS == CCTV_READY){

        was_ready = 1;

        pthread_create(&CCTV_ID, NULL, start_cctv_stream, (void*)&sarg);


    } else if (CCTV_STATUS == CCTV_STREAMING) {

        was_streaming = 1;

        pthread_cancel(CCTV_ID);

    } else {

        CCTV_STATUS = CCTV_READY;

        strcpy(result, "cctv: stream: failed to toggle: unknown state\n");

        return;
    }

    printf("stream toggle: action initiated\n");

    int ms_until_deadline = 0;

    struct timespec rnow;

    clock_gettime(CLOCK_MONOTONIC_RAW, &rnow);

    struct timespec rdeadline;

    while(1){

        clock_gettime(CLOCK_MONOTONIC_RAW, &rdeadline);

        ms_until_deadline = ((rdeadline.tv_sec - rnow.tv_sec) * 1000 + (rdeadline.tv_nsec - rnow.tv_nsec) / 1000000);

        if(ms_until_deadline > SOLIMOD_TIMEOUT_MS){

            strcpy(result, "cctv: stream: toggle failed: timed out\n");

            break;

        }

        if(was_ready == 1 && CCTV_STATUS == CCTV_STREAMING){

            sprintf(result, "cctv: stream: toggle success: started: %s\n", CCTV_MESSAGE);

            break;

        }


        if(was_streaming == 1 && CCTV_STATUS == CCTV_READY){

            sprintf(result, "cctv: stream: toggle success: ended: %s\n", CCTV_MESSAGE);

            break;

        }


        if(CCTV_STATUS == CCTV_FAILED){

            sprintf(result, "cctv: stream: toggle failed: %s\n", CCTV_MESSAGE);

            break;

        }


    }


    return;


}


void* start_cctv_stream(void* varg){

    CCTV_STREAM_ARG* cctv_stream_arg = (CCTV_STREAM_ARG*)varg;

    guint bus_watch_id;
    GMainLoop *loop = NULL;

    GstElement *bin, *pipeline, *source, *queue1, *convert1, *scale1, *enc1, *mux1, *queue2, *sink1;

    GstBus *bus;

    GstElement *filter1;
    GstCaps *caps1;

    GstStateChangeReturn ret;

    strcat(CCTV_MESSAGE, "gst entered\n");
    fmt_logln(LOGFPMOD, "gst entered");


    int argc = 1;
    char* arg1[1] = {"cctv-stream"}; 
    char** argv[1] = { arg1 };
    gst_init(&argc, &argv);

    strcat(CCTV_MESSAGE, "gst initiated\n");
    fmt_logln(LOGFPMOD, "gst initiated");

    loop = g_main_loop_new (NULL, FALSE);

    pipeline = gst_pipeline_new("soli-pipeline");

    source = gst_element_factory_make(SOLIMODCFG->mod_cctv_source, "source");

    queue1 = gst_element_factory_make("queue","queue1");

    convert1 = gst_element_factory_make("videoconvert", "convert1");

    scale1 = gst_element_factory_make("videoscale","scale1");

    filter1 = gst_element_factory_make("capsfilter", "filter1");



    enc1 = gst_element_factory_make("x264enc", "enc1");


    mux1 = gst_element_factory_make("flvmux","mux1");



    queue2 = gst_element_factory_make("queue", "queue2");



    sink1 = gst_element_factory_make("rtmpsink","sink1");



    if (!pipeline || !source || !queue1 || !convert1 || !scale1 
        || !filter1 || !enc1 || !mux1 || !queue2 || !sink1
    ){
        strcat(CCTV_MESSAGE, "one element could not be created. exiting.\n");
        fmt_logln(LOGFPMOD, "one element could not be created. exiting.");
        CCTV_STATUS = CCTV_FAILED;
        return NULL;
    }

    strcat(CCTV_MESSAGE, "elements created\n");
    fmt_logln(LOGFPMOD, "elements created");


    gst_bin_add_many(GST_BIN(pipeline), source, queue1, convert1, scale1, filter1, enc1, mux1, queue2, sink1, NULL);

    if (!gst_element_link_many(source, queue1, convert1, scale1, filter1, enc1, mux1, queue2, sink1,NULL))
    {
        strcat(CCTV_MESSAGE, "elements could not be linked: exiting.\n");
        fmt_logln(LOGFPMOD, "elements could not be linked: exiting.");
        CCTV_STATUS = CCTV_FAILED;
        return NULL;
    }

    strcat(CCTV_MESSAGE, "elements linked\n");
    fmt_logln(LOGFPMOD, "elements linked");

    caps1 = gst_caps_new_simple("video/x-raw",
        "width", G_TYPE_INT, 960,
        "height", G_TYPE_INT, 720,
        NULL);


    g_object_set(G_OBJECT(source), "device", SOLIMODCFG->mod_cctv_device, NULL);

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

    strcat(CCTV_MESSAGE, "cctv is streaming\n");
    fmt_logln(LOGFPMOD, "cctv is streaming");

    CCTV_STATUS = CCTV_STREAMING;

    g_main_loop_run (loop);

    strcat(CCTV_MESSAGE, "streaming loop\n");
    fmt_logln(LOGFPMOD, "streaming loop");


    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);

    return NULL;  
}


gboolean bus_call (GstBus * bus, GstMessage * msg, gpointer data){

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