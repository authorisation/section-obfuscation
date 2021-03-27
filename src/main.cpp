#include "stdafx.h"

std::string sections[]{
    //".text",
    ".rdata",
    ".data",
    ".pdata",
    ".idata",
    ".reloc"
};

void ProcessFile(std::string path, std::vector<PeClass> peList) {
    std::ifstream file(path);
    std::string str;
    std::string tempfile;
    while (std::getline(file, str))
    {
        tempfile += str + "\n"; // save file string in temp string
    }
    // todo random char and save dos
    // look windows docs to pe magic and class (for dont broke)
}


int main(int argc,char* argv[])
{
    // have args
    if (argv[1]) {
        // file exits
        if (std::filesystem::exists(argv[1])) {
            std::cout << "[!] Working file : " << argv[1] << std::endl; // print
            HANDLE file = CreateFileA(argv[1], GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (file != INVALID_HANDLE_VALUE) {
                DWORD bytesRead = NULL;
                // allocate heap
                DWORD fileSize = GetFileSize(file, NULL);
                LPVOID fileData = HeapAlloc(GetProcessHeap(), 0, fileSize);
                // read file bytes to memory
                ReadFile(file, fileData, fileSize, &bytesRead, NULL);

                // READ DOS HEADER 
                PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)fileData;
                std::cout << "\n[*] Dos Header Info;" << std::endl;
                printf("[+] 0x%x\t : Magic Number(e_magic)\n", dosHeader->e_magic);
                printf("[+] 0x%x\t : File address of new exe header(e_lfanew)\n", dosHeader->e_lfanew);

                // READ IMAGE_NT_HEADERS
                PIMAGE_NT_HEADERS imageNTHeader = (PIMAGE_NT_HEADERS)((DWORD)fileData + dosHeader->e_lfanew);

                std::cout << "\n[*] Image NT Headers Info;" << std::endl;
                printf("[+] 0x%x\t : Signature\n", imageNTHeader->Signature);
                printf("[+] 0x%x\t : Time Date Stamp\n", imageNTHeader->FileHeader.TimeDateStamp);
                printf("[+] 0x%x\t : Size Of Optional Header\n", imageNTHeader->FileHeader.SizeOfOptionalHeader);
                printf("[+] %x\t\t : Number Of Sections\n", imageNTHeader->FileHeader.NumberOfSections);



                std::cout << "\n[*] Optimal Header Info;" << std::endl;
                // start point .text or .vmp
                printf("[+] 0x%x\t : AddressOfEntryPoint\n", imageNTHeader->OptionalHeader.AddressOfEntryPoint);

                // we need it
                std::cout << "\n[*] Section Header Info;" << std::endl;
                DWORD sectionLocation = (DWORD)imageNTHeader + sizeof(DWORD) + (DWORD)(sizeof(IMAGE_FILE_HEADER)) + (DWORD)imageNTHeader->FileHeader.SizeOfOptionalHeader;

                auto sectionHeader = (PIMAGE_SECTION_HEADER)sectionLocation;
                std::vector<PeClass> pel;
                PeClass pe;
                for (size_t i = 0; i < imageNTHeader->FileHeader.NumberOfSections; i++, ++sectionHeader)
                {
                    // need a better find solution    
                    const char* PE = reinterpret_cast<const char*>(sectionHeader->Name);
                    pe.OrginalPE = PE;
                    pe.isMatched = 0;
                    for (std::string Item : sections)
                    {
                        if (std::string(PE).find(Item) != std::string::npos) {
                            pe.isMatched = 1;
                            break;
                        }
                    }

                    printf("[+] '%s'\t : Section Text(%d)\n", pe.OrginalPE, pe.isMatched);

                    if (std::find(pel.begin(), pel.end(), pe) != pel.end()) { /*Found do nothing*/ }
                    else {
                        // Insert in vector.
                        pel.insert(pel.begin(), pe);
                    }
                    
                }
                printf("\n\n");
                CloseHandle(file);
                ProcessFile(argv[1], pel);
            }
            else {
                printf("[-] Failed to open specified file. (%d)\n", file);
            }
        } 
    }
    else {
        std::cout << "[-] File is not specified, please use this project.exe any.exe/.dll" << std::endl;
    }

    system("pause");
}