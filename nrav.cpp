#include "nrav.h"

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

// Gets center position of bar.
double RushPlay::get_center_pos()
{
    return 100 - ((double)(line_of_scrimmage + rush_end) / 2);
}

// Gets vertical size of bar.
double RushPlay::get_y_size()
{
    return abs(line_of_scrimmage - rush_end);
}

// Gets vertical position of bar.
int RushPlay::get_y_pos()
{
    return rush_end > line_of_scrimmage ? 100 - rush_end - 1 : 100 - rush_end + 1;
}

// Checks if a first down marker is needed.
bool RushPlay::first_down_marker_exists()
{
    return first_down_marker != 0 && line_of_scrimmage != first_down_marker;
}

// Gets the color's hex value and converts it a number between 0 and 1 usable by jgraph.
double get_rgb(char rgb, string hex_str)
{
    int val = 0;

    switch (rgb)
    {
    case 'r':
        val = stoi(hex_str.substr(1, 2), 0, 16);
        break;
    case 'g':
        val = stoi(hex_str.substr(3, 2), 0, 16);
        break;
    case 'b':
        val = stoi(hex_str.substr(5, 2), 0, 16);
        break;
    default:
        val = 0.0;
    }

    return (double)val / 255;
}

// Creates the jgraph using the rushing data.
void create_jgraph(multimap<int, RushPlay> &rushes, map<string, vector<string>> &team_details)
{
    vector<Rgb> pos_team(2, Rgb());
    vector<Rgb> def_team(2, Rgb());
    TeamName team_name;
    TeamName team_abbr;
    ofstream ofs;
    double empty_length = 0.0;
    double field_length = 0.0;
    double gap_length = 0.0;
    double legend_pos1 = 0.0;
    double legend_pos2 = 0.0;
    double legend_pos3 = 0.0;
    double line_length = 0.0;
    double mark_length = 0.0;
    double pt1 = 0.0;
    double pt2 = 0.0;
    double pt3 = 0.0;
    double pt4 = 0.0;
    double pt5 = 0.0;
    double pt6 = 0.0;
    double pt7 = 0.0;
    double pt8 = 0.0;
    double seg_length = 0.0;
    double seg2_length = 0.0;
    double side_length = 0.0;
    double title_pos = 0.0;
    double x_mark = 0.0;
    double x_max = 0.0;
    double x_min = 0.0;
    double x_pts = 0.0;
    int x_pos = 0;

    // Opens an output stream for the temporary jgraph source file.
    ofs.open("temp.jgr");

    // Creates a new graph.
    ofs << "newgraph\n";

    // Calculates the number of values on the x axis based on the number of plays.
    x_max = (double)(rushes.size()) + 0.8;
    x_min = 0.2;

    // Creates the x axis, which is based on the number of plays.
    ofs << "xaxis\n";
    ofs << "min " << x_min << " max " << x_max << " size 5\n";
    ofs << "nodraw\n";

    // Creates the y axis, which is a constant 100 points, for each yard of the football field.
    ofs << "yaxis\n";
    ofs << "min 0 max 100 size 9\n";
    ofs << "nodraw\n";

    x_pts = (x_max + x_min) / 2;
    x_mark = (x_max - x_min);

    // Creates the green football field.
    ofs << "newcurve marktype box marksize " << x_mark << " 100 cfill 0.098 0.435 0.047 pts " << x_pts << " 50\n";

    title_pos = (x_max + x_min) / 2;

    // Grab the team name and color data for the home and away team.
    team_abbr.pos = rushes.begin()->second.posteam;
    team_name.pos = team_details[team_abbr.pos][2];
    transform(team_name.pos.begin(), team_name.pos.end(), team_name.pos.begin(), ::toupper);

    pos_team[0].red = get_rgb('r', team_details[team_abbr.pos][3]);
    pos_team[0].green = get_rgb('g', team_details[team_abbr.pos][3]);
    pos_team[0].blue = get_rgb('b', team_details[team_abbr.pos][3]);
    pos_team[1].red = get_rgb('r', team_details[team_abbr.pos][4]);
    pos_team[1].green = get_rgb('g', team_details[team_abbr.pos][4]);
    pos_team[1].blue = get_rgb('b', team_details[team_abbr.pos][4]);

    team_abbr.def = rushes.begin()->second.defteam;
    team_name.def = team_details[team_abbr.def][2];
    transform(team_name.def.begin(), team_name.def.end(), team_name.def.begin(), ::toupper);

    def_team[0].red = get_rgb('r', team_details[team_abbr.def][3]);
    def_team[0].green = get_rgb('g', team_details[team_abbr.def][3]);
    def_team[0].blue = get_rgb('b', team_details[team_abbr.def][3]);
    def_team[1].red = get_rgb('r', team_details[team_abbr.def][4]);
    def_team[1].green = get_rgb('g', team_details[team_abbr.def][4]);
    def_team[1].blue = get_rgb('b', team_details[team_abbr.def][4]);

    // Creates the endzones.
    ofs << "newcurve marktype box marksize " << x_mark << " 10 cfill " << def_team[0].red << " " << def_team[0].green << " " << def_team[0].blue << " pts " << x_pts << " 105\n";
    ofs << "newcurve marktype box marksize " << x_mark << " 10 cfill " << pos_team[0].red << " " << pos_team[0].green << " " << pos_team[0].blue << " pts " << x_pts << " -5\n";
    ofs << "newstring fontsize 24 hjc vjc font Times-Bold lcolor " << def_team[1].red << " " << def_team[1].green << " " << def_team[1].blue << " x " << title_pos << " y 105 : " << team_name.def << "\n";
    ofs << "newstring fontsize 24 hjc vjc font Times-Bold rotate 180 lcolor " << pos_team[1].red << " " << pos_team[1].green << " " << pos_team[1].blue << " x " << title_pos << " y -5 : " << team_name.pos << "\n";

    field_length = x_max - x_min;
    line_length = field_length - ((double)2 / 13) * field_length;
    gap_length = ((double)1 / 13) * field_length;
    seg_length = ((double)1 / 6) * line_length;
    pt1 = x_min;
    pt2 = pt1 + seg_length;
    pt3 = pt2 + gap_length;
    pt4 = pt3 + 4 * seg_length;
    pt5 = pt4 + gap_length;
    pt6 = x_max;

    // Creates the lines on the field with gaps for the numbers every 10 yards.
    for (int i = 10; i < 100; i += 10)
    {
        ofs << "newline gray 1 pts " << pt1 << " " << i << " " << pt2 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt3 << " " << i << " " << pt4 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt5 << " " << i << " " << pt6 << " " << i << "\n";
    }

    // Creates more solid lines so that there is a line every 5 yards.
    for (int i = 10; i < 110; i += 10)
    {
        ofs << "newline gray 1 pts " << x_min << " " << i - 5 << " " << x_max << " " << i - 5 << "\n";
    }

    // Creates the yard number markers.
    ofs << "newstring hjc vjc font Times lgray 1 fontsize 14 x 1.5\n";
    side_length = seg_length + ((double)1 / 2) * gap_length;
    pt1 = x_min + side_length;
    pt2 = x_max - side_length;

    for (int i = 1; i < 6; i += 1)
    {
        ofs << "copystring x " << pt1 << " y " << i * 10 << " : " << i << "0\n";
        ofs << "copystring x " << pt2 << " y " << i * 10 << " : " << i << "0\n";
    }

    for (int i = 6; i < 10; i += 1)
    {
        ofs << "copystring x " << pt1 << " y " << i * 10 << " : " << 10 - i << "0\n";
        ofs << "copystring x " << pt2 << " y " << i * 10 << " : " << 10 - i << "0\n";
    }

    mark_length = ((double)3 / 130) * field_length;
    empty_length = field_length - ((double)3 / 65) * field_length;
    seg2_length = ((double)1 / 3) * empty_length;
    pt1 = x_min;
    pt2 = x_min + mark_length;
    pt3 = x_min + seg2_length;
    pt4 = pt3 + mark_length;
    pt5 = pt4 + seg2_length;
    pt6 = pt5 + mark_length;
    pt7 = x_max - mark_length;
    pt8 = x_max;

    // Create the 1-yard line markers.
    for (int i = 1; i < 100; i++)
    {
        ofs << "newline gray 1 pts " << pt1 << " " << i << " " << pt2 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt3 << " " << i << " " << pt4 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt5 << " " << i << " " << pt6 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt7 << " " << i << " " << pt8 << " " << i << "\n";
    }

    // Create the graph data for each rushing play, sorted by the line of scrimmage.
    x_pos = 1;
    for (multimap<int, RushPlay>::iterator it = rushes.begin(); it != rushes.end(); it++)
    {
        RushPlay rush = it->second;
        double center_pos = rush.get_center_pos();
        double x_size = 0.5;
        double line_width = 0.7;
        double y_size = rush.get_y_size();
        Rgb bar_color = {0.996, 1, 0};

        // Color codes the bar based on whether it resulted in a rushing TD, first down conversion, short of first down, fumble, or tackle for loss.
        if (rush.rush_td || rush.two_point)
        {
            string type = rush.rush_td ? "TD" : "2PT";
            ofs << "newstring hjc vjc font Helvetica-Bold lcolor 1 1 1 fontsize 8 x " << x_pos << " y " << 100 - rush.rush_end + 1 << " : " << type << "\n";
            bar_color.red = 0;
            bar_color.green = 1;
            bar_color.blue = 0;
        }
        else if (rush.first_down)
        {
            bar_color.red = 0.498;
            bar_color.green = 1;
            bar_color.blue = 0;
        }
        else if (rush.fumble)
        {
            int y_pos = rush.get_y_pos();
            ofs << "newstring hjc vjc font Helvetica-Bold lcolor 1 0 0 fontsize 8 x " << x_pos << " y " << y_pos << " : FUM\n";
            bar_color.red = 1;
            bar_color.green = 0;
            bar_color.blue = 0;
        }
        else if (rush.tackled_for_loss)
        {
            bar_color.red = 1;
            bar_color.green = 0.498;
            bar_color.blue = 0;
        }

        // Creates the yardage bar.
        ofs << "newcurve marktype box marksize " << x_size << " " << y_size << " cfill " << bar_color.red << " " << bar_color.green << " " << bar_color.blue << "\n";
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
    legend_pos1 = x_max + (x_max / 50);
    legend_pos2 = x_max + (x_max / 5.4);
    legend_pos3 = x_max + (x_max / 11);

    // Creates the legend without using the built-in legend options.
    ofs << "newstring fontsize 18 hjl vjc font Times-Bold x " << legend_pos1 << " y 99 : NFL Rushing Attempts\n";
    ofs << "newstring fontsize 18 hjl vjc font Times-Bold x " << legend_pos1 << " y 96 : Visualization\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos1 << " y 93 : Rusher: " << team.rusher << "\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos1 << " y 90 : Season: " << team.year << "\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos1 << " y 87 : Week: " << team.week << "\n";
    ofs << "newstring fontsize 14 hjl vjc x " << legend_pos1 << " y 84 : Team: " << team.posteam << " " << team.posteam_type << " " << team.defteam << "\n";

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

// Parses through the csv file for plays belonging to the specified player in the specified week.
void parse_data(multimap<int, RushPlay> &rushes, ifstream &fin, int input_week, string rusher, int year)
{
    vector<string> row;
    string line;
    unsigned long long game_id = 0;
    int week = 0;
    bool found_game = false;
    bool skipped_labels = false;

    // Parses through the csv file for plays belonging to the specified player in the specified week.
    while (getline(fin, line))
    {
        row.clear();

        // Skips the first row of the csv file since it's just labels.
        if (!skipped_labels)
        {
            skipped_labels = true;
            continue;
        }

        istringstream iss(line);
        string field;
        string push_field("");
        bool no_quotes = true;

        // Separates the columns based on the commas.
        while (getline(iss, field, ','))
        {
            // Logic to properly read in quoted strings which may contain commas.
            if (static_cast<size_t>(std::count(field.begin(), field.end(), '"')) % 2 != 0)
            {
                no_quotes = !no_quotes;
            }

            // Pushes each field into a vector.
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
        if (week > input_week)
            break;

        // Stores the play data if the week and the rusher name matches.
        if (week == input_week && rusher.compare(row[RUSHER_PLAYER_NAME]) == 0)
        {
            // Marks that the section with the correct game is found.
            if (!found_game)
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

// Parses the csv file for team names and colors.
void parse_team_details(map<string, vector<string>> &team_details, ifstream &fin)
{
    string line;
    bool skipped_labels = false;

    // Parses through the csv file for plays belonging to the specified player in the specified week.
    while (getline(fin, line))
    {
        // Skips the first row of the csv file since it's just labels.
        if (!skipped_labels)
        {
            skipped_labels = true;
            continue;
        }

        istringstream iss(line);
        string field;
        string abbr;
        bool abbr_saved = false;

        // Separates the columns based on the commas.
        while (getline(iss, field, ','))
        {
            // Skips the first row of the csv file since it's just labels.
            if (!abbr_saved)
            {
                vector<string> details;
                abbr = field;
                team_details.insert(pair<string, vector<string>>(abbr, details));
                abbr_saved = true;
                continue;
            }

            // Pushes each field into a vector.
            team_details[abbr].push_back(field);
        }
    }
}
