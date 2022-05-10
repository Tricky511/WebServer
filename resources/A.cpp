#include <iostream>
#include <time.h>
using namespace std;
int main()
{
    srand(time(0));
    freopen("test1", "w", stdout);
    for (int i = 0; i < 100000; i++)
    {
        int t = rand() % 62;
        if (t < 10)
            printf("%c", '0' + t);
        else if (t < 36)
            printf("%c", 'a' + t - 10);
        else
            printf("%c", 'A' + t - 36);
    }
    fclose(stdout);
    return 0;
}