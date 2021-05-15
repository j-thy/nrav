#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <vector>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include "nrav.h"
using namespace std;

// Sets all the fields of information for the rushing play.
RushPlay::RushPlay(string rusher, int year, int week, vector<string> &row)
{
    this->rusher = rusher;
    this->year = year;
    this->week = week;
    posteam = row[POSTEAM];
    posteam_type = row[POSTEAM_TYPE].compare("home") == 0 ? "vs." : "@";
    defteam = row[DEFTEAM];
    line_of_scrimmage = stoi(row[YARDLINE_100]);
    rush_end = line_of_scrimmage - stoi(row[YARDS_GAINED]);
    first_down_marker = line_of_scrimmage - stoi(row[YDSTOGO]);
    rush_td = stoi(row[RUSH_TOUCHDOWN]);
    two_point = (stoi(row[TWO_POINT_ATTEMPT]) && row[TWO_POINT_CONV_RESULT].compare("success") == 0) ? true : false;
    first_down = stoi(row[FIRST_DOWN_RUSH]);
    tackled_for_loss = stoi(row[TACKLED_FOR_LOSS]);
    fumble = stoi(row[FUMBLE_LOST]);
}

double RushPlay::get_center_pos()
{
    return 100 - ((double)(line_of_scrimmage + rush_end) / 2);
}

double RushPlay::get_y_size()
{
    return abs(line_of_scrimmage - rush_end);
}

int RushPlay::get_y_pos()
{
    return rush_end > line_of_scrimmage ? 100 - rush_end - 1 : 100 - rush_end + 1;;
}

bool RushPlay::first_down_marker_exists()
{
    return first_down_marker != 0 && line_of_scrimmage != first_down_marker;
}

// Converts RGB values to a number between 0 and 1 useable by jgraph.
double get_rgb(char rgb, string hex_str)
{
    stringstream ss;
    string rgb_str;
    int val = 0;

    switch(rgb)
    {
        case 'r':
            rgb_str = hex_str.substr(1, 2);
            ss << rgb_str;
            ss >> hex >> val;
            break;
        case 'g':
            rgb_str = hex_str.substr(3, 2);
            ss << rgb_str;
            ss >> hex >> val;
            break;
        case 'b':
            rgb_str = hex_str.substr(5, 2);
            ss << rgb_str;
            ss >> hex >> val;
            break;
        default:
            val = 0.0;
    }

    return (double)val / 255;
}

void create_jgraph(multimap<int, RushPlay>& rushes, map<string, vector<string> >& team_details)
{
    // Opens an output stream for the temporary jgraph source file.
    ofstream ofs;
    ofs.open("fbf.jgr");

    // Creates a new graph.
    ofs << "newgraph\n";

    // Calculates the number of values on the x axis based on the number of plays.
    double x_max = (double)(rushes.size()) + 0.8;
    double x_min = 0.2;

    // Creates the x axis, which is based on the number of plays.
    ofs << "xaxis\n";
    ofs << "min " << x_min << " max " << x_max << " size 5\n";
    ofs << "nodraw\n";

    // Creates the y axis, which is a constant 100 points, for each yard of the football field.
    ofs << "yaxis\n";
    ofs << "min 0 max 100 size 9\n";
    ofs << "nodraw\n";

    double x_pts = (x_max + x_min) / 2;
    double x_mark = (x_max - x_min);

    // Create the green football field.
    ofs << "newcurve marktype box marksize " << x_mark << " 100 cfill 0.098 0.435 0.047 pts " << x_pts << " 50\n";

    double title_pos = (x_max + x_min)/2;
    string pos_team_name;
    string def_team_name;
    string pos_team_abbr;
    string def_team_abbr;
    double pos_team_red = 0.0;
    double pos_team_blue = 0.0;
    double pos_team_green = 0.0;
    double pos_team_2nd_red = 0.0;
    double pos_team_2nd_blue = 0.0;
    double pos_team_2nd_green = 0.0;
    double def_team_red = 0.0;
    double def_team_blue = 0.0;
    double def_team_green = 0.0;
    double def_team_2nd_red = 0.0;
    double def_team_2nd_blue = 0.0;
    double def_team_2nd_green = 0.0;

    // Grab the team name and color data for the home and away team.

    pos_team_abbr = rushes.begin()->second.posteam;
    pos_team_name = team_details[pos_team_abbr][2];
    transform(pos_team_name.begin(), pos_team_name.end(), pos_team_name.begin(), ::toupper);
    pos_team_red = get_rgb('r', team_details[pos_team_abbr][3]);
    pos_team_green = get_rgb('g', team_details[pos_team_abbr][3]);
    pos_team_blue = get_rgb('b', team_details[pos_team_abbr][3]);
    pos_team_2nd_red = get_rgb('r', team_details[pos_team_abbr][4]);
    pos_team_2nd_green = get_rgb('g', team_details[pos_team_abbr][4]);
    pos_team_2nd_blue = get_rgb('b', team_details[pos_team_abbr][4]);

    def_team_abbr = rushes.begin()->second.defteam;
    def_team_name = team_details[def_team_abbr][2];
    transform(def_team_name.begin(), def_team_name.end(), def_team_name.begin(), ::toupper);
    def_team_red = get_rgb('r', team_details[def_team_abbr][3]);
    def_team_green = get_rgb('g', team_details[def_team_abbr][3]);
    def_team_blue = get_rgb('b', team_details[def_team_abbr][3]);
    def_team_2nd_red = get_rgb('r', team_details[def_team_abbr][4]);
    def_team_2nd_green = get_rgb('g', team_details[def_team_abbr][4]);
    def_team_2nd_blue = get_rgb('b', team_details[def_team_abbr][4]);

    // Create the endzones.
    ofs << "newcurve marktype box marksize " << x_mark << " 10 cfill " << def_team_red << " " << def_team_green << " " << def_team_blue << " pts " << x_pts << " 105\n";
    ofs << "newcurve marktype box marksize " << x_mark << " 10 cfill " << pos_team_red << " " << pos_team_green << " " << pos_team_blue << " pts " << x_pts << " -5\n";
    ofs << "newstring fontsize 24 hjc vjc font Times-Bold lcolor " << def_team_2nd_red << " " << def_team_2nd_green << " " << def_team_2nd_blue << " x " << title_pos << " y 105 : " << def_team_name << "\n";
    ofs << "newstring fontsize 24 hjc vjc font Times-Bold rotate 180 lcolor " << pos_team_2nd_red << " " << pos_team_2nd_green << " " << pos_team_2nd_blue << " x " << title_pos << " y -5 : " << pos_team_name << "\n";

    double field_length = x_max - x_min;
    double line_length = field_length - ((double)2/13) * field_length;
    double gap_length = ((double)1/13) * field_length;
    double seg_length = ((double)1/6) * line_length;
    double pt1 = x_min;
    double pt2 = pt1 + seg_length;
    double pt3 = pt2 + gap_length;
    double pt4 = pt3 + 4 * seg_length;
    double pt5 = pt4 + gap_length;
    double pt6 = x_max;

    // Create the lines on the field with gaps for the numbers every 10 yards..
    for (int i = 10; i < 100; i += 10)
    {
        ofs << "newline gray 1 pts " << pt1 << " " << i << " " << pt2 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt3 << " " << i << " " << pt4 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt5 << " " << i << " " << pt6 << " " << i << "\n";
    }

    // Create more solid lines so that there is a line every 5 yards.
    for (int i = 10; i < 110; i += 10)
    {
        ofs << "newline gray 1 pts " << x_min << " " << i - 5 << " " << x_max << " " << i - 5 << "\n";
    }

    // Create the yard number markers.
    ofs << "newstring hjc vjc font Times lgray 1 fontsize 14 x 1.5\n";
    double side_length = seg_length + ((double)1/2) * gap_length;
    pt1 = x_min + side_length;
    pt2 = x_max - side_length;

    for (int i = 1; i < 6; i += 1)
    {
        ofs << "copystring x " << pt1 << " y " << i*10 << " : " << i << "0\n";
        ofs << "copystring x " << pt2 << " y " << i*10 << " : " << i << "0\n";
    }

    for (int i = 6; i < 10; i += 1)
    {
        ofs << "copystring x " << pt1 << " y " << i*10 << " : " << 10-i << "0\n";
        ofs << "copystring x " << pt2 << " y " << i*10 << " : " << 10-i << "0\n";
    }

    double mark_length = ((double)3/130) * field_length;
    double empty_length = field_length - ((double)3/65) * field_length;
    double seg2_length = ((double)1/3) * empty_length;
    pt1 = x_min;
    pt2 = x_min + mark_length;
    pt3 = x_min + seg2_length;
    pt4 = pt3 + mark_length;
    pt5 = pt4 + seg2_length;
    pt6 = pt5 + mark_length;
    double pt7 = x_max - mark_length;
    double pt8 = x_max;

    // Create the 1-yard line markers.
    for (int i = 1; i < 100; i++)
    {
        ofs << "newline gray 1 pts " << pt1 << " " << i << " " << pt2 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt3 << " " << i << " " << pt4 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt5 << " " << i << " " << pt6 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt7 << " " << i << " " << pt8 << " " << i << "\n";
    }

    // Create the graph data for each rushing play, sorted by the line of scrimmage.
    int x_pos = 1;
    for (multimap<int, RushPlay>::iterator it = rushes.begin(); it != rushes.end(); it++)
    {
        RushPlay rush = it->second;
        double center_pos = rush.get_center_pos();
        double x_size = 0.5;
        double line_width = 0.7;
        double y_size = rush.get_y_size();
        double red = 0.996;
        double green = 1;
        double blue = 0;

        // Color codes the bar based on whether it resulted in a rushing TD, first down conversion, short of first down, fumble, or tackle for loss.
        if(rush.rush_td || rush.two_point)
        {
            string type = rush.rush_td ? "TD" : "2PT";
            ofs << "newstring hjc vjc font Helvetica-Bold lcolor 1 1 1 fontsize 8 x " << x_pos << " y " << 100 - rush.rush_end + 1 << " : " << type << "\n";
            red = 0;
            green = 1;
            blue = 0;
        }
        else if(rush.first_down)
        {
            red = 0.498;
            green = 1;
            blue = 0;
        }
        else if(rush.fumble)
        {
            int y_pos = rush.get_y_pos();
            ofs << "newstring hjc vjc font Helvetica-Bold lcolor 1 0 0 fontsize 8 x " << x_pos << " y " << y_pos << " : FUM\n";
            red = 1;
            green = 0;
            blue = 0;
        }
        else if(rush.tackled_for_loss)
        {
            red = 1;
            green = 0.498;
            blue = 0;
        }

        // Creates the yardage bar.
        ofs << "newcurve marktype box marksize " << x_size << " " << y_size << " cfill " << red << " " << green << " " << blue << "\n";
        ofs << "pts " << x_pos << " " << center_pos << "\n";

        // Creates the line of scrimmage.
        ofs << "newcurve marktype box marksize " << line_width << " 0.5 cfill 0 0.392 0.863\n";
        ofs << "pts " << x_pos << " " << 100 - rush.line_of_scrimmage << "\n";

        // Creates the first down marker.
        if (rush.first_down_marker_exists())
        {
            ofs << "newcurve marktype box marksize " << line_width << " 0.5 cfill 1 0.784 0.196\n";
            ofs << "pts " << x_pos << " " << 100 - rush.first_down_marker << "\n";
        }

        x_pos++;
    }

    RushPlay team = rushes.begin()->second;
    double legend_pos1 = x_max + (x_max/50);
    double legend_pos2 = x_max + (x_max/5.4);
    double legend_pos3 = x_max + (x_max/11);

    // Creates the legend without using the built-in legend options.
    ofs << "newstring fontsize 18 hjl vjc font Times-Bold x " << legend_pos1 << " y 99 : NFL Rushing Attempts\n";
    ofs << "newstring fontsize 18 hjl vjc font Times-Bold x " << legend_pos1 << " y 96 : Visualization\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos1 << " y 93 : Rusher: " << team.rusher << "\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos1 << " y 90 : Season: " << team.year << "\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos1 << " y 87 : Week: " << team.week << "\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos1 << " y 84 : Team: " << team.posteam << " " <<  team.posteam_type << " " <<  team.defteam << "\n";

    ofs << "newstring fontsize 18 hjl vjc font Times-Bold x " << legend_pos1 << " y 65 : Yards Gained\n";

    ofs << "newcurve marktype box marksize 0.5 5 cfill 0 1 0\n";
    ofs << "pts " << legend_pos3 << " 60\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos2 << " y 61 : Touchdown or\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos2 << " y 59 : 2 Point Conversion\n";

    ofs << "newcurve marktype box marksize 0.5 5 cfill 0.498 1 0\n";
    ofs << "pts " << legend_pos3 << " 50\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos2 << " y 50 : First Down Converted\n";

    ofs << "newcurve marktype box marksize 0.5 5 cfill 0.996 1 0\n";
    ofs << "pts " << legend_pos3 << " 40\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos2 << " y 40 : Short of First Down\n";

    ofs << "newcurve marktype box marksize 0.5 5 cfill 1 0.498 0\n";
    ofs << "pts " << legend_pos3 << " 30\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos2 << " y 30 : Tackled for Loss\n";

    ofs << "newcurve marktype box marksize 0.5 5 cfill 1 0 0\n";
    ofs << "pts " << legend_pos3 << " 20\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos2 << " y 20 : Fumble Lost\n";

    ofs << "newstring fontsize 18 hjl vjc font Times-Bold x " << legend_pos1 << " y 10 : Line Markers\n";

    ofs << "newcurve marktype box marksize 0.7 0.5 cfill 1 0.784 0.196\n";
    ofs << "pts " << legend_pos3 << " 5\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos2 << " y 5 : First Down Line\n";

    ofs << "newcurve marktype box marksize 0.7 0.5 cfill 0 0.392 0.863\n";
    ofs << "pts " << legend_pos3 << " 0\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos2 << " y 0 : Line of Scrimmage\n";

    // Closes the output file.
    ofs.close();
}

void parse_data(multimap<int, RushPlay>& rushes, ifstream& fin, int input_week, string rusher, int year)
{
    vector<string> row;
    string line;
    bool skipped_labels = false;
    bool found_game = false;
    unsigned long long game_id = 0;
    int week = 0;

    // Parses through the csv file for plays belonging to the specified player in the specified week.
    while (getline(fin, line))
    {
        row.clear();

        // Skip the first row of the csv file since it's just labels.
        if(!skipped_labels)
        {
            skipped_labels = true;
            continue;
        }

        istringstream iss(line);
        string field;
        string push_field("");
        bool no_quotes = true;

        // Separate the columns based on the commas.
        while (getline(iss, field, ','))
        {
            // Logic to properly read in quoted strings which may contain commas.
            if (static_cast<size_t>(std::count(field.begin(), field.end(), '"')) % 2 != 0)
            {
                no_quotes = !no_quotes;
            }

            // Push each field into a vector.
            push_field += field + (no_quotes ? "" : ",");

            if (no_quotes)
            {
                row.push_back(push_field);
                push_field.clear();
            }
        }

        // Since the data is sorted by games, ends parsing once the rows pertaining to the specified game is exhausted.
        if (found_game && stoull(row[OLD_GAME_ID], NULL, 0) != game_id)
            break;

        // Grabs the week of the play from the data.
        week = stoi(row[WEEK]);

        // Ends parsing if the row is past the specified week.
        if(week > input_week)
            break;

        // Stores the play data if the week and the rusher name matches.
        if (week == input_week && rusher.compare(row[RUSHER_PLAYER_NAME]) == 0)
        {
            // Marks that the section with the correct game is found.
            if(!found_game)
            {
                found_game = true;
                game_id = stoull(row[OLD_GAME_ID]);
            }

            // Stores all the data pertaining to the rush play, keyed and sorted by the line of scrimmage.
            RushPlay rp(rusher, year, input_week, row);
            rushes.insert(pair<int, RushPlay>(rp.line_of_scrimmage, rp));
        }
    }
}

void parse_team_details(map<string, vector<string> >& team_details, ifstream& fin)
{
    string line;
    bool skipped_labels = false;

    // Parses through the csv file for plays belonging to the specified player in the specified week.
    while (getline(fin, line))
    {
        // Skip the first row of the csv file since it's just labels.
        if(!skipped_labels)
        {
            skipped_labels = true;
            continue;
        }

        istringstream iss(line);
        string field;
        string abbr;
        bool abbr_saved = false;

        // Separate the columns based on the commas.
        while (getline(iss, field, ','))
        {
            // Skip the first row of the csv file since it's just labels.
            if(!abbr_saved)
            {
                vector<string> details;
                abbr = field;
                team_details.insert(pair<string, vector<string> >(abbr, details));
                abbr_saved = true;
                continue;
            }

            // Push each field into a vector.
            team_details[abbr].push_back(field);
        }
    }
}