//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Utility/BsEditorUtility.h"
#include <gtk/gtk.h>
#include <String/BsUnicode.h>

namespace bs
{
	// Note: Code below is the only reason we depend on GTK. Ideally we replace the GTK file dialog with built-in file
	// dialog that can be used transparently across platforms, in which case we can also drop GTK.
	bool EditorUtility::openBrowseDialog(FileDialogType type, const Path& defaultPath, const String& filterList,
									Vector<Path>& paths)
	{
		static bool gtkInitialized = false;
		if(!gtkInitialized)
		{
			gtk_init(nullptr, nullptr);
			gtkInitialized = true;
		}

		GtkWidget* fakeParent = gtk_window_new(GTK_WINDOW_TOPLEVEL);

		const char* titleString;
		const char* cancelString;
		const char* acceptString;
		GtkFileChooserAction action;

		if(((int)type & (int)FileDialogType::OpenFile) != 0)
		{
			titleString = "Open File";
			cancelString = "Cancel";
			acceptString = "Open";
			action = GTK_FILE_CHOOSER_ACTION_OPEN;
		}
		else if(((int)type & (int)FileDialogType::OpenFolder) != 0)
		{
			titleString = "Open Folder";
			cancelString = "Cancel";
			acceptString = "Open";
			action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
		}
		else // Save dialog
		{
			titleString = "Save File";
			cancelString = "Cancel";
			acceptString = "Save";
			action = GTK_FILE_CHOOSER_ACTION_SAVE;
		}

		GtkWidget* fileDialog = gtk_file_chooser_dialog_new(titleString,
				GTK_WINDOW(fakeParent), action, cancelString, GTK_RESPONSE_CANCEL,
				acceptString, GTK_RESPONSE_ACCEPT, nullptr);

		GtkFileChooser* chooser = GTK_FILE_CHOOSER(fileDialog);

		gboolean selectMultiple = ((int)type & (int)FileDialogType::Multiselect) != 0;
		gtk_file_chooser_set_select_multiple(chooser, selectMultiple);
		gtk_file_chooser_set_do_overwrite_confirmation(chooser, true);

		String defaultPathStr = defaultPath.toString();
		if(!defaultPathStr.empty())
			gtk_file_chooser_set_current_folder(chooser, defaultPathStr.c_str());

		if(!filterList.empty())
		{
			GtkFileFilter* fileFilter = gtk_file_filter_new();

			gtk_file_filter_set_name(fileFilter, filterList.c_str());
			gtk_file_filter_add_pattern(fileFilter, filterList.c_str());
			gtk_file_chooser_add_filter(chooser, fileFilter);
		}

		// TODO - Can't set transient-for because the windows belong to two different X11 displays. I'd have to use gtk
		// display as the primary display (which is too awkward to be worth it)
		//GdkDisplay* gdkDisplay = gdk_x11_lookup_xdisplay(x11display);// gtk_widget_get_display(fileDialog);;
		//GdkWindow* parentWindow = gdk_x11_window_foreign_new_for_display(gdkDisplay, xParent);
		//gdk_window_set_transient_for(gtk_widget_get_window(fileDialog), parentWindow);

		gint response = gtk_dialog_run(GTK_DIALOG(fileDialog));
		if(response == GTK_RESPONSE_ACCEPT)
		{
			GSList* filenames = gtk_file_chooser_get_filenames(chooser);

			guint numElems = g_slist_length(filenames);
			for(guint i = 0; i < numElems; ++i)
			{
				char* filename = (char*)g_slist_nth_data(filenames, i);
				paths.push_back(String(filename));

				g_free(filename);
			}

			g_slist_free(filenames);
		}

		gtk_widget_destroy(fileDialog);
		gtk_widget_destroy(fakeParent);

		// Make sure the destroy events are processed
		while(gtk_events_pending())
			gtk_main_iteration();

		return response == GTK_RESPONSE_ACCEPT;
	}
}
