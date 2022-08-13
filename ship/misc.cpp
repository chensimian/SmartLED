#include <chrono>
#include <fstream>
#include <thread>
#include <sstream>
#include <iosfwd>
#include <iostream>
#include <cstdio>
#include <cstdlib>
//#include <filesystem>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>

/*
#ifdef USE_STD_REGEX
#include <regex>
#else
*/
#include <jpcre2.hpp>
typedef jpcre2::select<char> jp;
//#endif // USE_STD_REGEX

#include <rapidjson/document.h>

/*
#ifdef USE_MBEDTLS
#include <mbedtls/md5.h>
#else
#include <openssl/md5.h>
#endif // USE_MBEDTLS
*/

#include "misc.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
//#include <io.h>
#include <windows.h>
#include <winreg.h>
#else
#ifndef __hpux
#include <sys/select.h>
#endif /* __hpux */
#ifndef _access
#define _access access
#endif // _access
#include <sys/socket.h>
#endif // _WIN32

void sleep(int interval)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(interval));
}

bool regMatch(const std::string &src, const std::string &match)
{
    jp::Regex reg;
    reg.setPattern(match).addModifier("m").addPcre2Option(PCRE2_ANCHORED|PCRE2_ENDANCHORED|PCRE2_UTF).compile();
    if(!reg)
        return false;
    return reg.match(src, "g");
}

bool regFind(const std::string &src, const std::string &match)
{
    jp::Regex reg;
    reg.setPattern(match).addModifier("m").addPcre2Option(PCRE2_UTF|PCRE2_ALT_BSUX).compile();
    if(!reg)
        return false;
    return reg.match(src, "g");
}

std::string regReplace(const std::string &src, const std::string &match, const std::string &rep, bool global, bool multiline)
{
    jp::Regex reg;
    reg.setPattern(match).addModifier(multiline ? "m" : "").addPcre2Option(PCRE2_UTF|PCRE2_MULTILINE|PCRE2_ALT_BSUX).compile();
    if(!reg)
        return src;
    return reg.replace(src, rep, global ? "gx" : "x");
}

bool regValid(const std::string &reg)
{
    jp::Regex r;
    r.setPattern(reg).addPcre2Option(PCRE2_UTF|PCRE2_ALT_BSUX).compile();
    return !!r;
}

int regGetMatch(const std::string &src, const std::string &match, size_t group_count, ...)
{
    jp::Regex reg;
    reg.setPattern(match).addModifier("m").addPcre2Option(PCRE2_UTF|PCRE2_ALT_BSUX).compile();
    jp::VecNum vec_num;
    jp::RegexMatch rm;
    size_t count = rm.setRegexObject(&reg).setSubject(src).setNumberedSubstringVector(&vec_num).setModifier("g").match();
    if(!count)
        return -1;
    va_list vl;
    va_start(vl, group_count);
    size_t index = 0, match_index = 0;
    while(group_count)
    {
        std::string* arg = va_arg(vl, std::string*);
        if(arg != NULL)
            *arg = std::move(vec_num[match_index][index]);
        index++;
        group_count--;
        if(vec_num[match_index].size() <= index)
        {
            match_index++;
            index = 0;
        }
        if(vec_num.size() <= match_index)
            break;
    }
    va_end(vl);
    return 0;
}

//#endif // USE_STD_REGEX

std::string regTrim(const std::string &src)
{
    return regReplace(src, "^\\s*([\\s\\S]*)\\s*$", "$1", false, false);
}

bool isIPv4(const std::string &address)
{
    return regMatch(address, "^(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)(\\.(25[0-5]|2[0-4]\\d|[0-1]?\\d?\\d)){3}$");
}

bool isIPv6(const std::string &address)
{
    std::vector<std::string> regLists = {"^(?:[0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}$", "^((?:[0-9A-Fa-f]{1,4}(:[0-9A-Fa-f]{1,4})*)?)::((?:([0-9A-Fa-f]{1,4}:)*[0-9A-Fa-f]{1,4})?)$", "^(::(?:[0-9A-Fa-f]{1,4})(?::[0-9A-Fa-f]{1,4}){5})|((?:[0-9A-Fa-f]{1,4})(?::[0-9A-Fa-f]{1,4}){5}::)$"};
    for(unsigned int i = 0; i < regLists.size(); i++)
    {
        if(regMatch(address, regLists[i]))
            return true;
    }
    return false;
}
std::vector<std::string> split(const std::string &s, const std::string &seperator)
{
    std::vector<std::string> result;
    typedef std::string::size_type string_size;
    string_size i = 0;

    while(i != s.size())
    {
        int flag = 0;
        while(i != s.size() && flag == 0)
        {
            flag = 1;
            for(string_size x = 0; x < seperator.size(); ++x)
                if(s[i] == seperator[x])
                {
                    ++i;
                    flag = 0;
                    break;
                }
        }

        flag = 0;
        string_size j = i;
        while(j != s.size() && flag == 0)
        {
            for(string_size x = 0; x < seperator.size(); ++x)
                if(s[j] == seperator[x])
                {
                    flag = 1;
                    break;
                }
            if(flag == 0)
                ++j;
        }
        if(i != j)
        {
            result.push_back(s.substr(i, j-i));
            i = j;
        }
    }
    return result;
}

std::string replace_all_distinct(std::string str, const std::string &old_value, const std::string &new_value)
{
    for(std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())
    {
        if((pos = str.find(old_value, pos)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
    }
    return str;
}

unsigned char FromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z')
        y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z')
        y = x - 'a' + 10;
    else if (x >= '0' && x <= '9')
        y = x - '0';
    else
        y = x;
    return y;
}

std::string UrlDecode(const std::string& str)
{
    std::string strTemp;
    string_size length = str.length();
    for (string_size i = 0; i < length; i++)
    {
        if (str[i] == '+')
            strTemp += ' ';
        else if (str[i] == '%')
        {
            if(i + 2 >= length)
                return strTemp;
            if(isalnum(str[i + 1]) && isalnum(str[i + 2]))
            {
                unsigned char high = FromHex((unsigned char)str[++i]);
                unsigned char low = FromHex((unsigned char)str[++i]);
                strTemp += high * 16 + low;
            }
            else
                strTemp += str[i];
        }
        else
            strTemp += str[i];
    }
    return strTemp;
}

// UTF8 to ANSI code page (GBK on 936)
std::string UTF8ToACP(const std::string &str_src)
{
#ifdef _WIN32
    const char* strUTF8 = str_src.data();
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    std::string strTemp(szGBK);
    if (wszGBK)
        delete[] wszGBK;
    if (szGBK)
        delete[] szGBK;
    return strTemp;
#else
    return str_src;
#endif
}

bool is_str_utf8(const std::string &data)
{
    const char *str = data.c_str();
    unsigned int nBytes = 0;
    for (unsigned int i = 0; str[i] != '\0'; ++i)
    {
        unsigned char chr = *(str + i);
        if (nBytes == 0)
        {
            if (chr >= 0x80)
            {
                if (chr >= 0xFC && chr <= 0xFD)
                    nBytes = 6;
                else if (chr >= 0xF8)
                    nBytes = 5;
                else if (chr >= 0xF0)
                    nBytes = 4;
                else if (chr >= 0xE0)
                    nBytes = 3;
                else if (chr >= 0xC0)
                    nBytes = 2;
                else
                    return false;
                nBytes--;
            }
        }
        else
        {
            if ((chr & 0xC0) != 0x80)
                return false;
            nBytes--;
        }
    }
    if (nBytes != 0)
        return false;
    return true;
}


std::string trim_of(const std::string& str, char target, bool before, bool after)
{
    if (!before && !after)
        return str;
    std::string::size_type pos = 0;
    if (before)
        pos = str.find_first_not_of(target);
    if (pos == std::string::npos)
    {
        return str;
    }
    std::string::size_type pos2 = str.size() - 1;
    if (after)
        pos2 = str.find_last_not_of(target);
    if (pos2 != std::string::npos)
    {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}

std::string trim(const std::string& str, bool before, bool after)
{
    return trim_of(str, ' ', before, after);
}

// TODO: Add preprocessor option to disable (open web service safety)
std::string fileGet(const std::string &path, bool scope_limit)
{
    std::string content;

    std::FILE *fp = std::fopen(path.c_str(), "rb");
    if(fp)
    {
        std::fseek(fp, 0, SEEK_END);
        long tot = std::ftell(fp);
        /*
        char *data = new char[tot + 1];
        data[tot] = '\0';
        std::rewind(fp);
        std::fread(&data[0], 1, tot, fp);
        std::fclose(fp);
        content.assign(data, tot);
        delete[] data;
        */
        content.resize(tot);
        std::rewind(fp);
        std::fread(&content[0], 1, tot, fp);
        std::fclose(fp);
    }

    /*
    std::stringstream sstream;
    std::ifstream infile;
    infile.open(path, std::ios::binary);
    if(infile)
    {
        sstream<<infile.rdbuf();
        infile.close();
        content = sstream.str();
    }
    */
    return content;
}

bool fileExist(const std::string &path, bool scope_limit)
{
    struct stat st;
    return stat(path.data(), &st) == 0 && S_ISREG(st.st_mode);
}

void ProcessEscapeChar(std::string &str)
{
    string_size pos = str.find('\\');
    while(pos != str.npos)
    {
        if(pos == str.size())
            break;
        switch(str[pos + 1])
        {
        case 'n':
            str.replace(pos, 2, "\n");
            break;
        case 'r':
            str.replace(pos, 2, "\r");
            break;
        case 't':
            str.replace(pos, 2, "\t");
            break;
        default:
            /// ignore others for backward compatibility
            //str.erase(pos, 1);
            break;
        }
        pos = str.find('\\', pos + 1);
    }
}
