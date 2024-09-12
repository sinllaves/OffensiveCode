#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <windows.h>
#include <psapi.h>
#include <cstdlib>  // For strtoull
#include <algorithm> // For std::remove
#include <string>    // For string manipulation

// Function declarations
bool SaveShellcodeToFile(const std::string& fileName, const std::vector<unsigned char>& shellcode);
bool CheckNullBytes(std::vector<unsigned char>& shellcode, const std::string& outputDir);
void CheckArchitecture(std::vector<unsigned char>& shellcode, const std::string& outputDir);
bool LoadShellcodeFromFile(const std::string& fileName, std::vector<unsigned char>& shellcode);
bool CheckShellcodeFits(SIZE_T shellcodeSize, SIZE_T availableMemory);
void AddPadding(std::vector<unsigned char>& shellcode, int alignment);

// Function to extract directory from file path (manual string manipulation)
std::string ExtractDirectory(const std::string& filePath) {
    size_t lastSlashPos = filePath.find_last_of("\\/");
    if (lastSlashPos != std::string::npos) {
        return filePath.substr(0, lastSlashPos);
    }
    else {
        return "";  // No directory, file is in the current working directory
    }
}

// Function to save shellcode to a binary file
bool SaveShellcodeToFile(const std::string& fileName, const std::vector<unsigned char>& shellcode) {
    std::ofstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file for writing: " << fileName << std::endl;
        return false;
    }
    file.write(reinterpret_cast<const char*>(shellcode.data()), shellcode.size());
    file.close();
    return true;
}

// Function to check for null bytes, report how many, and ask the user if they want to delete them
bool CheckNullBytes(std::vector<unsigned char>& shellcode, const std::string& outputDir) {
    std::vector<size_t> nullBytePositions;  // To store the positions of null bytes

    // Scan through the shellcode to find null bytes
    for (size_t i = 0; i < shellcode.size(); ++i) {
        if (shellcode[i] == 0x00) {
            nullBytePositions.push_back(i + 1);  // Storing positions (1-based index)
        }
    }

    // If null bytes are found
    if (!nullBytePositions.empty()) {
        std::cout << "Null bytes found: " << nullBytePositions.size() << "\n";
        std::cout << "Locations of null bytes (1-based index): ";
        for (const auto& pos : nullBytePositions) {
            std::cout << pos << " ";
        }
        std::cout << std::endl;

        // Ask the user whether to delete the null bytes or not
        char userChoice;
        std::cout << "Do you want to delete the null bytes? (Y/N): ";
        std::cin >> userChoice;

        // Handle user input for deletion
        if (userChoice == 'Y' || userChoice == 'y') {
            // Remove null bytes
            shellcode.erase(std::remove(shellcode.begin(), shellcode.end(), 0x00), shellcode.end());
            std::cout << "Null bytes have been removed." << std::endl;

            // Save the modified shellcode to a new file
            std::string outputFileName = outputDir + "/nonullbyteshellcode.bin";
            if (!SaveShellcodeToFile(outputFileName, shellcode)) {
                std::cerr << "Failed to save the null-byte-free shellcode." << std::endl;
                return false;
            }

            std::cout << "Modified shellcode saved to " << outputFileName << std::endl;

            // Reload the new file and check for padding
            shellcode.clear();
            if (!LoadShellcodeFromFile(outputFileName, shellcode)) {
                std::cerr << "Failed to load the modified shellcode from " << outputFileName << std::endl;
                return false;
            }

            // Ask the user if they want to add padding to the new null-byte-free shellcode
            CheckArchitecture(shellcode, outputDir);
        }
        else {
            // User decided not to delete null bytes
            std::cout << "Please remove the null bytes manually and try again." << std::endl;
            return false;
        }
    }
    else {
        std::cout << "No null bytes found in shellcode." << std::endl;
    }

    return true;
}

// Function to check if the shellcode is for 32-bit or 64-bit architecture and offer to add padding
void CheckArchitecture(std::vector<unsigned char>& shellcode, const std::string& outputDir) {
    size_t shellcodeSize = shellcode.size();
    bool needsPadding = false;

    if (shellcodeSize % 8 == 0) {
        std::cout << "Shellcode is aligned for 64-bit architecture (size: " << shellcodeSize << " bytes)." << std::endl;
    }
    else if (shellcodeSize % 4 == 0) {
        std::cout << "Shellcode is aligned for 32-bit architecture (size: " << shellcodeSize << " bytes)." << std::endl;
    }
    else {
        needsPadding = true;
        std::cout << "Shellcode size (" << shellcodeSize << " bytes) does not align with typical 32-bit or 64-bit architecture." << std::endl;

        // Ask the user if they want to add padding
        char userChoice;
        std::cout << "Do you want to add padding to align it with 32-bit or 64-bit architecture? (Y/N): ";
        std::cin >> userChoice;

        if (userChoice == 'Y' || userChoice == 'y') {
            std::cout << "Choose architecture to align with:\n";
            std::cout << "1. 32-bit\n";
            std::cout << "2. 64-bit\n";
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                AddPadding(shellcode, 4);  // Align to 32-bit (4-byte alignment)
                std::cout << "Padding added. Shellcode is now aligned to 32-bit architecture." << std::endl;
            }
            else if (choice == 2) {
                AddPadding(shellcode, 8);  // Align to 64-bit (8-byte alignment)
                std::cout << "Padding added. Shellcode is now aligned to 64-bit architecture." << std::endl;
            }
            else {
                std::cerr << "Invalid choice. No padding added." << std::endl;
                return;
            }

            // Save the modified shellcode to "nopsaddedshellcode.bin" in the same folder as the original shellcode
            std::string outputFileName = outputDir + "/nopsaddedshellcode.bin";
            if (!SaveShellcodeToFile(outputFileName, shellcode)) {
                std::cerr << "Failed to save the shellcode with added padding." << std::endl;
                return;
            }

            std::cout << "Modified shellcode with padding saved to " << outputFileName << std::endl;
        }
    }
}

// Function to load shellcode from a binary file
bool LoadShellcodeFromFile(const std::string& fileName, std::vector<unsigned char>& shellcode) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        return false;
    }

    // Read file content into the vector
    file.unsetf(std::ios::skipws);
    shellcode.insert(shellcode.begin(), std::istream_iterator<unsigned char>(file), std::istream_iterator<unsigned char>());
    return true;
}

// Function to add padding to align shellcode to a given alignment (4 or 8 bytes)
void AddPadding(std::vector<unsigned char>& shellcode, int alignment) {
    size_t shellcodeSize = shellcode.size();
    int paddingNeeded = alignment - (shellcodeSize % alignment);

    if (paddingNeeded != alignment) {  // If padding is needed
        for (int i = 0; i < paddingNeeded; ++i) {
            shellcode.push_back(0x90);  // Add NOP instructions (0x90) as padding
        }
    }
}

// Function to check if the shellcode size fits the available memory
bool CheckShellcodeFits(SIZE_T shellcodeSize, SIZE_T availableMemory) {
    if (shellcodeSize <= availableMemory) {
        std::cout << "Shellcode can fit into allocated memory." << std::endl;
        return true;
    }
    else {
        std::cerr << "Shellcode size exceeds available memory!" << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: <program.exe> <shellcode_file> <available_memory>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    SIZE_T availableMemory = strtoull(argv[2], nullptr, 10);  // Convert string to unsigned long long

    std::vector<unsigned char> shellcode;

    // Extract the directory path of the shellcode file
    std::string outputDir = ExtractDirectory(fileName);

    // Load shellcode from the file
    if (!LoadShellcodeFromFile(fileName, shellcode)) {
        return 1; // Exit if file could not be loaded
    }

    // Perform null byte check and potentially save the modified shellcode
    if (!CheckNullBytes(shellcode, outputDir)) {
        return 1; // Exit if null bytes are found and user doesn't remove them
    }

    // Perform memory fit check
    if (!CheckShellcodeFits(shellcode.size(), availableMemory)) {
        return 1; // Exit if shellcode doesn't fit in memory
    }

    std::cout << "All prechecks passed." << std::endl;
    return 0;
}
