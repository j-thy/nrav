/*
 *  Jonathan Ting
 *  Professor Plank
 *  COSC 494
 *  12 October 2020
 *  Lab 3: Jgraph
 *  Overview: NFL Rushing Attempts Visualization Tool
 */
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
using namespace std;

// Contains the data on the rushing plays for a player.
class RushPlay
{
public:
    RushPlay(string rusher, int year, int week, string posteam, string posteam_type, string defteam, int yardline_100, int yards_to_go, int yards_gained, bool rush_td, bool two_point_attempt, string two_point_res, bool first_down, bool tackled_for_loss, bool fumble);
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

// Sets all the fields of information for the rushing play.
RushPlay::RushPlay(string rusher, int year, int week, string posteam, string posteam_type, string defteam, int yardline_100, int yards_to_go, int yards_gained, bool rush_td, bool two_point_attempt, string two_point_res, bool first_down, bool tackled_for_loss, bool fumble)
{
    this->rusher = rusher;
    this->year = year;
    this->week = week;
    this->posteam = posteam;
    this->posteam_type = posteam_type.compare("home") == 0 ? "vs." : "@";
    this->defteam = defteam;
    line_of_scrimmage = yardline_100;
    rush_end = yardline_100 - yards_gained;
    first_down_marker = yardline_100 - yards_to_go;
    this->rush_td = rush_td;
    two_point = (two_point_attempt && two_point_res.compare("success") == 0) ? true : false;
    this->first_down = first_down;
    this->tackled_for_loss = tackled_for_loss;
    this->fumble = fumble;
}

void create_jgraph(multimap<int, RushPlay> rushes);

int main(int argc, char *argv[])
{
    multimap<int, RushPlay> rushes;
    vector<string> row;
    string line;
    string rusher;
    string temp;
    string output_file_name;
    unsigned long long game_id = 0;
    int week = 0;
    int input_week = 0;
    int year = 0;
    bool skipped_labels = false;
    bool found_game = false;

    // Sets the arguments from the command-line.
    output_file_name = argv[1];
    rusher = argv[2];
    input_week = stoi(argv[3]);
    year = stoi(argv[4]);

    if (input_week < 1 || input_week > 21)
    {
        fprintf(stderr, "ERROR: Week must be inbetween 1-21 (inclusive).\n");
        return 1;
    }

    if (year < 1999)
    {
        fprintf(stderr, "ERROR: Play-by-play data only goes back as far as 1999.\n");
        return 1;
    }

    ostringstream oss;

    // Make jgraph executable.
    system("chmod +x ./jgraph/jgraph");

    // Runs curl to download the archived csv file of the NFL play-by-play data from nflfastR-data on GitHub.
    oss << "curl -L -o data.csv.gz https://raw.githubusercontent.com/guga31bb/nflfastR-data/master/data/play_by_play_" << year << ".csv.gz";
    system(oss.str().c_str());

    // Unzips the archived csv file into usable csv data.
    system("gunzip data.csv.gz");

    // Reads in the csv data to begin parsing.
    ifstream fin("data.csv");

    if(fin.fail())
    {
        fprintf(stderr, "ERROR: Couldn't find the data file for the given year.\n");
        remove("data.csv.gz");
        return 1;
    }

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
        if (found_game && stoull(row[2], NULL, 0) != game_id)
            break;

        // Grabs the week of the play from the data.
        week = stoi(row[6]);

        // Ends parsing if the row is past the specified week.
        if(week > input_week)
            break;

        // Stores the play data if the week and the rusher name matches.
        if (week == input_week && rusher.compare(row[171]) == 0)
        {
            // Marks that the section with the correct game is found.
            if(!found_game)
            {
                found_game = true;
                game_id = stoull(row[2]);
            }

            // Stores all the data pertaining to the rush play, keyed and sorted by the line of scrimmage.
            RushPlay rp(rusher, year, input_week, row[7], row[8], row[9], stoi(row[11]), stoi(row[25]), stoi(row[29]), stoi(row[152]), stoi(row[155]), row[45], stoi(row[116]), stoi(row[142]), stoi(row[143]));
            rushes.insert(pair<int, RushPlay>(rp.line_of_scrimmage, rp));
        }
    }

    if(rushes.size() == 0)
    {
        fprintf(stderr, "Rushing plays could not be found for the given inputs.\n");
        remove("data.csv");
        return 1;
    }

    // Creates the jgraph using the rushing data.
    create_jgraph(rushes);
    
    ostringstream oss2;

    // Runs the jgraph command, creating an image with the given output file name.
    oss2 << "./jgraph/jgraph -P fbf.jgr | ps2pdf - | convert -density 300 - -quality 100 " << output_file_name << ".jpg";
    system(oss2.str().c_str());

    // Deletes the temporary jgraph file and csv data after finished and return.
    remove("fbf.jgr");
    remove("data.csv");
    return 0;
}

// Converts RGB values to a number between 0 and 1 useable by jgraph.
double rgb_converter(double rgb)
{
    return rgb / 255;
}

void create_jgraph(multimap<int, RushPlay> rushes)
{
    // A map with data for the endzone decoration.
    // Contains the team name, rgb value for primary colors, and rgb value for secondary colors.
    const map<string, string> team_colors
    {
        { "ARI", "CARDINALS 151 35 63 0 0 0" },
        { "ATL", "FALCONS 167 25 48 0 0 0" },
        { "BAL", "RAVENS 26 25 95 0 0 0" },
        { "BUF", "BILLS 0 51 141 198 12 48" },
        { "CAR", "PANTHERS 0 133 202 16 24 32" },
        { "CHI", "BEARS 11 22 42 200 56 3" },
        { "CIN", "BENGALS 251 79 20 0 0 0" },
        { "CLE", "BROWNS 49 29 0 255 60 0" },
        { "DAL", "COWBOYS 0 34 68 255 255 255" },
        { "DEN", "BRONCOS 251 79 20 0 34 68" },
        { "DET", "LIONS 0 118 182 176 183 188" },
        { "GB", "PACKERS 24 48 40 255 184 28" },
        { "HOU", "TEXANS 3 32 47 167 25 48" },
        { "IND", "COLTS 0 44 95 162 170 173" },
        { "JAX", "JAGUARS 16 24 32 215 162 42" },
        { "KC", "CHIEFS 227 24 55 255 184 28" },
        { "LAC", "CHARGERS 0 42 94 255 194 14" },
        { "LA", "RAMS 0 53 148 255 163 0" },
        { "LV", "RAIDERS 0 0 0 165 172 175" },
        { "MIA", "DOLPHINS 0 142 151 252 76 2" },
        { "MIN", "VIKINGS 79 38 131 255 198 47" },
        { "NE", "PATRIOTS 0 34 68 198 12 48" },
        { "NO", "SAINTS 211 188 141 16 24 31" },
        { "NYG", "GIANTS 1 35 82 163 13 45" },
        { "NYJ", "JETS 18 87 64 0 0 0" },
        { "PHI", "EAGLES 0 76 84 165 172 175" },
        { "PIT", "STEELERS 255 182 18 16 24 32" },
        { "SEA", "SEAHAWKS 0 34 68 105 190 40" },
        { "SF", "49ERS 170 0 0 173 153 93" },
        { "TB", "BUCCANEERS 213 10 10 255 121 0" },
        { "TEN", "TITANS 12 35 64 75 146 219" },
        { "WAS", "WASHINGTON 63 16 16 255 182 18" }
    };

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
    istringstream iss(team_colors.find(rushes.begin()->second.posteam)->second);
    iss >> pos_team_name >> pos_team_red >> pos_team_blue >> pos_team_green >> pos_team_2nd_red >> pos_team_2nd_blue >> pos_team_2nd_green;
    istringstream iss2(team_colors.find(rushes.begin()->second.defteam)->second);
    iss2 >> def_team_name >> def_team_red >> def_team_blue >> def_team_green >> def_team_2nd_red >> def_team_2nd_blue >> def_team_2nd_green;

    // Create the endzones.
    ofs << "newcurve marktype box marksize " << x_mark << " 10 cfill " << rgb_converter(def_team_red) << " " << rgb_converter(def_team_blue) << " " << rgb_converter(def_team_green) << " pts " << x_pts << " 105\n";
    ofs << "newcurve marktype box marksize " << x_mark << " 10 cfill " << rgb_converter(pos_team_red) << " " << rgb_converter(pos_team_blue) << " " << rgb_converter(pos_team_green) << " pts " << x_pts << " -5\n";
    ofs << "newstring fontsize 24 hjc vjc font Times-Bold lcolor " << rgb_converter(def_team_2nd_red) << " " << rgb_converter(def_team_2nd_blue) << " " << rgb_converter(def_team_2nd_green) << " x " << title_pos << " y 105 : " << def_team_name << "\n";
    ofs << "newstring fontsize 24 hjc vjc font Times-Bold rotate 180 lcolor " << rgb_converter(pos_team_2nd_red) << " " << rgb_converter(pos_team_2nd_blue) << " " << rgb_converter(pos_team_2nd_green) << " x " << title_pos << " y -5 : " << pos_team_name << "\n";

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
        double center_pos = 100 - ((double)(rush.line_of_scrimmage + rush.rush_end) / 2);
        double x_size = 0.5;
        double line_width = 0.7;
        double y_size = abs(rush.line_of_scrimmage - rush.rush_end);
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
            int y_pos = rush.rush_end > rush.line_of_scrimmage ? 100 - rush.rush_end - 1 : 100 - rush.rush_end + 1;
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
        if (rush.first_down_marker != 0 && rush.line_of_scrimmage != rush.first_down_marker)
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
