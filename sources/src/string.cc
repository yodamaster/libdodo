/***************************************************************************
 *            string.cc
 *
 *  Thu Jun  10 2010
 *  Copyright  2010  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <libdodo/string.h>

#include <stdio.h>
#include <string.h>

using namespace dodo;

#define ALIGN(x, a) (((x) + ((a) - 1))&~((a) - 1))
#define ALIGN_STRING_BUFFER_SIZE(x) ALIGN(x, (x)>LEVEL1_DCACHE_LINESIZE?PAGE_SIZE:LEVEL1_DCACHE_LINESIZE)

//-------------------------------------------------------------------

string::string() : buf(new char[LEVEL1_DCACHE_LINESIZE]),
                   bufSize(LEVEL1_DCACHE_LINESIZE),
                   strLen(0)
{
    buf[0] = 0x0;
}

//-------------------------------------------------------------------

string::string(const string &s) : buf(NULL),
                                  bufSize(LEVEL1_DCACHE_LINESIZE),
                                  strLen(s.strLen)
{
    if (strLen) {
        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + 1);
        buf = new char[bufSize];
        memcpy(buf, s.buf, strLen);
        buf[strLen] = 0x0;
    } else {
        buf = new char[LEVEL1_DCACHE_LINESIZE];
        buf[0] = 0x0;
    }
}

//-------------------------------------------------------------------

string::string(const char *data) : buf(NULL),
                                   bufSize(LEVEL1_DCACHE_LINESIZE),
                                   strLen(0)
{
    if (data) {
        strLen = strlen(data);
        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + 1);
        buf = new char[bufSize];
        memcpy(buf, data, bufSize);
    } else {
        buf = new char[LEVEL1_DCACHE_LINESIZE];
        buf[0] = 0x0;
    }
}

//-------------------------------------------------------------------

string::string(const char *str,
               unsigned long length) : buf(NULL),
                                       bufSize(LEVEL1_DCACHE_LINESIZE),
                                       strLen(length)
{
    if (str) {
        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + 1);
        buf = new char[bufSize];
        memcpy(buf, str, strLen);
        buf[strLen] = 0x0;
    } else {
        buf = new char[LEVEL1_DCACHE_LINESIZE];
        buf[0] = 0x0;
    }
}

//-------------------------------------------------------------------

string::string(char symbol,
               unsigned long count) : buf(NULL),
                                      bufSize(LEVEL1_DCACHE_LINESIZE),
                                      strLen(count)
{
    if (strLen) {
        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + 1);
        buf = new char[bufSize];
        memset(buf, symbol, strLen);
        buf[strLen] = 0x0;
    } else {
        buf = new char[LEVEL1_DCACHE_LINESIZE];
        buf[0] = 0x0;
    }
}

//-------------------------------------------------------------------

string::~string()
{
    delete [] buf;
}

//-------------------------------------------------------------------

void
string::erase(unsigned long index,
              unsigned long count)
{
    if (count == POSITION_END)
        count = strLen;

    unsigned long end = index + count;


    if (end >= strLen) {
        buf[strLen = index] = 0x0;
    } else {
        strLen -= count;
        memmove(buf+index, buf+end, strLen);
        buf[strLen] = 0x0;
    }
}

//-------------------------------------------------------------------

void
string::clear()
{
    strLen = 0;
    buf[0] = 0x0;
}

//-------------------------------------------------------------------

void
string::resize(unsigned long len)
{
    if (len >= strLen)
        return;

    if (!(len < strLen || bufSize > len)) {
        unsigned long newBufSize = ALIGN_STRING_BUFFER_SIZE(len + 1);

        char *newBuf = new char[newBufSize];

        memcpy(newBuf, buf, bufSize);
        delete [] buf;
        /* memset(newBuf+strLen, 0x0, newBufSize - bufSize); */

        buf = newBuf;
        bufSize = newBufSize;
    }

    strLen = len;
    buf[strLen] = 0x0;
}

//-------------------------------------------------------------------

unsigned long
string::find(const string &str,
             unsigned long index) const
{
    if (index > strLen)
        return POSITION_END;

    char *pos = strstr(buf+index, str.buf);
    if (!pos)
        return POSITION_END;
    else
        return pos - buf;
}

//-------------------------------------------------------------------

string &
string::replace(unsigned long index,
                unsigned long num,
                const string &str)
{
    if (index > strLen)
        return *this;

    if (bufSize < strLen - num + str.strLen) {
        unsigned long newBufSize = ALIGN_STRING_BUFFER_SIZE(strLen - num + str.strLen + 1);

        char *newBuf = new char[newBufSize];

        memcpy(newBuf, buf, bufSize);
        delete [] buf;

        buf = newBuf;
        bufSize = newBufSize;
    }

    if (num != str.strLen)
        memmove(buf+index+str.strLen, buf+index+num, strLen-(index+num));
    memcpy(buf+index, str.buf, str.strLen);
    strLen += (int)str.strLen - (int)num;

    return *this;
}

//-------------------------------------------------------------------

string &
string::insert(unsigned long index,
               const string &str)
{
    if (index > strLen)
        return *this;

    if (bufSize < strLen + str.strLen) {
        unsigned long newBufSize = ALIGN_STRING_BUFFER_SIZE(strLen + str.strLen + 1);

        char *newBuf = new char[newBufSize];

        memcpy(newBuf, buf, bufSize);
        delete [] buf;

        buf = newBuf;
        bufSize = newBufSize;
    }

    memmove(buf+index+str.strLen, buf+index, strLen-index);
    memcpy(buf+index, str.buf, str.strLen);
    strLen += str.strLen;

    return *this;
}

//-------------------------------------------------------------------

string
string::substr(unsigned long index,
               unsigned long length) const
{
    if (index > strLen)
        return string();

    if (length == POSITION_END)
        length = strLen;

    return string(buf+index, (length+index>strLen)?strLen-index:length);
}

//-------------------------------------------------------------------

void
string::reserve(unsigned long count)
{
    if (count > bufSize) {
        bufSize = ALIGN_STRING_BUFFER_SIZE(count + 1);

        char *newBuf = new char[bufSize];

        memcpy(newBuf, buf, strLen);
        delete [] buf;

        buf = newBuf;
    }
}

//-------------------------------------------------------------------

void
string::assign(const char *data,
               unsigned long length)
{
    strLen = length;

    if (bufSize <= strLen) {
        delete [] buf;

        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + 1);
        buf = new char[bufSize];
    }

    memcpy(buf, data, strLen);
    buf[strLen] = 0x0;
}

//-------------------------------------------------------------------

void
string::append(const char *data,
                   unsigned long length)
{
    if (bufSize < strLen + length) {
        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + length + 1);

        char *newBuf = new char[bufSize];

        memcpy(newBuf, buf, strLen);
        delete [] buf;

        buf = newBuf;
    }
    memcpy(buf+strLen, data, length);
    strLen += length;
    buf[strLen] = 0x0;
}

//-------------------------------------------------------------------

string &
string::operator=(const string &data)
{
    strLen = data.strLen;

    if (bufSize <= strLen) {
        delete [] buf;

        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + 1);
        buf = new char[bufSize];
    }

    memcpy(buf, data.buf, bufSize);

    return *this;
}

//-------------------------------------------------------------------

string &
string::operator=(const char *data)
{
    strLen = strlen(data);

    if (bufSize <= strLen) {
        delete [] buf;

        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + 1);
        buf = new char[bufSize];
    }

    memcpy(buf, data, bufSize);

    return *this;
}

//-------------------------------------------------------------------

string &
string::operator=(char ch)
{
    buf[0] = ch;
    buf[1] = 0x0;
    strLen = 1;

    return *this;
}

//-------------------------------------------------------------------

string &
string::operator+=(const string &str)
{
    if (bufSize < strLen + str.strLen) {
        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + str.strLen + 1);

        char *newBuf = new char[bufSize];

        memcpy(newBuf, buf, strLen);
        delete [] buf;

        buf = newBuf;
    }

    memcpy(buf+strLen, str.buf, str.strLen+1);
    strLen += str.strLen;

    return *this;
}

//-------------------------------------------------------------------

string &
string::operator+=(const char *str)
{
    unsigned long len = strlen(str);

    if (bufSize < strLen + len) {
        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + len + 1);

        char *newBuf = new char[bufSize];

        memcpy(newBuf, buf, strLen);
        delete [] buf;

        buf = newBuf;
    }
    memcpy(buf+strLen, str, len+1);
    strLen += len;

    return *this;
}

//-------------------------------------------------------------------

string &
string::operator+=(char c)
{
    if (bufSize < strLen + 1) {
        bufSize = ALIGN_STRING_BUFFER_SIZE(strLen + LEVEL1_DCACHE_LINESIZE + 1);

        char *newBuf = new char[bufSize];

        memcpy(newBuf, buf, strLen);
        delete [] buf;

        buf = newBuf;
    }
    buf[strLen] = c;
    ++strLen;
    buf[strLen] = 0x0;

    return *this;
}

//-------------------------------------------------------------------

namespace dodo {
    bool
    operator<(const string &s1,
              const string &s2)
    {
        return memcmp(s1.buf, s2.buf, s1.strLen) < 0;
    }

    //-------------------------------------------------------------------

    bool
    operator>(const string &s1,
              const string &s2)
    {
        return memcmp(s1.buf, s2.buf, s1.strLen) > 0;
    }

    //-------------------------------------------------------------------

    bool
    operator==(const string &s1,
               const string &s2)
    {
        return memcmp(s1.buf, s2.buf, s1.strLen) == 0;
    }

    //-------------------------------------------------------------------

    bool
    operator==(const string &s1,
               const char *s2)
    {
        return memcmp(s1.buf, s2, s1.strLen) == 0;
    }

    //-------------------------------------------------------------------

    string
    operator+(const string &s1,
              const string &s2)
    {
        string str = s1;
        str += s2;
        return str;
    }

    //-------------------------------------------------------------------

    string
    operator+(const string &s1,
              const char *s2)
    {
        string str = s1;
        str += s2;
        return str;
    }

    //-------------------------------------------------------------------

    string
    operator+(const char *s1,
              const string &s2)
    {
        string str = s1;
        str += s2;
        return str;
    }
};

//-------------------------------------------------------------------
