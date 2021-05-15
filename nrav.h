#include <string>
using namespace std;

class RushPlay
{
public:
    RushPlay(string rusher,
             int year,
             int week,
             string posteam,
             string posteam_type,
             string defteam,
             int yardline_100,
             int yards_to_go,
             int yards_gained,
             bool rush_td,
             bool two_point_attempt,
             string two_point_res,
             bool first_down,
             bool tackled_for_loss,
             bool fumble);
    string rusher;
    int year;
    int week;
    string posteam;
    string posteam_type;
    string defteam;
    int line_of_scrimmage;
    int rush_end;
    int first_down_marker;
    bool rush_td;
    bool two_point;
    bool first_down;
    bool tackled_for_loss;
    bool fumble;
};

void create_jgraph(multimap<int, RushPlay> rushes);