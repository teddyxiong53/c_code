#include "GstUtils.h"



bool GstUtils::initializeGStreamer()
{
	GError *err = NULL;
	bool result;
	int argc = 0;
	char **argv = {

    };
	if (!(result = gst_init_check(&argc, &argv, &err)))
		g_warning("ERROR: %s\n", err->message);
	if (err)
		g_error_free(err);
	return result;
}

GstElement *GstUtils::createElement(
	GstElement *bin, const std::string &factory, const std::string &name)
{
	g_debug("createElement: factory=%s, name=%s\n", factory.c_str(), name.c_str());
	GstElement *element = gst_element_factory_make(factory.c_str(), name.c_str());
	if (!element) {
		g_warning("Unable to create: factory=%s, name=%s\n", factory.c_str(), name.c_str());
		return NULL;
	}

	if (!gst_bin_add(GST_BIN(bin), element)) {
		g_warning("Unable to add: name=%s\n",  name.c_str());
		gst_object_unref(element);
		return NULL;
	}

	return element;
}

