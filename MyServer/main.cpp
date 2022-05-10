
#include "webserver.h"
#include <unistd.h>

int main()
{
    Webserver server(1316, 60000, 4, 200);
    server.Start();
}