#include "stdafx.h"

const char* blacklist[] = {
    "INIT",
    ".pdata",
    ".rdata",
    ".data",
    ".reloc",
    ".text"
};

bool isBlacklist(std::string arg) {
    //for (const auto& elem : blacklist) if (arg.find(elem) != std::string::npos) return true;
    return false;
}

int main(int argc, char* argv[])
{
	const char* file_path = argv[1];
	if (!file_path) {
		std::cout << "[-] File is not specified" << std::endl;
		return 0;
	}
	if (!std::filesystem::exists(file_path)) {
		std::cout << "[-] Failed to open specified file." << std::endl;
		return 0;
	}
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	std::cout << "[!] Working file : " << file_path << std::endl;
	HANDLE file = CreateFileA(file_path, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file != INVALID_HANDLE_VALUE) {
		DWORD fileSize = GetFileSize(file, NULL);
		BYTE* pByte = new BYTE[fileSize];
		DWORD dw;
		ReadFile(file, pByte, fileSize, &dw, NULL);

		PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pByte;
		if (dos->e_magic != IMAGE_DOS_SIGNATURE)
			return false;
		PIMAGE_FILE_HEADER FH = (PIMAGE_FILE_HEADER)(pByte + dos->e_lfanew + sizeof(DWORD));
		PIMAGE_OPTIONAL_HEADER OH = (PIMAGE_OPTIONAL_HEADER)(pByte + dos->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER));
		PIMAGE_SECTION_HEADER SH = (PIMAGE_SECTION_HEADER)(pByte + dos->e_lfanew + sizeof(IMAGE_NT_HEADERS));

		//ZeroMemory(&SH[FH->NumberOfSections], sizeof(IMAGE_SECTION_HEADER));

		for (size_t i = 0; i < FH->NumberOfSections; i++, ++SH)
		{
			// because name is BYTE
			if (isBlacklist(std::string(reinterpret_cast<char*>(SH->Name), sizeof(SH->Name)))) {
				printf("[+] '%s'\t : Section Text(Blacklist)\n", SH->Name);
			}
			else {
				printf("[+] '%s'\t : Section Text\n", SH->Name);
				RtlCopyMemory(&SH->Name, random_string(IMAGE_SIZEOF_SHORT_NAME).c_str(), IMAGE_SIZEOF_SHORT_NAME);
			}
		}

		//SetFilePointer(file, SH[FH->NumberOfSections].PointerToRawData + SH[FH->NumberOfSections].SizeOfRawData, NULL, FILE_BEGIN);
		//SetEndOfFile(file);
		//OH->AddressOfEntryPoint = OH->AddressOfEntryPoint;
		//OH->SizeOfImage = SH[FH->NumberOfSections].VirtualAddress + SH[FH->NumberOfSections].Misc.VirtualSize;

		SetFilePointer(file, 0, NULL, FILE_BEGIN);

		WriteFile(file, pByte, fileSize, &dw, NULL);
		CloseHandle(file);
		printf("\n[+] Success\n");
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::cout << "[-] Finished operation in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}
}