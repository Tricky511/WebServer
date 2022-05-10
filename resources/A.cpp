#include <iostream>
#include <time.h>
using namespace std;

int main()
{
    freopen("test", "w", stdout);
    for (int i = 0; i < 100000; i++)
    {
        int c = rand() % 62;
        if (c < 10)
            printf("%c", '0' + c);
        else if (c < 36)
            printf("%c", 'a' + c - 10);
        else
            printf("%c", 'A' + c - 36);
    }
    fclose(stdout);
    return 0;
}