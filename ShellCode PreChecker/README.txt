Usage: 

shellcode_prechecker.exe shellcode.bin 2048

This will check the shellcode located in shellcode.bin and ensure it fits within 2048 bytes of available memory

The Shellcode Prechecker can accept any file type that contains raw binary shellcode. 
Since the prechecker reads the file as a binary stream, the actual file extension doesn't matter. 
What matters is that the content is pure shellcode (a sequence of machine instructions) without any headers or metadata.

.bin, .dat, .sc, .raw, .payload., .txt. 

Dont use files with headers: .exe, .dll, .elf

Donut output will work out the box.

Use Only Raw Binary Files:
The prechecker should only receive files in pure binary format, meaning no extra characters, indentations, or formatting.
Avoid Encodings: The shellcode file should not be encoded (e.g., Base64 or hex-encoded). It should be in its native binary format.

Why Indentations or Whitespace Are Problematic:
Raw binary shellcode is typically a stream of bytes that should be read exactly as is.
If the shellcode file contains indentations, spaces, newlines, or other formatting characters, the prechecker will misinterpret them as part of the data. 
This will lead to: 
	Invalid or corrupted shellcode.
	Errors when checking for null bytes or shellcode size.
	Incorrect architecture detection.

How to Handle Formatted Shellcode:

If you have shellcode with formatting (e.g., a hex dump with indentations or newlines), you need to convert it into raw binary format before using it with the prechecker.
Tools like xxd or Python can help convert formatted shellcode into raw binary.

Example of formatted shellcode (hex dump):
c

0x90, 0x90, 0xcc, 0x90, // NOPs and INT3

Convert Formatted Shellcode to Raw Binary:
Use tools to strip away the formatting and save the raw binary data in a .bin file.

Example using xxd to convert hex to binary:
bash

xxd -r -p shellcode_hex.txt shellcode.bin

Example using Python:
python

# Python script to convert escaped hex shellcode to a raw binary file

shellcode = (
    "\x48\x83\xEC\x28\x48\x83\xE4\xF0\x48\x8D\x15\x66\x00\x00\x00"
    "\x48\x8D\x0D\x52\x00\x00\x00\xE8\x9E\x00\x00\x00\x4C\x8B\xF8"
    "\x48\x8D\x0D\x5D\x00\x00\x00\xFF\xD0\x48\x8D\x15\x5F\x00\x00"
    "\x00\x48\x8D\x0D\x4D\x00\x00\x00\xE8\x7F\x00\x00\x00\x4D\x33"
    "\xC9\x4C\x8D\x05\x61\x00\x00\x00\x48\x8D\x15\x4E\x00\x00\x00"
    "\x48\x33\xC9\xFF\xD0\x48\x8D\x15\x56\x00\x00\x00\x48\x8D\x0D"
    "\x0A\x00\x00\x00\xE8\x56\x00\x00\x00\x48\x33\xC9\xFF\xD0\x4B"
    "\x45\x52\x4E\x45\x4C\x33\x32\x2E\x44\x4C\x4C\x00\x4C\x6F\x61"
    "\x64\x4C\x69\x62\x72\x61\x72\x79\x41\x00\x55\x53\x45\x52\x33"
    "\x32\x2E\x44\x4C\x4C\x00\x4D\x65\x73\x73\x61\x67\x65\x42\x6F"
    "\x78\x41\x00\x48\x65\x6C\x6C\x6F\x20\x77\x6F\x72\x6C\x64\x00"
    "\x4D\x65\x73\x73\x61\x67\x65\x00\x45\x78\x69\x74\x50\x72\x6F"
    "\x63\x65\x73\x73\x00\x48\x83\xEC\x28\x65\x4C\x8B\x04\x25\x60"
    "\x00\x00\x00\x4D\x8B\x40\x18\x4D\x8D\x60\x10\x4D\x8B\x04\x24"
    "\xFC\x49\x8B\x78\x60\x48\x8B\xF1\xAC\x84\xC0\x74\x26\x8A\x27"
    "\x80\xFC\x61\x7C\x03\x80\xEC\x20\x3A\xE0\x75\x08\x48\xFF\xC7"
    "\x48\xFF\xC7\xEB\xE5\x4D\x8B\x00\x4D\x3B\xC4\x75\xD6\x48\x33"
    "\xC0\xE9\xA7\x00\x00\x00\x49\x8B\x58\x30\x44\x8B\x4B\x3C\x4C"
    "\x03\xCB\x49\x81\xC1\x88\x00\x00\x00\x45\x8B\x29\x4D\x85\xED"
    "\x75\x08\x48\x33\xC0\xE9\x85\x00\x00\x00\x4E\x8D\x04\x2B\x45"
    "\x8B\x71\x04\x4D\x03\xF5\x41\x8B\x48\x18\x45\x8B\x50\x20\x4C"
    "\x03\xD3\xFF\xC9\x4D\x8D\x0C\x8A\x41\x8B\x39\x48\x03\xFB\x48"
    "\x8B\xF2\xA6\x75\x08\x8A\x06\x84\xC0\x74\x09\xEB\xF5\xE2\xE6"
    "\x48\x33\xC0\xEB\x4E\x45\x8B\x48\x24\x4C\x03\xCB\x66\x41\x8B"
    "\x0C\x49\x45\x8B\x48\x1C\x4C\x03\xCB\x41\x8B\x04\x89\x49\x3B"
    "\xC5\x7C\x2F\x49\x3B\xC6\x73\x2A\x48\x8D\x34\x18\x48\x8D\x7C"
    "\x24\x30\x4C\x8B\xE7\xA4\x80\x3E\x2E\x75\xFA\xA4\xC7\x07\x44"
    "\x4C\x4C\x00\x49\x8B\xCC\x41\xFF\xD7\x49\x8B\xCC\x48\x8B\xD6"
    "\xE9\x14\xFF\xFF\xFF\x48\x03\xC3\x48\x83\xC4\x28\xC3"
)

# Write to a raw binary file
with open("shellcode.bin", "wb") as f:
    f.write(shellcode.encode('latin1'))  # Use 'latin1' to preserve binary byte values

Copy the code into a Python file (e.g., convert_shellcode.py).
Run the script:
python convert_shellcode.py
The shellcode will be written to a raw binary file named shellcode.bin.

------------------------------------------------------------------
Key Points:
Architecture Compatibility:
Null Byte Check:
The tool scans the shellcode for null bytes (0x00) and warns you if any are found, as they can cause issues during injection.

Architecture Check:
Based on the shellcode's size alignment:

If the shellcode size is a multiple of 8 bytes, it's likely designed for a 64-bit architecture.
If the shellcode size is a multiple of 4 bytes, it's likely for a 32-bit architecture.

Memory Fit Check:
Compares the size of the shellcode to the provided available memory. If the shellcode size exceeds the available memory, a warning will be displayed
