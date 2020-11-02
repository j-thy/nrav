#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
using namespace std;

class RushPlay
{
public:
    RushPlay(int yardline_100, int yards_to_go, int yards_gained, bool rush_td, bool two_point_attempt, string two_point_res, bool first_down, bool tackled_for_loss, bool fumble);
    int line_of_scrimmage;
    int rush_end;
    int first_down_marker;
    bool rush_td;
    bool two_point;
    bool first_down;
    bool tackled_for_loss;
    bool fumble;
};

RushPlay::RushPlay(int yardline_100, int yards_to_go, int yards_gained, bool rush_td, bool two_point_attempt, string two_point_res, bool first_down, bool tackled_for_loss, bool fumble)
{
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
    unsigned long long game_id = 0;
    int week = 0;
    int input_week = 0;
    int count = 0;
    int year = 0;
    bool skipped_labels = false;
    bool found_game = false;

    rusher = argv[1];
    input_week = stoi(argv[2]);
    year = stoi(argv[3]);

    ostringstream oss;

    oss << "curl -o data.csv.gz https://raw.githubusercontent.com/guga31bb/nflfastR-data/master/data/play_by_play_" << year << ".csv.gz";

    system(oss.str().c_str());
    system("gunzip data.csv.gz");

    ifstream fin("data.csv");

    while (getline(fin, line))
    {
        row.clear();

        if(!skipped_labels)
        {
            skipped_labels = true;
            continue;
        }

        istringstream iss(line);
        string field;
        string push_field("");
        bool no_quotes = true;

        while (getline(iss, field, ','))
        {
            if (static_cast<size_t>(std::count(field.begin(), field.end(), '"')) % 2 != 0)
            {
                no_quotes = !no_quotes;
            }

            push_field += field + (no_quotes ? "" : ",");

            if (no_quotes)
            {
                row.push_back(push_field);
                push_field.clear();
            }
        }

        if (found_game && stoull(row[2], NULL, 0) != game_id)
            break;

        // Week
        week = stoi(row[6]);

        if(week > input_week)
            break;

        if (week == input_week && rusher.compare(row[171]) == 0)
        {
            if(!found_game)
            {
                found_game = true;
                game_id = stoull(row[2]);
            }

            bool fumble = stoi(row[143]);

            
            if(fumble)
            {
                string team;
                int yd_line;
                istringstream iss2(row[279]);
                iss2 >> team >> yd_line;
                if(iss2.fail())
                {
                    yd_line = team.compare("NA") == 0 ? stoi(row[11]) : stoi(team);
                }
                if(team.compare(row[7]) == 0)
                    yd_line = 100 - yd_line;
                int yards_gained = stoi(row[11]) - yd_line;

                RushPlay rp(stoi(row[11]), stoi(row[25]), yards_gained, stoi(row[152]), stoi(row[155]), row[45], stoi(row[116]), stoi(row[142]), fumble);
                rushes.insert(pair<int, RushPlay>(rp.line_of_scrimmage, rp));
            }
            else
            {
                RushPlay rp(stoi(row[11]), stoi(row[25]), stoi(row[29]), stoi(row[152]), stoi(row[155]), row[45], stoi(row[116]), stoi(row[142]), fumble);
                rushes.insert(pair<int, RushPlay>(rp.line_of_scrimmage, rp));
            }
        }
    }

    create_jgraph(rushes);
    // Change later for TA based on jgraph location
    system("./jgraph/jgraph -P fbf.jgr | ps2pdf - | convert -density 300 - -quality 100 fbf.jpg");

    remove("fbf.jgr");
    remove("data.csv");
    return 0;
}

void create_jgraph(multimap<int, RushPlay> rushes)
{ // " <<  << "
    ofstream ofs;
    ofs.open("fbf.jgr");

    ofs << "newgraph\n";

    double x_max = (double)(rushes.size()) + 0.8;
    double x_min = 0.2;

    ofs << "xaxis\n";
    ofs << "min " << x_min << " max " << x_max << " size 5\n";
    ofs << "nodraw\n";

    ofs << "yaxis\n";
    ofs << "min 0 max 100 size 9\n";
    ofs << "nodraw\n";

    double x_pts = (x_max + x_min) / 2;
    double x_mark = (x_max - x_min);

    ofs << "newcurve marktype box marksize " << x_mark << " 100 cfill 0.098 0.435 0.047 pts " << x_pts << " 50\n";
    ofs << "newcurve marktype box marksize " << x_mark << " 10 cfill 0.047 0.137 0.251 pts " << x_pts << " 105\n";
    ofs << "newcurve marktype box marksize " << x_mark << " 10 cfill 0.231 0.282 0.173 pts " << x_pts << " -5\n";

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

    for (int i = 10; i < 100; i += 10)
    {
        ofs << "newline gray 1 pts " << pt1 << " " << i << " " << pt2 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt3 << " " << i << " " << pt4 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt5 << " " << i << " " << pt6 << " " << i << "\n";
    }

    for (int i = 10; i < 110; i += 10)
    {
        ofs << "newline gray 1 pts " << x_min << " " << i - 5 << " " << x_max << " " << i - 5 << "\n";
    }

    ofs << "newstring hjc vjc font Times-Italic lgray 1 fontsize 14 x 1.5\n";
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

    for (int i = 1; i < 100; i++)
    {
        ofs << "newline gray 1 pts " << pt1 << " " << i << " " << pt2 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt3 << " " << i << " " << pt4 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt5 << " " << i << " " << pt6 << " " << i << "\n";
        ofs << "newline gray 1 pts " << pt7 << " " << i << " " << pt8 << " " << i << "\n";
    }

    int x_pos = 1;
    for (multimap<int, RushPlay>::iterator it = rushes.begin(); it != rushes.end(); it++)
    {
        RushPlay rush = (*it).second;
        double center_pos = 100 - ((double)(rush.line_of_scrimmage + rush.rush_end) / 2);
        double x_size = 0.5;
        double line_width = 0.7;
        double y_size = abs(rush.line_of_scrimmage - rush.rush_end);
        double red = 0.996;
        double green = 1;
        double blue = 0;

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

        ofs << "newcurve marktype box marksize " << x_size << " " << y_size << " cfill " << red << " " << green << " " << blue << "\n";
        ofs << "pts " << x_pos << " " << center_pos << "\n";

        ofs << "newcurve marktype box marksize " << line_width << " 0.5 cfill 0 0.392 0.863\n";
        ofs << "pts " << x_pos << " " << 100 - rush.line_of_scrimmage << "\n";

        if (rush.first_down_marker != 0 && rush.line_of_scrimmage != rush.first_down_marker)
        {
            ofs << "newcurve marktype box marksize " << line_width << " 0.5 cfill 1 0.784 0.196\n";
            ofs << "pts " << x_pos << " " << 100 - rush.first_down_marker << "\n";
        }

        x_pos++;
    }

    ofs.close();
}
