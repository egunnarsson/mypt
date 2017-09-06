
#include <algorithm>
#include <functional>
#include <cctype>
#include <fstream>

#include "Parsing.h"

namespace eobj 
{
    // http://stackoverflow.com/a/217605/1620667

    // trim from start
    inline std::string &ltrim(std::string &s) 
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    // trim from end
    inline std::string &rtrim(std::string &s) 
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    // trim from both ends
    inline std::string &trim(std::string &s) 
    {
        return ltrim(rtrim(s));
    }

    bool Line::readFrom(std::ifstream &stream)
    {
        if (std::getline(stream, m_string).fail())
            return false;
        trim(m_string);
        m_it = m_string.begin();
        return true;
    }

    bool Line::empty() const
    {
        return m_string.empty();
    }

    bool Line::atEnd() const
    {
        return m_it >= m_string.end();
    }

    char Line::get() const
    {
        return m_it < m_string.end() ? *m_it : '\0';
    }

    char Line::get(int i) const
    {
        auto it2 = m_it + i;
        return it2 < m_string.end() ? *it2 : '\0';
    }

    char Line::next()
    {
        ++m_it;
        return get();
    }

    bool equal(const std::string &str, const std::string::iterator &other)
    {
        return std::equal(str.begin(), str.end(), other);
    }

    bool skip_whitespace(Line &line)
    {
        line.m_it = std::find_if_not(line.m_it, line.m_string.end(), std::isspace);
        return !line.atEnd();
    }

    bool parse_int(Line &line, int &out)
    {
        out = 0;
        bool negative = false;
        if (skip_whitespace(line))
        {
            if (line.get() == '-')
            {
                negative = true;
                line.next();
            }
            if (!std::isdigit(line.get()))
            {
                return false;
            }
            while (std::isdigit(line.get()))
            {
                out = out * 10 + unsigned int(line.get() - '0');
                line.next();
            }
            if (negative)
            {
                out = -out;
            }
            return true;
        }
        return false;
    }

    bool parse_uint(Line &line, unsigned int &out)
    {
        // TODO cleanup, this is copied from old
        out = 0;
        if (skip_whitespace(line)) {
            if (!std::isdigit(line.get())) return false;
            while (std::isdigit(line.get())) {
                out = out * 10 + unsigned int(line.get() - '0');
                line.next();
            }
            return true;
        }
        return false;
    }

    bool parse_float(Line &line, float &out)
    {
        // TODO cleanup, this is copied from old
        if (skip_whitespace(line)) {

            //test cases '-.' '-.0' '0.' whats valid and not... '0.0E+1'

            out = 0;
            float sign;

            if (std::isdigit(line.get())) {
                sign = 1;
            }
            else {
                sign = line.get() == '-' ? -1.0f : (line.get() == '+' ? 1.0f : 0.0f);
                if (!sign) {
                    return false;
                }
                line.next();
            }

            while (std::isdigit(line.get())) {
                out = out * 10 + float(line.get() - '0');
                line.next();
            }

            if (line.get() == '.') {
                line.next();
                float frac = 0.1f;
                while (std::isdigit(line.get())) {
                    out += frac * float(line.get() - '0');
                    frac *= 0.1f;
                    line.next();
                }
            }

            if (line.get() == 'e' || line.get() == 'E') {
                line.next();
                float expSign = 1;
                if (line.get() == '-') {
                    expSign = -1;
                    line.next();
                }
                else if (line.get() == '+') {
                    line.next();
                }
                unsigned int exp;
                if (!parse_uint(line, exp)) {
                    return false;
                }
                out = out * powf(10.0f, exp * expSign);
            }

            out *= sign;

            return true;
        }

        return false;
    }

    template<>
    bool parse_vector(Line &line, eobj::Position &out)
    {
        return parse_float(line, out.x) && parse_float(line, out.y) && parse_float(line, out.z);
    }

    template<>
    bool parse_vector(Line &line, eobj::Direction &out)
    {
        return parse_float(line, out.x) && parse_float(line, out.y) && parse_float(line, out.z);
    }

    template<>
    bool parse_vector(Line &line, eobj::Color &out)
    {
        return parse_float(line, out.x) && parse_float(line, out.y) && parse_float(line, out.z);
    }

    template<>
    bool parse_vector(Line &line, eobj::vec2 &out)
    {
        return parse_float(line, out.x) && parse_float(line, out.y);
    }

    bool parse_color(Line &line, eobj::Color &color)
    {
        if (skip_whitespace(line))
        {
            switch (line.get())
            {
            case 's':
                return false;
            case 'x':
            {

            }
            default:
                return parse_vector<eobj::Color>(line, color);
            }
        }
        return false;
    }

    bool parse_word(Line &line, std::string &out)
    {
        if (skip_whitespace(line))
        {
            auto start = line.m_it;

            for (char current = line.get();
                current != '\0' &&
                current != ' ' &&
                current != '\t' &&
                current != '\n' &&
                current != '\r';
                current = line.next());

            out = std::string(start, line.m_it);
            return true;
        }
        return false;
    }
}
