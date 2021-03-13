#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <zlib.h>
#include <vector>
#include <chrono>
#include <random>
#include "IL/il.h"
#include "enet/enet.h"
typedef unsigned char BYTE;

using namespace std;

ENetHost* client;
ENetPeer* peer;
bool connected = false;

int random(int min, int max){
	std::random_device r;
	typedef std::mt19937 rng_type;
	std::uniform_int_distribution<rng_type::result_type> udist(min, max);
	rng_type rng;
	rng.seed(r() ^ (
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
                ).count() +
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
                ).count() ));
	rng_type::result_type random_number = udist(rng);
	if (random_number < min) return min;
	if (random_number > max) return max;
	return random_number;
}

bool debug = true;
string directoryoftextures = "http/cache/game/";
//string directoryoftextures = "textures/";
string saveDirectory = "http/worlds/";
void log(string s){
    if (debug) cout << s << endl;
}

enum spreadType{
    FILLER, //not real spread type, just to others be ok
    SINGLE,
    DIRECT8,
    HORIZONTAL,
    ATTACH_TO_WALL_5,
    DIRECT4,
    RANDOM,
    VERTICAL,
    CAVE_PLAT,
    ATTACH_TO_WALL_4,
    DIAGONAL
};

struct itemD{
    int id;
    int textureX = 0;
    int textureY = 0;
    string texture;
    int spreadType = 0;
    int clothingType = 0;
};

vector<itemD> items;

BYTE* glue;
BYTE* water;
BYTE* fire;

void serializeItems(){
    string secret = "PBG892FXX982ABC*";
	std::ifstream file("items.dat", std::ios::binary | std::ios::ate);
	int size = file.tellg();
	char* data = new char[size];
	file.seekg(0, std::ios::beg);

	if (file.read((char*)(data), size))
	{
		printf("Decoding items data...\n");

	}
	else {
		printf("Decoding of items data has failed...\n");
#ifndef __linux__
		_getch();
#endif
		exit(0);
	}
	int itemCount;
	int memPos = 0;
	int16_t itemsdatVersion = 0;
	memcpy(&itemsdatVersion, data + memPos, 2);
	memPos += 2;
	memcpy(&itemCount, data + memPos, 4);
	memPos += 4;
	for (int i = 0;i < itemCount;i++) {
		itemD item;
		{
			memcpy(&item.id, data + memPos, 4);
			memPos += 4;
		}
		{
			//memcpy(&item.edT, data + memPos, 1);
			memPos += 1;
		}
		{
			//memcpy(&item.itemC, data + memPos, 1);
			memPos += 1;
		}
		{
			//memcpy(&item.actionType, data + memPos, 1);
			//item.actionType = data[memPos];
			memPos += 1;
		}
		{
			//item.hitSoundType = data[memPos];
			memPos += 1;
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				//item.name += data[memPos] ^ (secret[(j + item.itemID) % secret.length()]);
				memPos++;
			}
		}
		{
            item.texture = "";
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.texture += data[memPos];
				memPos++;
			}
		}
		//memcpy(&item.textureHash, data + memPos, 4);
		memPos += 4;
		//item.itemKind = data[memPos];
		memPos += 1;
		//memcpy(&item.val1, data + memPos, 4);
		memPos += 4;
		item.textureX = data[memPos];
		memPos += 1;
		item.textureY = data[memPos];
		memPos += 1;
		item.spreadType = data[memPos];
		memPos += 1;
		//item.isStripeyWallpaper = data[memPos];
		memPos += 1;
		//item.collisionType = data[memPos];
		memPos += 1;
		//item.breakHits = data[memPos];
		//item.breakHits = item.breakHits / 6;
		memPos += 1;
		//memcpy(&item.restoreTime, data + memPos, 4);
		memPos += 4;
		item.clothingType = data[memPos];
		memPos += 1;
		//memcpy(&item.rarity, data + memPos, 2);
		memPos += 2;
		//item.maxAmount = data[memPos];
		memPos += 1;
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				//item.extraFile += data[memPos];
				memPos++;
			}
		}
		//memcpy(&item.extraFileHash, data + memPos, 4);
		memPos += 4;
		//memcpy(&item.audioVolume, data + memPos, 4);
		memPos += 4;
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				//item.petName += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				//item.petPrefix += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				//item.petSuffix += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				//item.petAbility += data[memPos];
				memPos++;
			}
		}
		{
			//item.seedBase = data[memPos];
			memPos += 1;
		}
		{
			//item.seedOverlay = data[memPos];
			memPos += 1;
		}
		{
			//item.treeBase = data[memPos];
			memPos += 1;
		}
		{
			//item.treeLeaves = data[memPos];
			memPos += 1;
		}
		{
			//memcpy(&item.seedColor, data + memPos, 4);
			memPos += 4;
		}
		{
			//memcpy(&item.seedOverlayColor, data + memPos, 4);
			memPos += 4;
		}
		memPos += 4; // deleted ingredients
		{
			//memcpy(&item.growTime, data + memPos, 4);
			memPos += 4;
		}
		//memcpy(&item.val2, data + memPos, 2);
		memPos += 2;
		//memcpy(&item.isRayman, data + memPos, 2);
		memPos += 2;
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				//item.extraOptions += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				//item.texture2 += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				//item.extraOptions2 += data[memPos];
				memPos++;
			}
		}
		//memPos += 80;
		{
			int16_t extraField4Length = 0;
			memcpy(&extraField4Length, data + memPos, 2);
			memPos += 2;
			for (int j = 0; j < extraField4Length; j++) {
				//item.extraFieldUnk_4 += data[memPos];
				memPos++;
			}
			memPos += 4;
			//memcpy(&item.value, data + memPos, 2);
            short value = 0;
			memcpy(&value, data + memPos, 2);
			memPos += 2;
			//memcpy(&item.value2, data + memPos, 2);
			memPos += 2;
			//memcpy(&item.unkValueShort1, data + memPos, 2);
			//if (item.unkValueShort1 & 4) cout << itemID << ":Transmute" << endl;
			memPos += 2 + (16 - value);
			//memcpy(&item.unkValueShort2, data + memPos, 2);
			memPos += 2;
		}
		memPos += 50;
		if (itemsdatVersion >= 11) {
			{
				int16_t strLen = *(int16_t*)&data[memPos];
				memPos += 2;
				for (int j = 0; j < strLen; j++) {
					//item.punchOptions += data[memPos];
					memPos++;
				}
			}
		}
		if (i != item.id)
			cout << "Item are unordered! (" + to_string(item.id) + ", " + to_string(i) + ")" << endl;
		items.push_back(item);
	}
	string sizes = to_string(items.size());
	cout << "Loaded items.dat, size: " << sizes << endl;
}

struct WorldItem{
    int foreground = 0;
    int background = 0;
    int locked = 0;
    bool enabled = false;  
    bool red = false;
    bool green = false;
    bool blue = false;
    bool isRotated = false;
    bool water = false;
    bool fire = false;
    bool glue = false;
};

struct texture{
    int itemID = 0;
    BYTE* texture;
    int h = 0;
    int w = 0;
};
vector<texture> textures;

ILuint getTexture(string filename){
    log("Getting " + filename);
        std::ifstream file(directoryoftextures + filename, std::ios::binary | std::ios::ate);
        int size = file.tellg();
        char* data = new char[size];
        file.seekg(0, std::ios::beg);

        if (file.read((char*)(data), size))
        {
            log("Getting file to BYTE...");
        }
        else {
            cout << "Couldn't get file " << filename << ", is it in right place?" << endl;
            exit(0);
        }
        string thingie = "";
        for (int i = 0; i < 6; i++){
            thingie += data[i];
        }
        log("File type is " + thingie);
        if (thingie == "RTPACK"){
            int ver = data[6];
            int reserved = data[9];
            log("File ver: " + to_string(ver) + ", reserved: " + to_string(reserved));
            int compressedSize = 0;
            int decompressedSize = 0;
            memcpy(&compressedSize, data + 8, 4);
            memcpy(&decompressedSize, data + 12, 4);
            int compressionType = data[13];
            log("compressedSize: " + to_string(compressedSize) + ", decompressedSize: " + to_string(decompressedSize) + ", compressedSize: " + to_string(compressedSize));
            //28 - start of data
            BYTE* compdata = new BYTE[compressedSize];
            memcpy(compdata, data + 32, compressedSize);
            uLong usize = decompressedSize;
            BYTE* unpack = new BYTE[usize];
            int result = uncompress(unpack, &usize, compdata, compressedSize);
            if (result == Z_OK){
                string type = "";
                for (int i = 0; i < 6; i++) type += unpack[i];
                log("File type is " + type);
                if (type == "RTTXTR"){
                    BYTE* texdata = new BYTE[usize - 8];
                    memcpy(texdata, unpack + 8, usize - 8);
                    int height = 0;
                    int width = 0;
                    memcpy(&height, texdata, 4);
                    memcpy(&width, texdata + 4, 4);
                    log("Height: " + to_string(height) + ", width: " + to_string(width));
                    int format = 0;
                    memcpy(&format, texdata + 8, 4);
                    if (format == 0x1401){
                        int OriginalHeight = 0;
                        int OriginalWidth = 0;
                        memcpy(&OriginalHeight, texdata + 12, 4);
                        memcpy(&OriginalWidth, texdata + 16, 4);
                        log("OHeight: " + to_string(OriginalHeight) + ", OWidth: " + to_string(OriginalWidth));
                        bool UsesAlpha = texdata[20] == 1;
                        bool IsCompressed = texdata[21] == 1;
                        log("Alpha: " + to_string(UsesAlpha) + ", IsCompressed: " + to_string(IsCompressed));
                        short ReservedFlags = 0;
                        memcpy(&ReservedFlags, texdata + 22, 2);
                        int MipmapCount = 0;
                        memcpy(&MipmapCount, texdata + 24, 4);
                        log("ReservedFlags: " + to_string(ReservedFlags) + ", MipmapCount: " + to_string(MipmapCount));
                        int* rttex_reserved = new int[16];
                        int memoff = 0;
                        for (int i = 0; i < 16; i++){
                            memcpy(&rttex_reserved[i], texdata + 28 + memoff, 4);
                            memoff += 4;
                        }
                        memoff += 24;
                        //BYTE* image = new BYTE[width * height * 4];
                        ILuint result;
                        ilGenImages(1, &result);
                        ilBindImage(result);
                        ilTexImage(OriginalWidth, OriginalHeight, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
                        BYTE* image = ilGetData();
                        memset(image, 0, (OriginalWidth * OriginalHeight * 4));
                        for (int y = 0; y < OriginalHeight; y++)
                            for (int x = 0; x < OriginalWidth; x++){
                                unsigned newpos = (4 * y * OriginalWidth) + (4 * x);
                                int mempos = 28 + (y * width * 4) + x * 4;
                                if (!UsesAlpha) mempos = 28 + (y * width * 3) + x * 3;
                                image[newpos + 0] = texdata[mempos]; //R
                                image[newpos + 1] = texdata[mempos + 1]; //G
                                image[newpos + 2] = texdata[mempos + 2]; //B
                                image[newpos + 3] = 255; //A
                                if (UsesAlpha) image[newpos + 3] = texdata[mempos + 3];
                            }
                        delete[] texdata;
                        delete[] data;
                        return result;
                    }else{
                        cout << "Only OGL_RGBA_8888 format supported!" << endl;
                    }
                    delete[] texdata;
                }else{
                    cout << "Unknown (uncompressed) file type " << type << " (" << filename << ")" << endl;
                }
            }else{
                cout << "Error while decompressing " << filename << ", error code: " << to_string(result) << endl;
            }
        }
        else if (thingie == "RTTXTR"){
                    BYTE* texdata = new BYTE[size - 8];
                    memcpy(texdata, data + 8, size - 8);
                    int height = 0;
                    int width = 0;
                    memcpy(&height, texdata, 4);
                    memcpy(&width, texdata + 4, 4);
                    log("Height: " + to_string(height) + ", width: " + to_string(width));
                    int format = 0;
                    memcpy(&format, texdata + 8, 4);
                    if (format == 0x1401){
                        int OriginalHeight = 0;
                        int OriginalWidth = 0;
                        memcpy(&OriginalHeight, texdata + 12, 4);
                        memcpy(&OriginalWidth, texdata + 16, 4);
                        log("OHeight: " + to_string(OriginalHeight) + ", OWidth: " + to_string(OriginalWidth));
                        bool UsesAlpha = texdata[20] == 1;
                        bool IsCompressed = texdata[21] == 1;
                        log("Alpha: " + to_string(UsesAlpha) + ", IsCompressed: " + to_string(IsCompressed));
                        short ReservedFlags = 0;
                        memcpy(&ReservedFlags, texdata + 22, 2);
                        int MipmapCount = 0;
                        memcpy(&MipmapCount, texdata + 24, 4);
                        log("ReservedFlags: " + to_string(ReservedFlags) + ", MipmapCount: " + to_string(MipmapCount));
                        int* rttex_reserved = new int[16];
                        int memoff = 0;
                        for (int i = 0; i < 16; i++){
                            memcpy(&rttex_reserved[i], texdata + 28 + memoff, 4);
                            memoff += 4;
                        }
                        memoff += 24;
                        ILuint result;
                        ilGenImages(1, &result);
                        ilBindImage(result);
                        ilTexImage(OriginalWidth, OriginalHeight, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
                        BYTE* image = ilGetData();
                        memset(image, 0, (OriginalWidth * OriginalHeight * 4));
                        for (int y = 0; y < height; y++)
                            for (int x = 0; x < width; x++){
                                if (y >= OriginalHeight || x >= OriginalWidth) continue;
                                unsigned newpos = (4 * y * width) + (4 * x);
                                image[newpos + 0] = texdata[28 + memoff]; //R
                                image[newpos + 1] = texdata[28 + memoff + 1]; //G
                                image[newpos + 2] = texdata[28 + memoff + 2]; //B
                                image[newpos + 3] = 255; //A
                                if (UsesAlpha) image[newpos + 3] = texdata[28 + memoff + 3];
                                memoff += 3;
                                if (UsesAlpha) memoff += 1;
                            }
                        delete[] texdata;
                        delete[] data;
                        return result;
                    }else{
                        cout << "Only OGL_RGBA_8888 format supported!" << endl;
                    }
                    delete[] texdata;
        }
        else{
            cout << "Unknown file type " << thingie << " (" << filename << ")" << endl;
        }
        delete[] data;
    ILuint n;
    return n;
}

void loadTextures(){
    vector<string> files;
    for (const itemD& item : items){
        bool found = false;
        for (const string& s : files){
            if (s == item.texture) found = true;
        }
        if (!found) files.push_back(item.texture);
    }
    files.push_back("water.rttex");
    files.push_back("fire.rttex");
    files.push_back("particles.rttex");
    for (const string& s : files){
        string filename = s;
        log("Getting " + filename);
        std::ifstream file(directoryoftextures + filename, std::ios::binary | std::ios::ate);
        int size = file.tellg();
        char* data = new char[size];
        file.seekg(0, std::ios::beg);

        if (file.read((char*)(data), size))
        {
            log("Getting file to BYTE...");
        }
        else {
            cout << "Couldn't get file " << filename << ", is it in right place?" << endl;
            exit(0);
        }
        string thingie = "";
        for (int i = 0; i < 6; i++){
            thingie += data[i];
        }
        log("File type is " + thingie);
        if (thingie == "RTPACK"){
            int ver = data[6];
            int reserved = data[9];
            log("File ver: " + to_string(ver) + ", reserved: " + to_string(reserved));
            int compressedSize = 0;
            int decompressedSize = 0;
            memcpy(&compressedSize, data + 8, 4);
            memcpy(&decompressedSize, data + 12, 4);
            int compressionType = data[13];
            log("compressedSize: " + to_string(compressedSize) + ", decompressedSize: " + to_string(decompressedSize) + ", compressedSize: " + to_string(compressedSize));
            //28 - start of data
            BYTE* compdata = new BYTE[compressedSize];
            memcpy(compdata, data + 32, compressedSize);
            uLong usize = decompressedSize;
            BYTE* unpack = new BYTE[usize];
            int result = uncompress(unpack, &usize, compdata, compressedSize);
            if (result == Z_OK){
                string type = "";
                for (int i = 0; i < 6; i++) type += unpack[i];
                log("File type is " + type);
                if (type == "RTTXTR"){
                    BYTE* texdata = new BYTE[usize - 8];
                    memcpy(texdata, unpack + 8, usize - 8);
                    int height = 0;
                    int width = 0;
                    memcpy(&height, texdata, 4);
                    memcpy(&width, texdata + 4, 4);
                    log("Height: " + to_string(height) + ", width: " + to_string(width));
                    int format = 0;
                    memcpy(&format, texdata + 8, 4);
                    if (format == 0x1401){
                        int OriginalHeight = 0;
                        int OriginalWidth = 0;
                        memcpy(&OriginalHeight, texdata + 12, 4);
                        memcpy(&OriginalWidth, texdata + 16, 4);
                        log("OHeight: " + to_string(OriginalHeight) + ", OWidth: " + to_string(OriginalWidth));
                        bool UsesAlpha = texdata[20] == 1;
                        bool IsCompressed = texdata[21] == 1;
                        log("Alpha: " + to_string(UsesAlpha) + ", IsCompressed: " + to_string(IsCompressed));
                        short ReservedFlags = 0;
                        memcpy(&ReservedFlags, texdata + 22, 2);
                        int MipmapCount = 0;
                        memcpy(&MipmapCount, texdata + 24, 4);
                        log("ReservedFlags: " + to_string(ReservedFlags) + ", MipmapCount: " + to_string(MipmapCount));
                        int* rttex_reserved = new int[16];
                        int memoff = 0;
                        for (int i = 0; i < 16; i++){
                            memcpy(&rttex_reserved[i], texdata + 28 + memoff, 4);
                            memoff += 4;
                        }
                        memoff += 24;
                        BYTE* image = new BYTE[OriginalWidth * OriginalHeight * 4];
                        for (int y = 0; y < OriginalHeight; y++)
                            for (int x = 0; x < OriginalWidth; x++){
                                unsigned newpos = (4 * y * OriginalWidth) + (4 * x);
                                image[newpos + 0] = texdata[28 + memoff]; //R
                                image[newpos + 1] = texdata[28 + memoff + 1]; //G
                                image[newpos + 2] = texdata[28 + memoff + 2]; //B
                                image[newpos + 3] = 255; //A
                                if (UsesAlpha) image[newpos + 3] = texdata[28 + memoff + 3];
                                memoff += 3;
                                if (UsesAlpha) memoff += 1;
                            }
                        log("Getting tile");
                        if (filename == "glue.rttex"){
                            log("Found glue texture");
                            int tx = 5;
                            int ty = 5;
                            int startpos = (tx * 32 * 4) + ((OriginalHeight - (ty * 32) - 1) * OriginalWidth * 4);
                            glue = new BYTE[32 * 32 * 4];
                                    for (int y = 0; y < 32; y++)
                                        for (int x = 0; x < 32; x++){
                                            int newpos = startpos + (x * 4) - ((32 - y - 1) * 4 * OriginalWidth);
                                            glue[(x * 4) + (y * 4 * 32)] = image[newpos];
                                            glue[(x * 4) + (y * 4 * 32) + 1] = image[newpos + 1];
                                            glue[(x * 4) + (y * 4 * 32) + 2] = image[newpos + 2];
                                            glue[(x * 4) + (y * 4 * 32) + 3] = image[newpos + 3];
                                        }
                            continue;
                        }
                        if (filename == "water.rttex"){
                            log("Found water texture");
                            int tx = 0;
                            int ty = 0;
                            int startpos = (tx * 32 * 4) + ((OriginalHeight - (ty * 32) - 1) * OriginalWidth * 4);
                            water = new BYTE[256 * 192 * 4];
                                    for (int y = 0; y < 192; y++)
                                        for (int x = 0; x < 256; x++){
                                            int newpos = startpos + (x * 4) - ((192 - y - 1) * 4 * OriginalWidth);
                                            water[(x * 4) + (y * 4 * 256)] = image[newpos];
                                            water[(x * 4) + (y * 4 * 256) + 1] = image[newpos + 1];
                                            water[(x * 4) + (y * 4 * 256) + 2] = image[newpos + 2];
                                            water[(x * 4) + (y * 4 * 256) + 3] = image[newpos + 3];
                                        }
                            continue;
                        }
                        if (filename == "fire.rttex"){
                            log("Found fire texture");
                            int tx = 0;
                            int ty = 0;
                            int startpos = (tx * 32 * 4) + ((OriginalHeight - (ty * 32) - 1) * OriginalWidth * 4);
                            fire = new BYTE[256 * 192 * 4];
                                    for (int y = 0; y < 192; y++)
                                        for (int x = 0; x < 256; x++){
                                            int newpos = startpos + (x * 4) - ((192 - y - 1) * 4 * OriginalWidth);
                                            fire[(x * 4) + (y * 4 * 256)] = image[newpos];
                                            fire[(x * 4) + (y * 4 * 256) + 1] = image[newpos + 1];
                                            fire[(x * 4) + (y * 4 * 256) + 2] = image[newpos + 2];
                                            fire[(x * 4) + (y * 4 * 256) + 3] = image[newpos + 3];
                                        }
                            continue;
                        }
                        for (const itemD& item : items){
                            if (item.texture != filename) continue;
                            texture tex;
                            tex.itemID = item.id;
                            int tx = item.textureX;
                            int ty = item.textureY;
                            if (tx < 0 || ty < 0 || tx > (OriginalWidth - 1)/32 || ty > (OriginalHeight-1)/32){
                                cout << "tx or ty is bigger than texture's size (" << filename << ", tx: " << to_string(tx) << ", ty: " << to_string(ty) << ", id: " << to_string(tex.itemID) << ") Saving converted .rttex file for check" << endl;
                                ILuint ImgId;
                                ILenum Error;
                                ilGenImages(1, &ImgId);
                                ilBindImage(ImgId);
                                ilTexImage(OriginalWidth, OriginalHeight, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, image);
                                
                                ilEnable(IL_FILE_OVERWRITE);
                                string name = filename + ".png";
                                ilSaveImage(name.c_str());
                                ilDeleteImages(1, &ImgId);
                                exit(1);
                            }
                            int startpos = (tx * 32 * 4) + ((OriginalHeight - (ty * 32) - 1) * OriginalWidth * 4);
                            switch(item.spreadType){
                                case SINGLE:{
                                    tex.h = 32;
                                    tex.w = 32;
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * 32)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * 32) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * 32) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * 32) + 3] = image[newpos + 3];
                                        }
                                    
                                }
                                break;
                                case DIRECT8:{
                                    tex.h = (32 * 6);
                                    tex.w = (32 * 8);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case HORIZONTAL:{
                                    tex.h = 32;
                                    tex.w = (32 * 4);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case ATTACH_TO_WALL_5:{
                                    tex.h = 32;
                                    tex.w = (32 * 5);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case DIRECT4:{
                                    tex.h = (32 * 2);
                                    tex.w = (32 * 8);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case RANDOM:{
                                    tex.h = (32 * 1);
                                    tex.w = (32 * 4);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case VERTICAL:{
                                    tex.h = (32 * 1);
                                    tex.w = (32 * 4);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case CAVE_PLAT:{
                                    tex.h = (32 * 1);
                                    tex.w = (32 * 10);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case ATTACH_TO_WALL_4:{
                                    tex.h = (32 * 1);
                                    tex.w = (32 * 4);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case DIAGONAL:{
                                    tex.h = (32 * 1);
                                    tex.w = (32 * 4);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                default:{
                                    cout << "Unknown spreadType: " << to_string(item.spreadType) << "! Saving tile page and quitting. (itemID: " << to_string(item.id) << ")" << endl;
                                    ILuint ImgId;
                                    ILenum Error;
                                    ilGenImages(1, &ImgId);
                                    ilBindImage(ImgId);
                                    ilTexImage(OriginalWidth, OriginalHeight, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, image);
                                    
                                    ilEnable(IL_FILE_OVERWRITE);
                                    string name = filename + ".png";
                                    ilSaveImage(name.c_str());
                                    ilDeleteImages(1, &ImgId);
                                    exit(1);
                                }
                                break;
                            }
                            textures.push_back(tex);
                        }
                    }else{
                        cout << "Only OGL_RGBA_8888 format supported!" << endl;
                    }
                    delete[] texdata;
                }else{
                    cout << "Unknown (uncompressed) file type " << type << " (" << filename << ")" << endl;
                }
            }else{
                cout << "Error while decompressing " << filename << ", error code: " << to_string(result) << endl;
            }
        }
        else if (thingie == "RTTXTR"){
                    BYTE* texdata = new BYTE[size - 8];
                    memcpy(texdata, data + 8, size - 8);
                    int height = 0;
                    int width = 0;
                    memcpy(&height, texdata, 4);
                    memcpy(&width, texdata + 4, 4);
                    log("Height: " + to_string(height) + ", width: " + to_string(width));
                    int format = 0;
                    memcpy(&format, texdata + 8, 4);
                    if (format == 0x1401){
                        int OriginalHeight = 0;
                        int OriginalWidth = 0;
                        memcpy(&OriginalHeight, texdata + 12, 4);
                        memcpy(&OriginalWidth, texdata + 16, 4);
                        log("OHeight: " + to_string(OriginalHeight) + ", OWidth: " + to_string(OriginalWidth));
                        bool UsesAlpha = texdata[20] == 1;
                        bool IsCompressed = texdata[21] == 1;
                        log("Alpha: " + to_string(UsesAlpha) + ", IsCompressed: " + to_string(IsCompressed));
                        short ReservedFlags = 0;
                        memcpy(&ReservedFlags, texdata + 22, 2);
                        int MipmapCount = 0;
                        memcpy(&MipmapCount, texdata + 24, 4);
                        log("ReservedFlags: " + to_string(ReservedFlags) + ", MipmapCount: " + to_string(MipmapCount));
                        int* rttex_reserved = new int[16];
                        int memoff = 0;
                        for (int i = 0; i < 16; i++){
                            memcpy(&rttex_reserved[i], texdata + 28 + memoff, 4);
                            memoff += 4;
                        }
                        memoff += 24;
                        BYTE* image = new BYTE[OriginalWidth * OriginalHeight * 4];
                        for (int y = 0; y < OriginalHeight; y++)
                            for (int x = 0; x < OriginalWidth; x++){
                                unsigned newpos = (4 * y * OriginalWidth) + (4 * x);
                                image[newpos + 0] = texdata[28 + memoff]; //R
                                image[newpos + 1] = texdata[28 + memoff + 1]; //G
                                image[newpos + 2] = texdata[28 + memoff + 2]; //B
                                image[newpos + 3] = 255; //A
                                if (UsesAlpha) image[newpos + 3] = texdata[28 + memoff + 3];
                                memoff += 3;
                                if (UsesAlpha) memoff += 1;
                            }
                        log("Getting tile");
                        if (filename == "glue.rttex"){
                            log("Found glue texture");
                            int tx = 5;
                            int ty = 5;
                            int startpos = (tx * 32 * 4) + ((OriginalHeight - (ty * 32) - 1) * OriginalWidth * 4);
                            glue = new BYTE[32 * 32 * 4];
                                    for (int y = 0; y < 32; y++)
                                        for (int x = 0; x < 32; x++){
                                            int newpos = startpos + (x * 4) - ((32 - y - 1) * 4 * OriginalWidth);
                                            glue[(x * 4) + (y * 4 * 32)] = image[newpos];
                                            glue[(x * 4) + (y * 4 * 32) + 1] = image[newpos + 1];
                                            glue[(x * 4) + (y * 4 * 32) + 2] = image[newpos + 2];
                                            glue[(x * 4) + (y * 4 * 32) + 3] = image[newpos + 3];
                                        }
                            continue;
                        }
                        if (filename == "water.rttex"){
                            log("Found water texture");
                            int tx = 0;
                            int ty = 0;
                            int startpos = (tx * 32 * 4) + ((OriginalHeight - (ty * 32) - 1) * OriginalWidth * 4);
                            water = new BYTE[256 * 192 * 4];
                                    for (int y = 0; y < 192; y++)
                                        for (int x = 0; x < 256; x++){
                                            int newpos = startpos + (x * 4) - ((192 - y - 1) * 4 * OriginalWidth);
                                            glue[(x * 4) + (y * 4 * 256)] = image[newpos];
                                            glue[(x * 4) + (y * 4 * 256) + 1] = image[newpos + 1];
                                            glue[(x * 4) + (y * 4 * 256) + 2] = image[newpos + 2];
                                            glue[(x * 4) + (y * 4 * 256) + 3] = image[newpos + 3];
                                        }
                            continue;
                        }
                        if (filename == "fire.rttex"){
                            log("Found fire texture");
                            int tx = 0;
                            int ty = 0;
                            int startpos = (tx * 32 * 4) + ((OriginalHeight - (ty * 32) - 1) * OriginalWidth * 4);
                            water = new BYTE[256 * 192 * 4];
                                    for (int y = 0; y < 192; y++)
                                        for (int x = 0; x < 256; x++){
                                            int newpos = startpos + (x * 4) - ((192 - y - 1) * 4 * OriginalWidth);
                                            glue[(x * 4) + (y * 4 * 256)] = image[newpos];
                                            glue[(x * 4) + (y * 4 * 256) + 1] = image[newpos + 1];
                                            glue[(x * 4) + (y * 4 * 256) + 2] = image[newpos + 2];
                                            glue[(x * 4) + (y * 4 * 256) + 3] = image[newpos + 3];
                                        }
                            continue;
                        }
                        for (const itemD& item : items){
                            if (item.texture != filename) continue;
                            texture tex;
                            tex.itemID = item.id;
                            int tx = item.textureX;
                            int ty = item.textureY;
                            if (tx < 0 || ty < 0 || tx > (OriginalWidth - 1)/32 || ty > (OriginalHeight-1)/32){
                                cout << "tx or ty is bigger than texture's size (" << filename << ", tx: " << to_string(tx) << ", ty: " << to_string(ty) << ", id: " << to_string(tex.itemID) << ") Saving converted .rttex file for check" << endl;
                                ILuint ImgId;
                                ILenum Error;
                                ilGenImages(1, &ImgId);
                                ilBindImage(ImgId);
                                ilTexImage(OriginalWidth, OriginalHeight, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, image);
                                
                                ilEnable(IL_FILE_OVERWRITE);
                                string name = filename + ".png";
                                ilSaveImage(name.c_str());
                                ilDeleteImages(1, &ImgId);
                                exit(1);
                            }
                            int startpos = (tx * 32 * 4) + ((OriginalHeight - (ty * 32) - 1) * OriginalWidth * 4);
                            switch(item.spreadType){
                                case SINGLE:{
                                    tex.h = 32;
                                    tex.w = 32;
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * 32)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * 32) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * 32) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * 32) + 3] = image[newpos + 3];
                                        }
                                    
                                }
                                break;
                                case DIRECT8:{
                                    tex.h = (32 * 6);
                                    tex.w = (32 * 8);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case HORIZONTAL:{
                                    tex.h = 32;
                                    tex.w = (32 * 4);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case ATTACH_TO_WALL_5:{
                                    tex.h = 32;
                                    tex.w = (32 * 5);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case DIRECT4:{
                                    tex.h = (32 * 2);
                                    tex.w = (32 * 8);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case RANDOM:{
                                    tex.h = (32 * 1);
                                    tex.w = (32 * 4);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case VERTICAL:{
                                    tex.h = (32 * 1);
                                    tex.w = (32 * 4);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case CAVE_PLAT:{
                                    tex.h = (32 * 1);
                                    tex.w = (32 * 10);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case ATTACH_TO_WALL_4:{
                                    tex.h = (32 * 1);
                                    tex.w = (32 * 4);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                case DIAGONAL:{
                                    tex.h = (32 * 1);
                                    tex.w = (32 * 4);
                                    tex.texture = new BYTE[tex.h * tex.w * 4];
                                    for (int y = 0; y < tex.h; y++)
                                        for (int x = 0; x < tex.w; x++){
                                            int newpos = startpos + (x * 4) - ((tex.h - y - 1) * 4 * OriginalWidth);
                                            tex.texture[(x * 4) + (y * 4 * tex.w)] = image[newpos];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 1] = image[newpos + 1];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 2] = image[newpos + 2];
                                            tex.texture[(x * 4) + (y * 4 * tex.w) + 3] = image[newpos + 3];
                                        }
                                }
                                break;
                                default:{
                                    cout << "Unknown spreadType: " << to_string(item.spreadType) << "! Saving tile page and quitting. (itemID: " << to_string(item.id) << ")" << endl;
                                    ILuint ImgId;
                                    ILenum Error;
                                    ilGenImages(1, &ImgId);
                                    ilBindImage(ImgId);
                                    ilTexImage(OriginalWidth, OriginalHeight, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, image);
                                    
                                    ilEnable(IL_FILE_OVERWRITE);
                                    string name = filename + ".png";
                                    ilSaveImage(name.c_str());
                                    ilDeleteImages(1, &ImgId);
                                    exit(1);
                                }
                                break;
                            }
                            textures.push_back(tex);
                        }
                    }else{
                        cout << "Only OGL_RGBA_8888 format supported!" << endl;
                    }
                    delete[] texdata;
        }
        else{
            cout << "Unknown file type " << thingie << " (" << filename << ")" << endl;
        }
        delete[] data;
    }
}

BYTE* getTexture(int id, WorldItem* tiles, int bx, int by, int w, int h){
    if (id == 0) return NULL;
    BYTE* tex = textures[id].texture;
    if (!tex) return NULL;
    int tx = 0;
    int ty = 0;
    bool rev = false;
    bool bg = (tiles[bx + (by * w)].background == id);
    switch(items[id].spreadType){
        case SINGLE:
            tx = 0;
            ty = 0;
            if (tiles[bx + (by * w)].isRotated) rev = true;
        break;
        case DIRECT8:{
            bool down = false;
            if (by == 0) down = true;
            else if (by - 1 >= 0) down = (tiles[bx + ((by - 1) * w)].foreground == id || tiles[bx + ((by - 1) * w)].background == id || (tiles[bx + ((by - 1) * w)].glue && !bg && tiles[bx + ((by - 1) * w)].foreground != 0) || (tiles[bx + ((by - 1) * w)].glue && bg && tiles[bx + ((by - 1) * w)].background != 0));
            bool up = false;
            if (by == h - 1) up = true;
            else if (by + 1 < h) up = (tiles[bx + ((by + 1) * w)].foreground == id || tiles[bx + ((by + 1) * w)].background == id || (tiles[bx + ((by + 1) * w)].glue && !bg && tiles[bx + ((by + 1) * w)].foreground != 0) || (tiles[bx + ((by + 1) * w)].glue && bg && tiles[bx + ((by + 1) * w)].background != 0));
            bool left = false;
            if (bx == 0) left = true;
            else if (bx - 1 >= 0) left = (tiles[bx - 1 + (by * w)].foreground == id || tiles[bx - 1 + (by * w)].background == id || (tiles[bx - 1 + (by * w)].glue && !bg && tiles[bx - 1 + (by * w)].foreground != 0) || (tiles[bx - 1 + (by * w)].glue && bg && tiles[bx - 1 + (by * w)].background != 0));
            bool right = false;
            if (bx == w - 1) right = true;
            else if (bx + 1 < w) right = (tiles[bx + 1 + (by * w)].foreground == id || tiles[bx + 1 + (by * w)].background == id || (tiles[bx + 1 + (by * w)].glue && !bg && tiles[bx + 1 + (by * w)].foreground != 0) || (tiles[bx + 1 + (by * w)].glue && bg && tiles[bx + 1 + (by * w)].background != 0));
            bool lefto = false;
            bool righto = false;
            if (right && bx < w - 1){
                if (by + 1 < h) righto = (tiles[bx + 1 + ((by + 1) * w)].foreground != id && tiles[bx + 1 + ((by + 1) * w)].background != id && !((tiles[bx + 1 + ((by + 1) * w)].glue && !bg && tiles[bx + 1 + ((by + 1) * w)].foreground != 0) || (tiles[bx + 1 + ((by + 1) * w)].glue && bg && tiles[bx + 1 + ((by + 1) * w)].background != 0)));
            }
            if (left && bx > 0){
                if (by + 1 < h) lefto = (tiles[bx - 1 + ((by + 1) * w)].foreground != id && tiles[bx - 1 + ((by + 1) * w)].background != id && !((tiles[bx - 1 + ((by + 1) * w)].glue && !bg && tiles[bx - 1 + ((by + 1) * w)].foreground != 0) || (tiles[bx - 1 + ((by + 1) * w)].glue && bg && tiles[bx - 1 + ((by + 1) * w)].background != 0)));
            }
            bool leftod = false;
            bool rightod = false;
            if (right && bx < w - 1){
                if (by - 1 >= 0) rightod = (tiles[bx + 1 + ((by - 1) * w)].foreground != id && tiles[bx + 1 + ((by - 1) * w)].background != id && !((tiles[bx + 1 + ((by - 1) * w)].glue && !bg && tiles[bx + 1 + ((by - 1) * w)].foreground != 0) || (tiles[bx + 1 + ((by - 1) * w)].glue && bg && tiles[bx + 1 + ((by - 1) * w)].background != 0)));
            }
            if (left && bx > 0){
                if (by - 1 >= 0) leftod = (tiles[bx - 1 + ((by - 1) * w)].foreground != id && tiles[bx - 1 + ((by - 1) * w)].background != id && !((tiles[bx - 1 + ((by - 1) * w)].glue && !bg && tiles[bx - 1 + ((by - 1) * w)].foreground != 0) || (tiles[bx - 1 + ((by - 1) * w)].glue && bg && tiles[bx - 1 + ((by - 1) * w)].background != 0)));
            }
            //any problem? use int pos as in DIRECT4
            tx = 4;
            ty = 1;
            if (right || left) ty += 2;
            if (right && !left) tx += 1;
            if (left && !right) tx += 2;
            if (up) tx -= 1;
            if (up && (left || right)) tx -= 1;
            if (down && !(right || left)) tx -= 2;
            if (down && (right || left)) {
                if (!up && ((right && rightod && !left) || (left && leftod && !right))) ty += 2;
                else ty -= 3;
            }
            if (down && right && left && !up) tx -= 3;
            if (up && (lefto || righto)) ty += 2;
            if (up && down && lefto) ty += 2;
            if (up && down && righto) ty -= 1;
            if (up && down && righto && !lefto) {
                if (!rightod && !left){
                    ty += 2;
                    tx += 4;
                }
                else {
                    ty += 3;
                    tx -= 2;
                }
            }
            if (up && (left && !lefto && right && !righto) && !down){
                ty -= 3;
            }
            if (up && left && !lefto && !right && !down){
                ty -= 2;
                tx -= 4;
            }
            if (up && right && !left && !righto && !down){
                ty -= 3;
                tx += 4;
            }
            if (up && down && left && right && !lefto && !righto){
                tx -= 2;
            }
            if (up && down && left && !lefto && righto) {
                ty -= 2;
                if (!rightod) tx += 6;
                else tx += 4;
            }
            if (up && down && lefto && !righto) {
                ty -= 2;
                tx += 1;
            }
            if (up && !down && left && right){
                if (righto && !lefto) tx -= 1;
                if (!righto && lefto) tx -= 2;
            }
            if (up && down && left && right){
                if (righto && !lefto && !rightod){
                    ty -= 1;
                }
                if (lefto && !righto && !leftod){
                    tx += 2;
                    ty -= 1;
                }
                if (leftod && !rightod && !lefto){
                    ty += 1;
                    tx += 7;
                }
                if (rightod && !leftod && !righto){
                    ty += 2;
                }
                if (rightod && leftod && !lefto && !righto){
                    ty += 2;
                    tx += 2;
                }
                if (rightod && leftod && lefto && righto) tx += 1;
                if (!(rightod && leftod && lefto && righto) && (lefto && rightod)) ty -= 1;
                if (!(rightod && leftod && lefto && righto) && (righto && leftod)) tx -= 7;
                if (!(rightod && leftod && lefto && righto) && (rightod && leftod && lefto)) {
                    ty += 2;
                    tx -= 3;
                }
                if (!(rightod && leftod && lefto && righto) && (rightod && leftod && righto)) {
                    tx += 10;
                }
            }
            if (down && right && left && !up){
                if (leftod || rightod) {
                    tx += 3;
                    ty += 4;
                }
                if (leftod) tx += 1;
                if (rightod) tx += 2;
            }
            if (up && down && left && !right){
                if (lefto || leftod) {
                    if (!lefto) tx -= 3;
                    else tx -= 4;
                    if (!lefto) ty += 4;
                    else ty += 2;
                }
                if (lefto) tx += 1;
                if (leftod) tx += 2;
            }
            if (tx < 0 || ty < 0 || tx > 7 || ty > 5 || (tx == 7 && ty == 5)){
                cout << to_string(tx) << ":" << to_string(ty) << endl;
                tx = 4;
                ty = 1;
            }
        }
        break;
        case HORIZONTAL:{
            bool left = false;
            if (bx == 0) left = true;
            else if (bx - 1 >= 0) left = (tiles[bx - 1 + (by * w)].foreground == id || tiles[bx - 1 + (by * w)].background == id || (tiles[bx - 1 + (by * w)].glue && !bg && tiles[bx - 1 + (by * w)].foreground != 0) || (tiles[bx - 1 + (by * w)].glue && bg && tiles[bx - 1 + (by * w)].background != 0));
            bool right = false;
            if (bx == w - 1) right = true;
            else if (bx + 1 < w) right = (tiles[bx + 1 + (by * w)].foreground == id || tiles[bx + 1 + (by * w)].background == id || (tiles[bx + 1 + (by * w)].glue && !bg && tiles[bx + 1 + (by * w)].foreground != 0) || (tiles[bx + 1 + (by * w)].glue && bg && tiles[bx + 1 + (by * w)].background != 0));
            if (left) tx = 2;
            if (right) tx = 0;
            if (left && right) tx = 1;
            if (!left && !right) tx = 3;
        }
        break;
        case ATTACH_TO_WALL_5:{
            bool down = false;
            if (by - 1 >= 0) down = (tiles[bx + ((by - 1) * w)].foreground != 0 && items[tiles[bx + ((by - 1) * w)].foreground].spreadType != ATTACH_TO_WALL_5);
            bool up = false;
            if (by + 1 < h) up = (tiles[bx + ((by + 1) * w)].foreground != 0 && items[tiles[bx + ((by + 1) * w)].foreground].spreadType != ATTACH_TO_WALL_5);
            bool left = false;
            if (bx - 1 >= 0) left = (tiles[bx - 1 + (by * w)].foreground != 0 && items[tiles[bx - 1 + (by * w)].foreground].spreadType != ATTACH_TO_WALL_5);
            bool right = false;
            if (bx + 1 < w) right = (tiles[bx + 1 + (by * w)].foreground != 0 && items[tiles[bx + 1 + (by * w)].foreground].spreadType != ATTACH_TO_WALL_5);
            tx = 4;
            if (left) tx = 0;
            if (up) tx = 1;
            if (right) tx = 2;
            if (down) tx = 3;
        }
        break;
        case DIRECT4:{
            bool down = false;
            if (by == 0) down = true;
            else if (by - 1 >= 0) down = (tiles[bx + ((by - 1) * w)].foreground == id || tiles[bx + ((by - 1) * w)].background == id || (tiles[bx + ((by - 1) * w)].glue && !bg && tiles[bx + ((by - 1) * w)].foreground != 0) || (tiles[bx + ((by - 1) * w)].glue && bg && tiles[bx + ((by - 1) * w)].background != 0));
            bool up = false;
            if (by == h - 1) up = true;
            else if (by + 1 < h) up = (tiles[bx + ((by + 1) * w)].foreground == id || tiles[bx + ((by + 1) * w)].background == id || (tiles[bx + ((by + 1) * w)].glue && !bg && tiles[bx + ((by + 1) * w)].foreground != 0) || (tiles[bx + ((by + 1) * w)].glue && bg && tiles[bx + ((by + 1) * w)].background != 0));
            bool left = false;
            if (bx == 0) left = true;
            else if (bx - 1 >= 0) left = (tiles[bx - 1 + (by * w)].foreground == id || tiles[bx - 1 + (by * w)].background == id || (tiles[bx - 1 + (by * w)].glue && !bg && tiles[bx - 1 + (by * w)].foreground != 0) || (tiles[bx - 1 + (by * w)].glue && bg && tiles[bx - 1 + (by * w)].background != 0));
            bool right = false;
            if (bx == w - 1) right = true;
            else if (bx + 1 < w) right = (tiles[bx + 1 + (by * w)].foreground == id || tiles[bx + 1 + (by * w)].background == id || (tiles[bx + 1 + (by * w)].glue && !bg && tiles[bx + 1 + (by * w)].foreground != 0) || (tiles[bx + 1 + (by * w)].glue && bg && tiles[bx + 1 + (by * w)].background != 0));
            if (up && down && right && left){
                tx = 0;
                ty = 0;
                break;
            }
            int pos = 12;
            if (left || right) pos += 1;
            if (right && !left) pos += 1;
            if (!right && left) pos += 2;
            if (down) pos -= 2;
            if (up) pos -= 1;
            if ((left || right) && down && !up) pos -= 7;
            if ((left || right) && up && !down) pos -= 6;
            if ((left || right) && up && down) pos -= 8;
            if (left && right && (down || up)) pos -= 3;

            tx = pos % 8;
            ty = pos / 8;
            if (tx < 0 || ty < 0 || tx > 7 || ty > 1){
                cout << to_string(tx) << ":" << to_string(ty) << ":" << to_string(pos) << endl;
                tx = 4;
                ty = 1;
            }
        }
        break;
        case RANDOM:{
            tx = random(0, 3);
        }
        break;
        case VERTICAL:{
            bool down = false;
            if (by - 1 >= 0) down = (tiles[bx + ((by - 1) * w)].foreground == id || tiles[bx + ((by - 1) * w)].background == id || (tiles[bx + ((by - 1) * w)].glue && !bg && tiles[bx + ((by - 1) * w)].foreground != 0) || (tiles[bx + ((by - 1) * w)].glue && bg && tiles[bx + ((by - 1) * w)].background != 0));
            bool up = false;
            if (by + 1 < h) up = (tiles[bx + ((by + 1) * w)].foreground == id || tiles[bx + ((by + 1) * w)].background == id || (tiles[bx + ((by + 1) * w)].glue && !bg && tiles[bx + ((by + 1) * w)].foreground != 0) || (tiles[bx + ((by + 1) * w)].glue && bg && tiles[bx + ((by + 1) * w)].background != 0));
            tx = 3;
            if (down) tx = 2;
            if (up) tx = 0;
            if (up && down) tx = 1;
        }
        break;
        case CAVE_PLAT:{
            bool down = false;
            if (by == 0) down = true;
            else if (by - 1 >= 0) down = (tiles[bx + ((by - 1) * w)].foreground == 3566 || (tiles[bx + ((by - 1) * w)].glue && tiles[bx + ((by - 1) * w)].foreground != 0));
            bool left = false;
            if (bx == 0) left = true;
            else if (bx - 1 >= 0) left = (tiles[bx - 1 + (by * w)].foreground == id || tiles[bx - 1 + (by * w)].background == id || (tiles[bx - 1 + (by * w)].glue && !bg && tiles[bx - 1 + (by * w)].foreground != 0) || (tiles[bx - 1 + (by * w)].glue && bg && tiles[bx - 1 + (by * w)].background != 0));
            bool right = false;
            if (bx == w - 1) right = true;
            else if (bx + 1 < w) right = (tiles[bx + 1 + (by * w)].foreground == id || tiles[bx + 1 + (by * w)].background == id || (tiles[bx + 1 + (by * w)].glue && !bg && tiles[bx + 1 + (by * w)].foreground != 0) || (tiles[bx + 1 + (by * w)].glue && bg && tiles[bx + 1 + (by * w)].background != 0));
            
            bool dleft = false;
            if (bx == 0) dleft = true;
            else if (bx - 1 >= 0) dleft = (tiles[bx - 1 + (by * w)].foreground == 3564);
            bool dright = false;
            if (bx == w - 1) dright = true;
            else if (bx + 1 < w) dright = (tiles[bx + 1 + (by * w)].foreground == 3564);

            tx = 3;
            if (down) {
                tx = 9;
                break;
            }
            if (dleft && dright){
                tx = 6;
                break;
            }
            if (dleft){
                tx = 7;
            }
            if (dleft && right){
                tx = 4;
                break;
            }
            if (dright){
                tx = 8;
            }
            if (dright && left){
                tx = 5;
                break;
            }
            if (left){
                tx = 2;
            }
            if (right){
                tx = 0;
            }
            if (left && right){
                tx = 1;
            }
        }
        break;
        case ATTACH_TO_WALL_4:{
            bool down = false;
            if (by - 1 >= 0) down = (tiles[bx + ((by - 1) * w)].foreground != 0 && items[tiles[bx + ((by - 1) * w)].foreground].spreadType != CAVE_PLAT && items[tiles[bx + ((by - 1) * w)].foreground].spreadType != HORIZONTAL);
            bool up = false;
            if (by + 1 < h) up = (tiles[bx + ((by + 1) * w)].foreground != 0 && items[tiles[bx + ((by + 1) * w)].foreground].spreadType != CAVE_PLAT && items[tiles[bx + ((by + 1) * w)].foreground].spreadType != HORIZONTAL);
            bool left = false;
            if (bx - 1 >= 0) left = (tiles[bx - 1 + (by * w)].foreground != 0 && items[tiles[bx - 1 + (by * w)].foreground].spreadType != CAVE_PLAT && items[tiles[bx - 1 + (by * w)].foreground].spreadType != HORIZONTAL);
            bool right = false;
            if (bx + 1 < w) right = (tiles[bx + 1 + (by * w)].foreground != 0 && items[tiles[bx + 1 + (by * w)].foreground].spreadType != CAVE_PLAT && items[tiles[bx + 1 + (by * w)].foreground].spreadType != HORIZONTAL);
            tx = 3;
            if (right) tx = 2;
            if (left) tx = 0;
            if (up) tx = 1;
            if (down) tx = 3;
        }
        break;
        case DIAGONAL:{
            if (tiles[bx + (by * w)].isRotated) rev = true;
            bool down = false;
            if (by - 1 >= 0){
                if (tiles[bx + (by * w)].isRotated && bx + 1 < w) down = ((tiles[bx + 1 + ((by - 1) * w)].foreground == id && tiles[bx + 1 + ((by - 1) * w)].isRotated == tiles[bx + (by * w)].isRotated) || (tiles[bx + 1 + ((by - 1) * w)].glue && tiles[bx + 1 + ((by - 1) * w)].foreground != 0));
                else if (bx - 1 >= 0) down = ((tiles[bx - 1 + ((by - 1) * w)].foreground == id && tiles[bx - 1 + ((by - 1) * w)].isRotated == tiles[bx + (by * w)].isRotated) || (tiles[bx - 1 + ((by - 1) * w)].glue && tiles[bx - 1 + ((by - 1) * w)].foreground != 0));
            }
            bool up = false;
            if (by + 1 < h) {
                if (tiles[bx + (by * w)].isRotated && bx + 1 < w) up = ((tiles[bx - 1 + ((by + 1) * w)].foreground == id && tiles[bx - 1 + ((by + 1) * w)].isRotated == tiles[bx + (by * w)].isRotated) || (tiles[bx - 1 + ((by + 1) * w)].glue && tiles[bx - 1 + ((by + 1) * w)].foreground != 0));
                else if (bx - 1 >= 0) up = ((tiles[bx + 1 + ((by + 1) * w)].foreground == id && tiles[bx + 1 + ((by + 1) * w)].isRotated == tiles[bx + (by * w)].isRotated) || (tiles[bx + 1 + ((by + 1) * w)].glue && tiles[bx + 1 + ((by + 1) * w)].foreground != 0));
            }
            tx = 3;
            if (down && !up) tx = 2;
            if (up && !down) tx = 0;
            if (up && down) tx = 1;
        }
        break;
        default:
            tx = 0;
            ty = 0;
        break;
    }

    int startpos = (tx * 32 * 4) + ((textures[id].h - 1 - (ty * 32)) * textures[id].w * 4);
    BYTE* result = new BYTE[32*32*4];
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 32; x++){
            //int pos = startpos + (x * 4) + ((textures[id].h - y - 1) * textures[id].w * 4);
            int pos = startpos + (x * 4) - (y * textures[id].w * 4);
            int respos = (x * 4) + ((32 - 1 - y) * 32 * 4);
            if (rev) respos = ((32 - x - 1) * 4) + ((32 - 1 - y) * 32 * 4);
            if ((tiles[bx + (by * w)].green || tiles[bx + (by * w)].blue) && (!tiles[bx + (by * w)].red || (tiles[bx + (by * w)].red && tiles[bx + (by * w)].green && tiles[bx + (by * w)].blue))) result[respos] = tex[pos] / 4; //R
            else result[respos] = tex[pos];
            if ((tiles[bx + (by * w)].red || tiles[bx + (by * w)].blue) && (!tiles[bx + (by * w)].green || (tiles[bx + (by * w)].red && tiles[bx + (by * w)].green && tiles[bx + (by * w)].blue))) result[respos + 1] = tex[pos + 1] / 4; //G
            else result[respos + 1] = tex[pos + 1];
            if ((tiles[bx + (by * w)].red || tiles[bx + (by * w)].green) && (!tiles[bx + (by * w)].blue || (tiles[bx + (by * w)].red && tiles[bx + (by * w)].green && tiles[bx + (by * w)].blue))) result[respos + 2] = tex[pos + 2] / 4; //B
            else result[respos + 2] = tex[pos + 2];
            result[respos + 3] = tex[pos + 3];
        }
    return result;
    return NULL;
}

void renderWorld(string worldname, WorldItem* items, int h, int w){
    if (!items) return;
    ILuint DonePic;
    ILenum Error;
    ilGenImages(1, &DonePic);
    ilBindImage(DonePic);
    ilTexImage((w * 32), (h * 32), 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
    ilBindImage(DonePic);
    BYTE* image = ilGetData();
    log("Making full image");
    memset(image, 0, (w * 32 * h * 32 * 4));
    for (int i = 0; i < (w * 32 * h * 32 * 4); i+=4){
        image[i] = 96;
        image[i + 1] = 215;
        image[i + 2] = 242;
        image[i + 3] = 255;
    }
    log("Made background");
    ILuint temp;
    ilGenImages(1, &temp);
    ilBindImage(temp);
    ilTexImage((w * 32), (h * 32), 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
    image = ilGetData();
    log("Background");
    memset(image, 0, (w * 32 * h * 32 * 4));
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++){
            int bg = items[x + (y * w)].background;
            if (bg == 0 || bg % 2 != 0){
                continue;
            }
            BYTE* tex = getTexture(bg, items, x, y, w, h);
            if (tex){
                ilSetPixels(x * 32, y * 32, 0, 32, 32, 1, IL_RGBA, IL_UNSIGNED_BYTE, tex);
            }
            if (tex && tex != textures[bg].texture) delete[] tex;
        }
    log("Made Background (block)");
    ilBindImage(DonePic);
    ilOverlayImage(temp, 0, 0, 0);
    ilDeleteImages(1, &temp);
    ilGenImages(1, &temp);
    ilBindImage(temp);
    ilTexImage((w*32), (h*32), 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
    image = ilGetData();
    log("Make Foreground");
    memset(image, 0, (w * 32 * h * 32 * 4));
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++){
            int fg = items[x + (y * w)].foreground;
            if (fg == 0 || fg % 2 != 0){
                continue;
            }
            BYTE* tex = getTexture(fg, items, x, y, w, h);
            if (tex){
                ilSetPixels(x * 32, y * 32, 0, 32, 32, 1, IL_RGBA, IL_UNSIGNED_BYTE, tex);
            }
            if (tex && tex != NULL && tex != textures[fg].texture) delete[] tex;
        }
    log("Made Foreground");
    {
        log("Making Shadows");
        ILuint sh;
        ilGenImages(1, &sh);
        ilBindImage(sh);
        ilTexImage((w * 32), (h * 32), 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, NULL);
        BYTE* shadow = ilGetData();
        memset(shadow, 0, (w * 32 * h * 32 * 4));
        if (shadow){
            for (int y = 0; y < (h * 32); y++)
                for (int x = 0; x < (w * 32); x++){
                    if (x + 4 < (w*32) && y + 4 < (h * 32)) {
                        int shpos = ((x + 4) * 4) + ((y + 4) * (w * 32) * 4);
                        int rpos = x * 4 + (y * (w * 32) * 4);
                        shadow[rpos + 3] = image[shpos + 3] / 2;
                    } 
                }
        }
        ilBindImage(DonePic);
        ilOverlayImage(sh, 0, 0, 0);
        ilDeleteImages(1, &sh);
        log("Made shadows");
    }
    ilBindImage(DonePic);
    ilOverlayImage(temp, 0, 0, 0);
    ilDeleteImages(1, &temp);
    ilBindImage(DonePic);
        
    ilEnable(IL_FILE_OVERWRITE);
    log("Saving");
    string name = saveDirectory + worldname + ".png";
    ilSaveImage(name.c_str());
    log("Saved");
    ilDeleteImages(1, &DonePic);
    log("Delete");
}

void connectClient(string hostName, int port) {
	client = enet_host_create(NULL /* create a client host */,
		1 /* only allow 1 outgoing connection */,
		2 /* allow up 2 channels to be used, 0 and 1 */,
		0 /* 56K modem with 56 Kbps downstream bandwidth */,
		0 /* 56K modem with 14 Kbps upstream bandwidth */);
	if (client == NULL) {
		cout << "Error while creating client, clsoing..." << endl;
		exit(-1);
	}
	ENetAddress address;
	client->checksum = enet_crc32;
	enet_host_compress_with_range_coder(client);
	enet_address_set_host(&address, hostName.c_str());
	address.port = port;
	peer = enet_host_connect(client, &address, 2, 0);
	if (peer == NULL) {
		cout << "Can't create peer, closing..." << endl;
		exit(-1);
	}
	enet_host_flush(client);
}

int main(){
    cout << "World Renderer Beta by Esto for GTace" << endl;
    using namespace std::chrono;
    long long started = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
    serializeItems();
    ilInit();
    loadTextures();
    {
        vector<texture> tttt = textures;
        textures.clear();
        int ssss = tttt.size();
        for (int i = 0; i < ssss; i++){
            for(int b = 0; b < tttt.size(); b++) {
                if (tttt[b].itemID == i) {
                    textures.push_back(tttt[b]);
                    break;
                }
            }
        }
    }
    int end = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - started;
    cout << "Intialized in " << to_string(end) << endl;
    enet_initialize();
	string server_ip = "127.0.0.1";
	connectClient(server_ip, 23272);
    connected = true;
	ENetEvent event;
	while (true) {
        if (!connected){
            connectClient(server_ip, 23272);
            connected = true;
        }
		while (enet_host_service(client, &event, 1000) > 0) {
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				cout << "We connected!" << endl;
                connected = true;
				enet_peer_ping_interval(peer, 100);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				cout << "Disconnected from server. Trying to reconnect..." << endl;
                connected = false;
				break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
                char zero = 0;
				memcpy(event.packet->data + event.packet->dataLength - 1, &zero, 1);
				string pdata = (char*)(event.packet->data);
				log("We received something!");
                if (!event.peer->data || event.peer->data != "OkServ"){
                    if (pdata == "DuckSpins24H"){
                        log("answering");
                        string msg = "NoGD!!!ASDASDF";
						ENetPacket* packet;
						packet = enet_packet_create(msg.c_str(), msg.length() + 1, ENET_PACKET_FLAG_RELIABLE);
						enet_peer_send(peer, 0, packet);
                        event.peer->data = (char*)"OkServ";
                    } else enet_peer_disconnect_now(event.peer, 0);
                } else if (event.peer->data == "OkServ"){
                    BYTE* data = new BYTE[event.packet->dataLength - 1];
                    memcpy(data, event.packet->data, event.packet->dataLength - 1);
                    log("Got packet from verif server");
                    if (data[0] != 0xFA){
                        log("Ignoring this packet. (Invalid header)");
                        delete[] data;
                        enet_packet_destroy(event.packet);
                        continue;
                    }
                    int memoff = 1;
                    int w = 0;
                    int h = 0;
                    int namelen = 0;
                    string worldname = "";
                    memcpy(&namelen, data + memoff, 2);
                    memoff += 2;
                    log(to_string(namelen));
                    for (int i = 0; i < namelen; i++) {
                        worldname += data[memoff];
                        memoff++;
                    }
                    log(worldname);
                    memcpy(&w, data + memoff, 4);
                    memoff += 4;
                    log(to_string(w));
                    memcpy(&h, data + memoff, 4);
                    memoff += 4;
                    log(to_string(h));
                    int square = w * h;
                    WorldItem* itemss = new WorldItem[square];
                    for (int i = 0; i < square; i++){
                        WorldItem item;
                        memcpy(&item.foreground, data + memoff, 4);
                        memoff += 4;
                        memcpy(&item.background, data + memoff, 4);
                        memoff += 4;
                        memcpy(&item.locked, data + memoff, 2);
                        memoff += 2;
                        short visual = 0;
                        memcpy(&visual, data + memoff, 2);
                        memoff += 2;
                        item.red = (visual & 0x2000);
                        item.green = (visual & 0x4000);
                        item.blue = (visual & 0x8000);
                        item.enabled = (visual & 0x0040);
                        item.isRotated = (visual & 0x0020);
                        item.water = (visual & 0x0400);
                        item.fire = (visual & 0x1000);
                        item.glue = (visual & 0x0800);
                        if (item.foreground % 2 == 0) itemss[i] = item;
                    }
                    WorldItem* items = new WorldItem[square];
                    for (int y = h - 1; y >= 0; y--)
                        for (int x = 0; x < w; x++){
                            WorldItem item = itemss[((h - y - 1) * w) + x];
                            items[(y * w) + x] = item;
                        }
                    log("items");
                    if (w >= 0 && h >= 0 && worldname.length() > 0) renderWorld(worldname, items, h, w);
                    delete[] itemss;
                    delete[] items;
                    delete[] data;
                } else enet_peer_disconnect_now(event.peer, 0);
			}
				enet_packet_destroy(event.packet);
				break;
			default:
				break;
			}
		}
	}
}
