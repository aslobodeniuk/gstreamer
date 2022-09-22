/* GStreamer unit tests for decodebin3
 * Copyright (C) 2022 Aleksandr Slobodeniuk <aslobodeniuk@fluendo.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <gst/check/gstcheck.h>
#include <gst/base/gstbaseparse.h>

/* Fake parser/decoder for parser_negotiation test */
static GType gst_fake_h264_parser_get_type (void);
static GType gst_fake_h264_decoder_get_type (void);

#undef parent_class
#define parent_class fake_h264_parser_parent_class
typedef struct _GstFakeH264Parser GstFakeH264Parser;
typedef GstElementClass GstFakeH264ParserClass;

struct _GstFakeH264Parser
{
  GstElement parent;
};

G_DEFINE_TYPE (GstFakeH264Parser, gst_fake_h264_parser, GST_TYPE_ELEMENT);

static void
gst_fake_h264_parser_class_init (GstFakeH264ParserClass * klass)
{
  static GstStaticPadTemplate sink_templ = GST_STATIC_PAD_TEMPLATE ("sink",
      GST_PAD_SINK, GST_PAD_ALWAYS,
      GST_STATIC_CAPS ("video/x-h264"));
  static GstStaticPadTemplate src_templ = GST_STATIC_PAD_TEMPLATE ("src",
      GST_PAD_SRC, GST_PAD_ALWAYS,
      GST_STATIC_CAPS ("video/x-h264, "
          "stream-format=(string) { avc, byte-stream }"));
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  gst_element_class_add_static_pad_template (element_class, &sink_templ);
  gst_element_class_add_static_pad_template (element_class, &src_templ);
  gst_element_class_set_metadata (element_class,
      "FakeH264Parser", "Codec/Parser/Converter/Video", "yep", "me");
}

static gboolean
gst_fake_h264_parser_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GstElement *self = GST_ELEMENT (parent);
  GstPad *otherpad = gst_element_get_static_pad (self, "src");
  GstCaps *accepted_caps;
  GstStructure *s;
  const gchar *stream_format;
  gboolean ret = TRUE;

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
      accepted_caps = gst_pad_get_allowed_caps (otherpad);
      accepted_caps = gst_caps_truncate (accepted_caps);

      s = gst_caps_get_structure (accepted_caps, 0);
      stream_format = gst_structure_get_string (s, "stream-format");
      if (!stream_format)
        gst_structure_set (s, "stream-format", G_TYPE_STRING, "avc", NULL);

      gst_pad_set_caps (otherpad, accepted_caps);
      gst_caps_unref (accepted_caps);
      gst_event_unref (event);
      event = NULL;
      break;
    default:
      break;
  }

  if (event)
    ret = gst_pad_push_event (otherpad, event);
  gst_object_unref (otherpad);

  return ret;
}

static GstFlowReturn
gst_fake_h264_parser_sink_chain (GstPad * pad, GstObject * parent,
    GstBuffer * buf)
{
  GstElement *self = GST_ELEMENT (parent);
  GstPad *otherpad = gst_element_get_static_pad (self, "src");
  GstFlowReturn ret = GST_FLOW_OK;

  buf = gst_buffer_make_writable (buf);

  ret = gst_pad_push (otherpad, buf);

  gst_object_unref (otherpad);

  return ret;
}

static void
gst_fake_h264_parser_init (GstFakeH264Parser * self)
{
  GstPad *pad;

  pad =
      gst_pad_new_from_template (gst_element_class_get_pad_template
      (GST_ELEMENT_GET_CLASS (self), "sink"), "sink");
  gst_pad_set_event_function (pad, gst_fake_h264_parser_sink_event);
  gst_pad_set_chain_function (pad, gst_fake_h264_parser_sink_chain);
  gst_element_add_pad (GST_ELEMENT (self), pad);

  pad =
      gst_pad_new_from_template (gst_element_class_get_pad_template
      (GST_ELEMENT_GET_CLASS (self), "src"), "src");
  gst_element_add_pad (GST_ELEMENT (self), pad);
}

#undef parent_class
#define parent_class fake_h264_decoder_parent_class
typedef struct _GstFakeH264Decoder GstFakeH264Decoder;
typedef GstElementClass GstFakeH264DecoderClass;

struct _GstFakeH264Decoder
{
  GstElement parent;
};

G_DEFINE_TYPE (GstFakeH264Decoder, gst_fake_h264_decoder, GST_TYPE_ELEMENT);

static void
gst_fake_h264_decoder_class_init (GstFakeH264DecoderClass * klass)
{
  static GstStaticPadTemplate sink_templ = GST_STATIC_PAD_TEMPLATE ("sink",
      GST_PAD_SINK, GST_PAD_ALWAYS,
      GST_STATIC_CAPS ("video/x-h264, " "stream-format=(string) byte-stream"));
  static GstStaticPadTemplate src_templ = GST_STATIC_PAD_TEMPLATE ("src",
      GST_PAD_SRC, GST_PAD_ALWAYS,
      GST_STATIC_CAPS ("video/x-raw"));
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  gst_element_class_add_static_pad_template (element_class, &sink_templ);
  gst_element_class_add_static_pad_template (element_class, &src_templ);
  gst_element_class_set_metadata (element_class,
      "FakeH264Decoder", "Codec/Decoder/Video", "yep", "me");
}

static gboolean
gst_fake_h264_decoder_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GstElement *self = GST_ELEMENT (parent);
  GstPad *otherpad = gst_element_get_static_pad (self, "src");
  GstCaps *caps;
  gboolean ret = TRUE;

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
      caps = gst_caps_new_empty_simple ("video/x-raw");
      gst_pad_set_caps (otherpad, caps);
      gst_caps_unref (caps);
      gst_event_unref (event);
      event = NULL;
      break;
    default:
      break;
  }

  if (event)
    ret = gst_pad_push_event (otherpad, event);
  gst_object_unref (otherpad);

  return ret;
}

static GstFlowReturn
gst_fake_h264_decoder_sink_chain (GstPad * pad, GstObject * parent,
    GstBuffer * buf)
{
  GstElement *self = GST_ELEMENT (parent);
  GstPad *otherpad = gst_element_get_static_pad (self, "src");
  GstFlowReturn ret = GST_FLOW_OK;

  buf = gst_buffer_make_writable (buf);

  ret = gst_pad_push (otherpad, buf);

  gst_object_unref (otherpad);

  return ret;
}

static void
gst_fake_h264_decoder_init (GstFakeH264Decoder * self)
{
  GstPad *pad;

  pad =
      gst_pad_new_from_template (gst_element_class_get_pad_template
      (GST_ELEMENT_GET_CLASS (self), "sink"), "sink");
  gst_pad_set_event_function (pad, gst_fake_h264_decoder_sink_event);
  gst_pad_set_chain_function (pad, gst_fake_h264_decoder_sink_chain);
  gst_element_add_pad (GST_ELEMENT (self), pad);

  pad =
      gst_pad_new_from_template (gst_element_class_get_pad_template
      (GST_ELEMENT_GET_CLASS (self), "src"), "src");
  gst_element_add_pad (GST_ELEMENT (self), pad);
}

static struct
{
  gint ap_received;
  gint ap_expected;
  GstCaps *expected_caps;
  gboolean check_interruption;
} fixture;

static const gint fixture_total_ap = 3;

static GstPadProbeReturn
parser_negotiation_buffer_cb (GstPad * pad,
    GstPadProbeInfo * info, gpointer user_data)
{
  GstCaps *caps;

  caps = gst_pad_get_current_caps (pad);
  fail_unless (gst_caps_is_equal (fixture.expected_caps, caps));
  gst_caps_unref (caps);

  return GST_PAD_PROBE_OK;
}

static void
parser_negotiation_pad_added_cb (GstElement * dec, GstPad * pad,
    gpointer user_data)
{
  GstBin *pipe = user_data;
  GstElement *sink;
  GstPad *sinkpad;

  sink = gst_element_factory_make ("fakesink", NULL);

  gst_bin_add (pipe, sink);
  gst_element_sync_state_with_parent (sink);
  sinkpad = gst_element_get_static_pad (sink, "sink");
  gst_pad_link (pad, sinkpad);
  gst_object_unref (sinkpad);

  gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER,
      parser_negotiation_buffer_cb, NULL, NULL);
}

static gboolean
parser_negotiation_autoplug_continue_cb (GstElement * element, GstPad * pad,
    GstCaps * caps, gpointer user_data)
{
  GST_INFO ("pad=%p %s caps = %" GST_PTR_FORMAT, pad, gst_pad_get_name (pad),
      caps);

  fixture.ap_received++;

  if (fixture.ap_received == fixture.ap_expected) {
    /* capture caps that are going to be exposed */
    fixture.expected_caps = gst_caps_ref (caps);

    if (fixture.check_interruption)
      return FALSE;
  }

  return TRUE;
}

static void
test_parser_negotiation_exec (gint stop_autoplugging_at)
{
  GstStateChangeReturn sret;
  GstMessage *msg;
  GstCaps *caps;
  GstElement *pipe, *src, *filter, *dec;

  GST_INFO ("New test: must stop at %d", stop_autoplugging_at);

  memset (&fixture, 0, sizeof (fixture));
  fixture.ap_expected = fixture_total_ap;
  if (stop_autoplugging_at) {
    fixture.ap_expected = stop_autoplugging_at;
    /* If we don't stop autoplugging, exposed caps will be
     * the raw caps after the decoder. Currently decodebin3 doesn't
     * emit autoplug-continue for this raw caps. */
    fixture.check_interruption = TRUE;
  }

  gst_element_register (NULL, "fakeh264parse", GST_RANK_PRIMARY + 101,
      gst_fake_h264_parser_get_type ());
  gst_element_register (NULL, "fakeh264dec", GST_RANK_PRIMARY + 100,
      gst_fake_h264_decoder_get_type ());

  pipe = gst_pipeline_new (NULL);

  src = gst_element_factory_make ("fakesrc", NULL);
  fail_unless (src != NULL);
  g_object_set (G_OBJECT (src), "num-buffers", 5, "sizetype", 2, "filltype", 2,
      "can-activate-pull", FALSE, NULL);

  filter = gst_element_factory_make ("capsfilter", NULL);
  fail_unless (filter != NULL);
  caps = gst_caps_from_string ("video/x-h264");
  g_object_set (G_OBJECT (filter), "caps", caps, NULL);
  gst_caps_unref (caps);

  dec = gst_element_factory_make ("decodebin3", NULL);
  fail_unless (dec != NULL);

  g_signal_connect (dec, "pad-added",
      G_CALLBACK (parser_negotiation_pad_added_cb), pipe);

  g_signal_connect (dec, "autoplug-continue",
      G_CALLBACK (parser_negotiation_autoplug_continue_cb), pipe);

  gst_bin_add_many (GST_BIN (pipe), src, filter, dec, NULL);
  gst_element_link_many (src, filter, dec, NULL);

  sret = gst_element_set_state (pipe, GST_STATE_PLAYING);
  fail_unless_equals_int (sret, GST_STATE_CHANGE_SUCCESS);

  /* wait for EOS or error */
  msg = gst_bus_timed_pop_filtered (GST_ELEMENT_BUS (pipe),
      GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
  fail_unless (msg != NULL);
  fail_unless (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_EOS);
  gst_message_unref (msg);

  gst_element_set_state (pipe, GST_STATE_NULL);
  gst_object_unref (pipe);

  fail_unless_equals_int (fixture.ap_received, fixture.ap_expected);

  gst_caps_unref (fixture.expected_caps);
  GST_INFO ("test finished ok");
}

GST_START_TEST (test_parser_negotiation)
{
  gint i;
  for (i = 0; i < fixture_total_ap; i++) {
    test_parser_negotiation_exec (i);
  }
}

GST_END_TEST;

static Suite *
decodebin3_suite (void)
{
  Suite *s = suite_create ("decodebin3");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_parser_negotiation);

  return s;
}

GST_CHECK_MAIN (decodebin3);
