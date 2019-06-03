#include "libPXET.hpp"
#include <dirent.h>

using namespace std;

libPXET::libPXET()
{

}

void libPXET::extract(std::string file)
{
    string dir = file.substr(0,file.find_last_of("."));
    string name = dir.substr(dir.find_last_of("\\")+1);
    name = "@"+name;
    dir = dir.substr(0,dir.find_last_of("\\")+1);
    dir = dir+name;

    _mkdir(dir.c_str());

    ifstream pxet(file, ios::binary);
    uint32_t file_data,pman_header,file_info;

    pxet.seekg(0x1C);
    pxet.read(reinterpret_cast<char*>(&file_data), sizeof(uint32_t));

    pxet.seekg(0x34);
    pxet.read(reinterpret_cast<char*>(&pman_header), sizeof(uint32_t));

    pxet.seekg(0x3C);
    pxet.read(reinterpret_cast<char*>(&file_info), sizeof(uint32_t));

    ///PMAN
    uint32_t files,dict;

    pxet.seekg(pman_header+0x10);
    pxet.read(reinterpret_cast<char*>(&files), sizeof(uint32_t));

    pxet.seekg(pman_header+0x18);
    pxet.read(reinterpret_cast<char*>(&dict), sizeof(uint32_t));

    for(int i=0; i<files; i++)
    {
        uint32_t file_id, file_name_addr;
        uint32_t file_size, file_address;

        pxet.seekg(pman_header+dict+(i*0xC) + 0);
        ///???

        pxet.seekg(pman_header+dict+(i*0xC) + 0x4);
        pxet.read(reinterpret_cast<char*>(&file_id), sizeof(uint32_t));

        pxet.seekg(pman_header+dict+(i*0xC) + 0x8);
        pxet.read(reinterpret_cast<char*>(&file_name_addr), sizeof(uint32_t));

        ///fetch name
        int ibuf = file_name_addr;
        char buf = 0x1;
        string filename = "";

        while(buf != 0x0)
        {
            pxet.seekg(pman_header+ibuf);
            pxet.get(buf);

            if(buf != 0x0)
            filename.push_back(buf);
            ibuf++;
        }

        pxet.seekg(file_info + (i*0xC) + 0x0);
        ///???

        pxet.seekg(file_info + (i*0xC) + 0x4);
        pxet.read(reinterpret_cast<char*>(&file_size), sizeof(uint32_t));

        pxet.seekg(file_info + (i*0xC) + 0x8);
        pxet.read(reinterpret_cast<char*>(&file_address), sizeof(uint32_t));

        cout << "File detected: " << hex << file_id << " name_addr: " << file_name_addr << " name: " << filename << " size: " << file_size << " addr: " << file_address << dec << endl;

        ///Read the file
        pxet.seekg(file_address);
        char f[file_size];
        pxet.read(f,file_size);

        ofstream out(dir+"\\"+filename, ios::binary);
        out.write(f,file_size);
        out.close();
    }

    pxet.close();
}
