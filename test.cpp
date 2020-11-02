#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <string>
using namespace std;

int main ()
{
    string test = "55";
    string team;
    int yd_line = 0;

    istringstream iss(test);
    iss >> team >> yd_line;

    if(iss.fail())
    {
        yd_line = team.compare("NA") == 0 ? 0 : stoi(team);
    }

    printf("%d\n", yd_line);
}