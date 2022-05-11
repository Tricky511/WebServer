#include "request.h"

void Request::init()
{
    method = path = version = "";
    state = LINE;
    header.clear(), post.clear();
}
std::string Request::getpath() const
{
    return path;
}
std::string Request::getmethod() const
{
    return method;
}
std::string Request::getversion() const
{
    return version;
}
std::string Request::getbody() const
{
    return body;
}
bool Request::isKeepAlive() const
{
    if (header.count("Connection") == 1)
    {
        return header.find("Connection")->second == "keep-alive" && version == "1.1";
    }
    return false;
}

bool Request::parse(Buffer &buff)
{
    const char CRLF[] = "\r\n";
    while (state != FINISH)
    {
        const char *end = std::search(buff.gethead(), buff.gettail(), CRLF, CRLF + 2);
        std::string line(buff.gethead(), end);
        switch (state)
        {
        case LINE:
            if (!parseRequestLine(line))
                return false;
            parsePath();
            break;
        case HEADER:
            parseRequestHeader(line);
            if (buff.getLen() <= 2)
                state = FINISH;
            break;
        case BODY:
            parseRequestBody(line);
            break;
        default:
            break;
        }
        if (end == buff.gettail())
            break;
        buff.updhead(end + 2 - buff.gethead());
    }

    return true;
}
bool Request::parseRequestLine(const std::string &line)
{

    size_t pos1 = line.find(' ');
    size_t pos2 = line.find(' ', pos1 + 1);
    if (pos1 == line.npos || pos2 == line.npos)
        return false;
    method = line.substr(0, pos1);
    path = line.substr(pos1 + 1, pos2 - pos1 - 1);
    version = line.substr(pos2 + 6);
    state = HEADER;
    return true;
}
void Request::parseRequestHeader(const std::string &line)
{
    size_t pos = line.find(':');
    if (pos == line.npos)
        state = BODY;
    else
    {
        if (line[pos + 1] == ' ')
            header[line.substr(0, pos)] = line.substr(pos + 2);
        else
            header[line.substr(0, pos)] = line.substr(pos + 1);
    }
}
void Request::parseRequestBody(const std::string &line)
{
    body = line;
    parsePost();
    state = FINISH;
}

void Request::parsePath()
{
    if (path == "/")
    {
        path = "/test";
    }
}
int Request::convertHex(char ch)
{
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return ch;
}

void Request::parsePost()
{
    if (method == "POST")
    {
        if (body.size() == 0)
        {
            return;
        }

        std::string key, value;
        int num = 0;
        int n = body.size();
        int i = 0, j = 0;

        for (; i < n; i++)
        {
            char ch = body[i];
            switch (ch)
            {
            case '=':
                key = body.substr(j, i - j);
                j = i + 1;
                break;
            case '+':
                body[i] = ' ';
                break;
            case '%':
                num = convertHex(body[i + 1]) * 16 + convertHex(body[i + 2]);
                body[i + 2] = num % 10 + '0';
                body[i + 1] = num / 10 + '0';
                i += 2;
                break;
            case '&':
                value = body.substr(j, i - j);
                j = i + 1;
                post[key] = value;
                break;
            default:
                break;
            }
        }
        if (post.count(key) == 0 && j < i)
        {
            value = body.substr(j, i - j);
            post[key] = value;
        }
    }
}