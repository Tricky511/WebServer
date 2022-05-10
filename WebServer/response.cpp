#include "response.h"
const std::unordered_map<std::string, std::string> Response::TYPE = {
    {".html", "text/html"},          {".xml", "text/xml"},          {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},          {".rtf", "application/rtf"},   {".pdf", "application/pdf"},
    {".word", "application/nsword"}, {".png", "image/png"},         {".gif", "image/gif"},
    {".jpg", "image/jpeg"},          {".jpeg", "image/jpeg"},       {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},         {".mpg", "video/mpeg"},        {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},   {".tar", "application/x-tar"}, {".css", "text/css "},
    {".js", "text/javascript "},
};

const std::unordered_map<int, std::string> Response::CODE_STATUS = {
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"},
};

Response::Response()
{
    code = -1;
    path = dir = "";
    isKeepAlive = false;
    mfile = nullptr;
    mfileStat = {0};
}
Response::~Response()
{
    unmap();
}
char *Response::file()
{
    return mfile;
}

size_t Response::fileLen() const
{
    return mfileStat.st_size;
}

void Response::unmap()
{
    if (mfile)
    {
        munmap(mfile, mfileStat.st_size);
        mfile = nullptr;
    }
}
void Response::init(const std::string dir, const std::string path, bool isKeepAlive, int code)
{
    if (mfile)
        unmap();
    this->dir = dir;
    this->path = path;
    this->isKeepAlive = isKeepAlive;
    this->code = code;
    mfile = nullptr;
    mfileStat = {0};
}
std::string Response::getFileType()
{
    std::string::size_type pos = path.find_last_of('.');
    if (pos == path.npos)
    {
        return "text/plain";
    }
    std::string suf = path.substr(pos);
    if (TYPE.count(suf))
        return TYPE.find(suf)->second;
    return "text/plain";
}
void Response::errorBody(Buffer &buff, std::string str)
{
    std::string body;
    std::string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if (CODE_STATUS.count(code) == 1)
    {
        status = CODE_STATUS.find(code)->second;
    }
    else
    {
        status = "Bad Request";
    }
    body += std::to_string(code) + " : " + status + "\n";
    body += "<p>" + str + "</p>";
    body += "<hr><em>TinyWebServer</em></body></html>";

    buff.append("Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    buff.append(body);
}
void Response::makeResponseLine(Buffer &buff)
{
    std::string status;
    if (!CODE_STATUS.count(code))
        code = 400;
    status = CODE_STATUS.find(code)->second;
    buff.append("HTTP/1.1 " + std::to_string(code) + " " + status + "\r\n");
}
void Response::makeResponseHeader(Buffer &buff)
{
    buff.append("Connection:");
    if (isKeepAlive)
    {
        buff.append("keep-alive\r\n");
        buff.append("keep-alive: max=6, timeout=120\r\n");
    }
    else
        buff.append("close\r\n");
    buff.append("Content-type: " + getFileType() + "\r\n");
}
void Response::makeResponseBody(Buffer &buff)
{

    int fd = open((dir + path).data(), O_RDONLY);
    if (fd < 0)
    {
        errorBody(buff, "File Not Found!");
        return;
    }

    int *mRet = (int *)mmap(0, mfileStat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (*mRet == -1)
    {
        errorBody(buff, "File Not Found!");
        return;
    }
    mfile = (char *)mRet;
    close(fd);
    buff.append("Content-length: " + std::to_string(mfileStat.st_size) + "\r\n\r\n");
}
void Response::makeResponse(Buffer &buff)
{
    if (stat((dir + path).data(), &mfileStat) < 0)
        code = 404;
    else if (!(mfileStat.st_mode & S_IROTH))
        code = 403;
    else if (code == -1)
        code = 200;
    makeResponseLine(buff);
    makeResponseHeader(buff);
    makeResponseBody(buff);
}