#include <windows.h>
#include <iostream>
#include <string>
#include <Lmcons.h> // for UNLEN

// Utility function to get the current Windows user's username
std::string get_current_username() {
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameA(username, &username_len)) {
        return std::string(username);
    }
    return "default_user";
}

// Create a directory if it does not exist
void ensure_directory_exists(const std::string& path) {
    DWORD ftyp = GetFileAttributesA(path.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES) {
        CreateDirectoryA(path.c_str(), NULL);
    }
    else if (!(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {
        std::cerr << "Error: " << path << " is not a directory!" << std::endl;
    }
}

// Write content to a file
void create_and_write_file(const std::string& filename, const std::string& content) {
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create file: " << filename << std::endl;
        return;
    }

    DWORD bytesWritten;
    BOOL writeOK = WriteFile(hFile, content.c_str(), content.size(), &bytesWritten, NULL);
    CloseHandle(hFile);
    if (!writeOK) {
        std::cerr << "Failed to write to file: " << filename << std::endl;
    }
}

// Rename and move a file
void rename_and_move_file(const std::string& oldPath, const std::string& newPath) {
    BOOL moveOK = MoveFileA(oldPath.c_str(), newPath.c_str());
    if (!moveOK) {
        std::cerr << "Failed to move file from " << oldPath << " to " << newPath << std::endl;
    }
}

// Delete a directory recursively
void delete_directory(const std::string& directory) {
    SHFILEOPSTRUCTA file_op = {
        NULL,
        FO_DELETE,
        directory.c_str(),
        "",
        FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
        false,
        0,
        "" };
    SHFileOperationA(&file_op);
}

int main() {
    std::string username = get_current_username();
    std::string basePath = "C:\\Users\\" + username + "\\AppData\\Local\\Temp\\";
    std::string catloggerDir = basePath + "Catlogger\\";
    std::string copycatDir = basePath + "COPYCAT\\";
    std::string snackbowlDir = basePath + "Snackbowl\\";

    ensure_directory_exists(catloggerDir);
    ensure_directory_exists(copycatDir);
    ensure_directory_exists(snackbowlDir);

    create_and_write_file(catloggerDir + "kissa.txt", "meow");
    create_and_write_file(copycatDir + "HereFirst.txt", "meow");

    rename_and_move_file(copycatDir + "HereFirst.txt", snackbowlDir + "ThereMOVED.txt");

    // Clean up
    delete_directory(catloggerDir);
    delete_directory(copycatDir);
    delete_directory(snackbowlDir);

    return 0;
}
