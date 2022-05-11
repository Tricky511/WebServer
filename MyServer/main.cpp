
#include "webserver.h"
#include <unistd.h>

int main()
{
    Webserver server(9991, 60000, 4, 400);
    server.Start();
}