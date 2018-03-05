//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#import <AppKit/AppKit.h>
#include "Utility/BsEditorUtility.h"

namespace bs
{
	bool EditorUtility::openBrowseDialog(FileDialogType type, const Path& defaultPath, const String& filterList,
									Vector<Path>& paths)
	{
		Vector<String> extensions;
		if(!filterList.empty())
		{
			extensions = StringUtil::split(filterList, ";");
			for(auto& entry : extensions)
			{
				String output;
				output.reserve(entry.size());

				bool foundLeadingDot = false;
				for(auto& curChar : entry)
				{
					// Skip leading dot
					if(curChar == '.' && !foundLeadingDot)
					{
						foundLeadingDot = true;
						continue;
					}

					// Skip wildcard character
					if(curChar == '*')
						continue;

					output += curChar;
				}

				entry = output;
			}
		}

		@autoreleasepool
		{
			bool save = ((UINT32) type & (UINT32) FileDialogType::Save) != 0;
			String path = defaultPath.toString();

			if (!save)
			{
				bool file = ((UINT32) type & (UINT32) FileDialogType::OpenFile) != 0;
				bool folder = ((UINT32) type & (UINT32) FileDialogType::OpenFolder) != 0;
				bool multiselect = ((UINT32) type & (UINT32) FileDialogType::Multiselect) != 0;

				NSOpenPanel* openDlg = [NSOpenPanel openPanel];

				[openDlg setCanChooseFiles:file];
				[openDlg setCanChooseDirectories:folder];
				[openDlg setAllowsMultipleSelection:multiselect];
				[openDlg setCanCreateDirectories:YES];

				NSString* pathString = [[NSString stringWithUTF8String:path.c_str()] stringByResolvingSymlinksInPath];
				[openDlg setDirectoryURL:[NSURL fileURLWithPath:pathString]];

				NSMutableArray* fileTypes = [[NSMutableArray alloc] init];
				for (UINT32 i = 0; i < (UINT32) extensions.size(); i++)
				{
					NSString* extensionString = [NSString stringWithUTF8String:extensions[i].c_str()];
					[fileTypes addObject:extensionString];
				}

				if(fileTypes.count > 0)
					[openDlg setAllowedFileTypes:fileTypes];

				if ([openDlg runModal] == NSModalResponseOK)
				{
					NSArray* files = [openDlg URLs];

					for (NSURL* file in files)
					{
						String fileStr = String([[file path] cStringUsingEncoding:kCFStringEncodingUTF8]);
						paths.push_back(fileStr);
					}

					return true;
				}
			}
			else
			{
				NSSavePanel* saveDlg = [NSSavePanel savePanel];

				[saveDlg setCanCreateDirectories:YES];

				NSString* pathString = [[NSString stringWithUTF8String:path.c_str()] stringByResolvingSymlinksInPath];
				[saveDlg setDirectoryURL:[NSURL fileURLWithPath:pathString]];

				NSMutableArray* fileTypes = [[NSMutableArray alloc] init];
				for(UINT32 i = 0; i < (UINT32)extensions.size(); i++)
				{
					NSString* extensionString = [NSString stringWithUTF8String:extensions[i].c_str()];
					[fileTypes addObject:extensionString];
				}

				if(fileTypes.count > 0)
					[saveDlg setAllowedFileTypes:fileTypes];

				if([saveDlg runModal] == NSModalResponseOK)
				{
					NSURL* file = [saveDlg URL];

					String fileStr = String([[file path] cStringUsingEncoding:kCFStringEncodingUTF8]);
					paths.push_back(fileStr);
					return true;
				}

				return false;
			}

			return false;
		}
	}
}
