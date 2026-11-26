/* SPDX-FileCopyrightText: 2026 Alexandru Balan
   SPDX-License-Identifier: BSD-2-Clause */
/* The three functions of GStreamer 1.26 that cutefish-webkit uses, on top of GStreamer 1.24 (Ubuntu 24.04).
   Field names reach WebKit only through these callbacks, so a GstIdStr made here only has to be read by
   gst_id_str_as_str here: it holds the interned name in its first word. */
typedef int gboolean;
typedef void *gpointer;
typedef unsigned int GQuark;
typedef char gchar;
typedef struct _GValue GValue;
typedef struct _GstStructure GstStructure;
typedef struct { gpointer pointer; unsigned char padding[8]; } GstIdStr;

const gchar *g_quark_to_string(GQuark quark);
gboolean gst_structure_foreach(const GstStructure *structure, gboolean (*func)(GQuark, const GValue *, gpointer), gpointer data);
void gst_structure_filter_and_map_in_place(GstStructure *structure, gboolean (*func)(GQuark, GValue *, gpointer), gpointer data);

typedef gboolean (*ForeachIdStr)(const GstIdStr *, const GValue *, gpointer);
typedef gboolean (*FilterMapIdStr)(const GstIdStr *, GValue *, gpointer);
struct Closure { void *func; gpointer data; };

static gboolean each(GQuark field, const GValue *value, gpointer closure)
{
    const struct Closure *c = closure;
    GstIdStr name = { (gpointer)g_quark_to_string(field), { 0 } };
    return ((ForeachIdStr)c->func)(&name, value, c->data);
}

static gboolean map(GQuark field, GValue *value, gpointer closure)
{
    const struct Closure *c = closure;
    GstIdStr name = { (gpointer)g_quark_to_string(field), { 0 } };
    return ((FilterMapIdStr)c->func)(&name, value, c->data);
}

gboolean gst_structure_foreach_id_str(const GstStructure *structure, ForeachIdStr func, gpointer data)
{
    struct Closure c = { (void *)func, data };
    return gst_structure_foreach(structure, each, &c);
}

void gst_structure_filter_and_map_in_place_id_str(GstStructure *structure, FilterMapIdStr func, gpointer data)
{
    struct Closure c = { (void *)func, data };
    gst_structure_filter_and_map_in_place(structure, map, &c);
}

const gchar *gst_id_str_as_str(const GstIdStr *name)
{
    return name->pointer;
}
