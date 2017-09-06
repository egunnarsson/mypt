#ifndef OBJ_PARSING_H
#define OBJ_PARSING_H

#include <string>
#include <vector>

#include "LoaderDefines.h"

namespace eobj
{
    // trim from start
    inline std::string &ltrim(std::string &s);

    // trim from end
    inline std::string &rtrim(std::string &s);

    // trim from both ends
    inline std::string &trim(std::string &s);

    struct Line {

        std::string m_string;
        std::string::iterator m_it;

        bool readFrom(std::ifstream &stream);
        bool empty() const;
        bool atEnd() const;
        char get() const;
        char get(int i) const;
        char next();

        // std::string token();
    };

    bool equal(const std::string &str, const std::string::iterator &other);
    bool skip_whitespace(Line &line);
    bool parse_int(Line &line, int &out);
    bool parse_uint(Line &line, unsigned int &out);
    bool parse_float(Line &line, float &out);
    template<class V>
    bool parse_vector(Line &line, V &out);
    /*{
    assert(0);
    return false;
    }*/
    template<>
    bool parse_vector(Line &line, eobj::Position &out);
    template<>
    bool parse_vector(Line &line, eobj::Direction &out);
    template<>
    bool parse_vector(Line &line, eobj::Color &out);
    template<>
    bool parse_vector(Line &line, eobj::vec2 &out);
    template<class V>
    bool parse(Line &line, std::vector<V> &list);
    bool parse_color(Line &line, eobj::Color &color);
    bool parse_word(Line &line, std::string &out);
}

#include "Parsing.inl"

#endif