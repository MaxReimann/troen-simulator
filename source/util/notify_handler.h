#include <streambuf>

void Log(const char *msg)
{
#ifdef _MSC_VER
#ifdef _UNICODE
    wchar_t buf[1024];
    MultiByteToWideChar(CP_ACP, 0, msg, -1, buf, 1024);
    OutputDebugString(buf);
#else
    OutputDebugString(msg);
#endif
#endif
}

class OsgMsgTrap : public std::streambuf
{
public:
    inline virtual int_type overflow(int_type c =
std::streambuf::traits_type::eof())
    {
        if (c == std::streambuf::traits_type::eof()) return
std::streambuf::traits_type::not_eof(c);
        char str[2];
        str[0] = c;
        str[1] = 0;
        Log(str);
        return c;
    }
} g_Trap;