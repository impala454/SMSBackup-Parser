/*
    Base64 related code written by DaedalusAlpha (http://stackoverflow.com/a/31322410/1544725)
*/

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <map>

#include "tinyxml2.h"

#pragma warning(disable:4996)

static const unsigned char BASE64[] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255,  62, 255,  63,
                                        52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 255, 255, 255,
                                        255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
                                        15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255,  63,
                                        255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
                                        41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255
                                      };

std::vector<unsigned char> base64Decode(std::string inputString)
{
    // Make sure string length is a multiple of 4
    while ((inputString.size() % 4) != 0)
        inputString.push_back('=');

    size_t inputSize = inputString.size();
    std::vector<unsigned char> ret;
    ret.reserve(3 * inputSize / 4);

    for (size_t i = 0; i < inputSize; i += 4)
    {
        // Get values for each group of four base 64 characters
        unsigned char b4[4];
        b4[0] = (inputString[i + 0] <= 'z') ? BASE64[inputString[i + 0]] : 0xff;
        b4[1] = (inputString[i + 1] <= 'z') ? BASE64[inputString[i + 1]] : 0xff;
        b4[2] = (inputString[i + 2] <= 'z') ? BASE64[inputString[i + 2]] : 0xff;
        b4[3] = (inputString[i + 3] <= 'z') ? BASE64[inputString[i + 3]] : 0xff;

        // Transform into a group of three unsigned chars
        unsigned char b3[3];
        b3[0] = ((b4[0] & 0x3f) << 2) + ((b4[1] & 0x30) >> 4);
        b3[1] = ((b4[1] & 0x0f) << 4) + ((b4[2] & 0x3c) >> 2);
        b3[2] = ((b4[2] & 0x03) << 6) + ((b4[3] & 0x3f) >> 0);

        // Add the byte to the return value if it isn't part of an '=' character (indicated by 0xff)
        if (b4[1] != 0xff) ret.push_back(b3[0]);
        if (b4[2] != 0xff) ret.push_back(b3[1]);
        if (b4[3] != 0xff) ret.push_back(b3[2]);
    }

    return ret;
}

std::string getDateFromEpoch(const std::string &epochStr)
{
    char buffer[80];
    struct tm *timeinfo;
    time_t t;
    unsigned long long epoch64;
    std::stringstream ss(epochStr);

    ss >> epoch64;
    epoch64 /= 1000;
    t = epoch64;
    timeinfo = localtime(&t);
    strftime(buffer, 80, "%Y%m%d_%I%M%S", timeinfo);
    std::string ret(buffer);

    return ret;
}

static void showUsage(std::string name)
{
    std::cerr << "Usage: " << name << " <options> XML_FILE\n"
              << "Options:\n"
              << "-h,--help\tShow this help message\n"
              << "-d,--destination DESTINATION\tSpecify the destination path for extracted images"
              << std::endl;
}

void incrementDisplay(unsigned int messages, unsigned int files)
{
    std::cout << "\r" << messages << " messages parsed, " << files << " files extracted";
    std::cout.flush();
}

int parseFile(const std::string &fileName, const std::string &destDir)
{
    unsigned int textOnly = 0;
    unsigned int date = 0;
    unsigned int fileCount = 0;
    unsigned int textCount = 0;
    unsigned int imagePartCount = 0;
    std::string dateStr;
    std::string partType;
    std::string fileFilename;
    std::vector<unsigned char> fileData;
    std::vector<std::string> types;
    std::map<std::string, unsigned int> partTypeCount;
    std::string fileDataStr;
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement *smsesElem;
    tinyxml2::XMLElement *mmsElem;
    tinyxml2::XMLElement *partsElem;
    tinyxml2::XMLElement *partElem;

    if (doc.LoadFile(fileName.c_str()) != 0)
    {
        std::cout << "XMLError: " << doc.ErrorID() << std::endl;
        return 1;
    }

    smsesElem = doc.FirstChildElement("smses");

    if (smsesElem == NULL)
    {
        std::cout << "XML Error: missing <smses>" << std::endl;
        return 1;
    }

    mmsElem = smsesElem->FirstChildElement("mms");

    types.push_back("image/png");
    types.push_back("image/jpeg");
    types.push_back("image/gif");
    types.push_back("video/3gpp");

    while (mmsElem != NULL)
    {
        dateStr = getDateFromEpoch(mmsElem->Attribute("date"));
        mmsElem->QueryUnsignedAttribute("text_only", &textOnly);

        if (textOnly == 0)
        {
            partsElem = mmsElem->FirstChildElement("parts");

            partElem = partsElem->FirstChildElement("part");

            while (partElem != NULL)
            {
                partType = partElem->Attribute("ct");
                partTypeCount[partType]++;

                if (std::find(types.begin(), types.end(), partType) != types.end())
                {
                    fileFilename = partElem->Attribute("name");

                    if (fileFilename == "null")
                        fileFilename = partElem->Attribute("cl");

                    fileFilename = dateStr + "_" + fileFilename;

                    if (destDir.length() > 0)
                        fileFilename = destDir + "//" + fileFilename;

                    fileData = base64Decode(partElem->Attribute("data"));
                    fileDataStr.assign(fileData.begin(), fileData.end());
                    std::ofstream fileFile(fileFilename.c_str(), std::ios::out | std::ios::binary);
                    fileFile.write(fileDataStr.c_str(), fileDataStr.length());
                    fileCount++;
                    incrementDisplay(textCount, fileCount);
                }

                partElem = partElem->NextSiblingElement("part");
            }
        }

        mmsElem = mmsElem->NextSiblingElement("mms");
        textCount++;
    }

    std::cout << std::endl << "done" << std::endl;

    // Checks all the message part types found, may help find others later
    // for (std::map<std::string, unsigned int>::iterator i = partTypeCount.begin(); i != partTypeCount.end(); ++i)
    //     std::cout << i->first << ": " << i->second << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        showUsage(argv[0]);
        return 1;
    }

    std::string xmlFile;
    std::string destDir;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help"))
        {
            showUsage(argv[0]);
            return 0;
        }
        else if ((arg == "-d") || (arg == "--destination"))
        {
            if (i + 1 < argc)
            {   // Make sure we aren't at the end of argv!
                destDir = argv[++i];
            }
            else
            {   // Uh-oh, there was no argument to the destination option.
                std::cerr << "--destination option requires one argument." << std::endl;
                return 1;
            }
        }
        else
        {
            xmlFile = argv[i];
        }
    }

    std::replace(xmlFile.begin(), xmlFile.end(), '\\', '/');
    std::replace(destDir.begin(), destDir.end(), '\\', '/');

    return parseFile(xmlFile, destDir);
}
