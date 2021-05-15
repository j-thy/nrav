#include <string>
using namespace std;

#define OLD_GAME_ID 2
#define WEEK 6
#define RUSHER_PLAYER_NAME 177
#define POSTEAM 7
#define POSTEAM_TYPE 8
#define DEFTEAM 9
#define YARDLINE_100 11
#define YDSTOGO 25
#define YARDS_GAINED 29
#define RUSH_TOUCHDOWN 156
#define TWO_POINT_ATTEMPT 159
#define TWO_POINT_CONV_RESULT 45
#define FIRST_DOWN_RUSH 120
#define TACKLED_FOR_LOSS 146
#define FUMBLE_LOST 147

// Contains the data on the rushing plays for a player.
class RushPlay
{
public:
    RushPlay(string rusher, int year, int week, vector<string> &row);
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

double get_rgb(char rgb, string hex);
void create_jgraph(multimap<int, RushPlay>& rushes, map<string, vector<string> >& team_details);
void parse_data(multimap<int, RushPlay>& rushes, ifstream& fin, int input_week, string rusher, int year);
void parse_team_details(map<string, vector<string> >& team_details, ifstream& fin);
