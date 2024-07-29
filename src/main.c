/*
    Copyright 2013-2024 Robert Tari <robert@tari.in>

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
#include <gtk/gtk.h>
#include <locale.h>
#include <libayatana-appindicator/app-indicator.h>
#include "glib.h"

GtkWidget *m_pMenuItemStart = NULL;
GtkWidget *m_pMenuItemStop = NULL;
GtkWidget *m_pMenuItemRestart = NULL;
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
    }
    else
    {
        app_indicator_set_status (m_pIndicator, APP_INDICATOR_STATUS_ACTIVE);
    }

    gtk_widget_set_sensitive (m_pMenuItemStop, bRunning);
    gtk_widget_set_sensitive (m_pMenuItemStart, !bRunning);
    gtk_widget_set_sensitive (m_pMenuItemRestart, bRunning);
}

static void onStop (GtkMenuItem *pItem, gpointer pData)
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

static void onStart (GtkMenuItem *pItem, gpointer pData)
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

static void onRestart (GtkMenuItem *pItem, gpointer pData)
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
    GtkWidget* pMenu = gtk_menu_new ();
    m_pMenuItemStart = gtk_menu_item_new_with_label (_("Start"));
    g_signal_connect (m_pMenuItemStart, "activate", G_CALLBACK (onStart), NULL);
    gtk_menu_shell_append (GTK_MENU_SHELL (pMenu), m_pMenuItemStart);
    m_pMenuItemStop = gtk_menu_item_new_with_label (_("Stop"));
    g_signal_connect (m_pMenuItemStop, "activate", G_CALLBACK (onStop), NULL);
    gtk_menu_shell_append (GTK_MENU_SHELL (pMenu), m_pMenuItemStop);
    m_pMenuItemRestart = gtk_menu_item_new_with_label (_("Restart"));
    g_signal_connect (m_pMenuItemRestart, "activate", G_CALLBACK (onRestart), NULL);
    gtk_menu_shell_append (GTK_MENU_SHELL (pMenu), m_pMenuItemRestart);
    gtk_widget_show_all (pMenu);
    m_pIndicator = app_indicator_new ("lampswitch", "lampswitch-active", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
    app_indicator_set_attention_icon_full (m_pIndicator, "lampswitch-attention", "Web server running");
    app_indicator_set_status (m_pIndicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_menu (m_pIndicator, GTK_MENU (pMenu));
    isRunning ();
}

gint main (gint argc, gchar *argv[])
{
    setlocale (LC_ALL, "");
    bindtextdomain ("lampswitch", LOCALEDIR);
    textdomain ("lampswitch");
    bind_textdomain_codeset ("lampswitch", "UTF-8");
    GtkApplication *pApplication = gtk_application_new ("in.tari.lampswitch", G_APPLICATION_IS_SERVICE);
    g_signal_connect (pApplication, "startup", G_CALLBACK (onStartup), NULL);
    gint nStatus = g_application_run (G_APPLICATION (pApplication), argc, argv);

    return nStatus;
}
