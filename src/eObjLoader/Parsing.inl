
namespace eobj
{
    template<class V>
    bool parse(Line &line, std::vector<V> &list)
    {
        V out(0);
        line.m_it += 2;
        if (parse_vector<V>(line, out)) {
            list.push_back(out);
            return true;
        }
        return false;
    }
}
