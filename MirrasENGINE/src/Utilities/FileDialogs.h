#pragma once

#include <PortableFileDialogs/FileDialogs.hpp>

namespace mirras::dialog
{
    // Renaming class names like this to make it clearer that they are not functions
    using FileOpen = pfd::open_file;
    using FileSave = pfd::save_file;
    using FolderSelect = pfd::select_folder;
    using Notification = pfd::notify;
    using Message = pfd::message;

    using Option = pfd::opt;
    using Icon = pfd::icon;
    using Button = pfd::button;
    using Choice = pfd::choice;
    using Settings = pfd::settings;
} // namespace mirras
