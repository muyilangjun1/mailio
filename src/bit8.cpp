/*

bit8.cpp
--------

Copyright (C) Tomislav Karastojkovic (http://www.alepho.com).

Distributed under the FreeBSD license, see the accompanying file LICENSE or
copy at http://www.freebsd.org/copyright/freebsd-license.html.

*/


#include <string>
#include <vector>
#include <boost/algorithm/string/trim.hpp>
#include <bit8.hpp>


using std::string;
using std::vector;
using boost::trim_right;


namespace mailio
{


bit8::bit8(codec::line_len_policy_t line_policy) : codec(line_policy)
{
}


vector<string> bit8::encode(const string& text) const
{
    vector<string> enc_text;
    string line;
    string::size_type line_len = 0;
    for (auto ch = text.begin(); ch != text.end(); ch++)
    {
        if (is_allowed(*ch))
        {
            line += *ch;
            line_len++;
        }
        else if (*ch == '\r' && (ch + 1) != text.end() && *(ch + 1) == '\n')
        {
            enc_text.push_back(line);
            line.clear();
            line_len = 0;
            // skip both crlf characters
            ch++;
        }
        else
            throw codec_error("Bad character.");
        
        if (line_len == int(_line_policy))
        {
            enc_text.push_back(line);
            line.clear();
            line_len = 0;
        }
    }
    if (!line.empty())
        enc_text.push_back(line);
    while (!enc_text.empty() && enc_text.back().empty())
        enc_text.pop_back();
    
    return enc_text;
}


string bit8::decode(const vector<string>& text) const
{
    string dec_text;
    for (const auto& line : text)
    {
        if (line.length() > int(_line_policy))
            throw codec_error("Bad line length.");
        
        for (auto ch : line)
        {
            if (!is_allowed(ch))
                throw codec_error("Bad character.");

            dec_text += ch;
        }
        dec_text += "\r\n";
    }
    trim_right(dec_text);
    
    return dec_text;
}


/*
For details see [rfc 2045, section 2.8].
*/
bool bit8::is_allowed(char ch) const
{
    return ((unsigned char)ch > 0 && (unsigned char)ch <= 255 && ch != '\r' && ch != '\n');
}


} // namespace mailio