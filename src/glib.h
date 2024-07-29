#include <glib.h>

static gboolean string_Equal (gchar *sText, gchar *sText2)
{
    gint nEquality = g_strcmp0 (sText, sText2);

    return (nEquality == 0);
}

static gchar* string_Remove (gchar *sText, gchar *sRemove)
{
    GString *sString = g_string_new (sText);
    g_string_replace (sString, sRemove, "", 0);
    gchar *sNewText = g_string_free_and_steal (sString);

    return sNewText;
}
