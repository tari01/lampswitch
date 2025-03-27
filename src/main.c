/*
    Copyright 2013-2025 Robert Tari <robert@tari.in>

    This file is part of LAMP Switch.

    LAMP Switch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LAMP Switch is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LAMP Switch. If not, see <http://www.gnu.org/licenses/gpl-3.0.txt>.
*/

#include <glib/gi18n.h>
#include <locale.h>
#include <ayatana-appindicator.h>
#include "glib.h"

AppIndicator *m_pIndicator = NULL;

static void isRunning ()
{
    const gchar *lServices[] = {"httpd", "apache2", "nginx", "php-fpm"};
    gboolean bRunning = FALSE;

    for (gint nService = 0; nService < 4; nService++)
    {
        GError *pError = NULL;
        gchar *sStatus = NULL;
        gchar *sCommand = g_strconcat ("systemctl is-active ", lServices[nService], NULL);
        g_spawn_command_line_sync (sCommand, &sStatus, NULL, NULL, &pError);
        g_free (sCommand);

        if (pError)
        {
            g_error ("Panic: Error while stopping services: %s", pError->message);
            g_error_free (pError);

            return;
        }

        gchar *sCleanStatus = string_Remove (sStatus, "\n");
        gboolean bEqual = string_Equal (sCleanStatus, "active");

        if (bEqual)
        {
            bRunning = TRUE;

            break;
        }
    }

    if (bRunning)
    {
        app_indicator_set_status (m_pIndicator, APP_INDICATOR_STATUS_ATTENTION);
        app_indicator_set_tooltip (m_pIndicator, NULL, NULL, _("Web services are running"));
    }
    else
    {
        app_indicator_set_status (m_pIndicator, APP_INDICATOR_STATUS_ACTIVE);
        app_indicator_set_tooltip (m_pIndicator, NULL, NULL, _("Web services are stopped"));
    }

    GSimpleActionGroup *pActions = app_indicator_get_actions (m_pIndicator);
    GAction *pAction = g_action_map_lookup_action (G_ACTION_MAP (pActions), "stop");
    g_object_set (G_OBJECT (pAction), "enabled", bRunning, NULL);
    pAction = g_action_map_lookup_action (G_ACTION_MAP (pActions), "start");
    g_object_set (G_OBJECT (pAction), "enabled", !bRunning, NULL);
    pAction = g_action_map_lookup_action (G_ACTION_MAP (pActions), "restart");
    g_object_set (G_OBJECT (pAction), "enabled", bRunning, NULL);
}

static void onStop (GSimpleAction *pAction, GVariant *pValue, gpointer pData)
{
    GError *pError = NULL;
    g_spawn_command_line_sync ("pkexec " DATADIR "/lampswitch/services.sh stop", NULL, NULL, NULL, &pError);

    if (pError)
    {
        g_error ("Panic: Error while stopping services: %s", pError->message);
        g_error_free (pError);

        return;
    }

    isRunning ();
}

static void onStart (GSimpleAction *pAction, GVariant *pValue, gpointer pData)
{
    GError *pError = NULL;
    g_spawn_command_line_sync ("pkexec " DATADIR "/lampswitch/services.sh start", NULL, NULL, NULL, &pError);

    if (pError)
    {
        g_error ("Panic: Error while starting services: %s", pError->message);
        g_error_free (pError);

        return;
    }

    isRunning ();
}

static void onRestart (GSimpleAction *pAction, GVariant *pValue, gpointer pData)
{
    GError *pError = NULL;
    g_spawn_command_line_sync ("pkexec " DATADIR "/lampswitch/services.sh restart", NULL, NULL, NULL, &pError);

    if (pError)
    {
        g_error ("Panic: Error while restarting services: %s", pError->message);
        g_error_free (pError);

        return;
    }

    isRunning ();
}

static void onStartup (GApplication *pApplication, gpointer pData)
{
    g_application_hold (G_APPLICATION (pApplication));
    m_pIndicator = app_indicator_new ("lampswitch", "lampswitch-active", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    app_indicator_set_attention_icon (m_pIndicator, "lampswitch-attention", _("Web services are running"));
    GMenu *pMenu = g_menu_new ();
    GSimpleActionGroup *pActions = g_simple_action_group_new ();
    GSimpleAction *pSimpleAction = g_simple_action_new ("start", NULL);
    g_action_map_add_action (G_ACTION_MAP (pActions), G_ACTION (pSimpleAction));
    g_signal_connect (pSimpleAction, "activate", G_CALLBACK (onStart), NULL);
    g_object_unref (pSimpleAction);
    GMenuItem *pItem = g_menu_item_new (_("Start"), "indicator.start");
    GIcon *pIcon = g_themed_icon_new_with_default_fallbacks ("media-playback-start");
    g_menu_item_set_icon (pItem, pIcon);
    g_object_unref (pIcon);
    g_menu_append_item (pMenu, pItem);
    g_object_unref (pItem);
    pSimpleAction = g_simple_action_new ("stop", NULL);
    g_action_map_add_action (G_ACTION_MAP (pActions), G_ACTION (pSimpleAction));
    g_signal_connect (pSimpleAction, "activate", G_CALLBACK (onStop), NULL);
    g_object_unref (pSimpleAction);
    pItem = g_menu_item_new (_("Stop"), "indicator.stop");
    pIcon = g_themed_icon_new_with_default_fallbacks ("media-playback-stop");
    g_menu_item_set_icon (pItem, pIcon);
    g_object_unref (pIcon);
    g_menu_append_item (pMenu, pItem);
    g_object_unref (pItem);
    pSimpleAction = g_simple_action_new ("restart", NULL);
    g_action_map_add_action (G_ACTION_MAP (pActions), G_ACTION (pSimpleAction));
    g_signal_connect (pSimpleAction, "activate", G_CALLBACK (onRestart), NULL);
    g_object_unref (pSimpleAction);
    pItem = g_menu_item_new (_("Restart"), "indicator.restart");
    pIcon = g_themed_icon_new_with_default_fallbacks ("view-refresh");
    g_menu_item_set_icon (pItem, pIcon);
    g_object_unref (pIcon);
    g_menu_append_item (pMenu, pItem);
    g_object_unref (pItem);
    app_indicator_set_menu (m_pIndicator, pMenu);
    g_object_unref (pMenu);
    app_indicator_set_actions (m_pIndicator, pActions);
    g_object_unref (pActions);
    isRunning ();
}

gint main (gint argc, gchar *argv[])
{
    setlocale (LC_ALL, "");
    bindtextdomain ("lampswitch", LOCALEDIR);
    textdomain ("lampswitch");
    bind_textdomain_codeset ("lampswitch", "UTF-8");
    GApplication *pApplication = g_application_new ("in.tari.lampswitch", G_APPLICATION_IS_SERVICE);
    g_signal_connect (pApplication, "startup", G_CALLBACK (onStartup), NULL);
    gint nStatus = g_application_run (pApplication, argc, argv);

    return nStatus;
}
