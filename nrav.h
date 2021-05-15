#include <string>
using namespace std;

// Contains the data on the rushing plays for a player.
class RushPlay
{
public:
    RushPlay(string rusher,
             int year,
             int week,
             vector<string> &row);
             
    double get_center_pos();
    double get_y_size();
    int get_y_pos();
    bool first_down_marker_exists();

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

double rgb_converter(double rgb);
void create_jgraph(multimap<int, RushPlay> rushes);