/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2018, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* <DESC>
 * Download a given URL into a local file named page.out.
 * </DESC>
 */

#include <vector>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include <map>

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
    this->tackled_for_loss = tackled_for_loss;
    this->fumble = fumble;
}

int main(int argc, char *argv[])
{
    fstream fin;
    int week = 0;
    int count = 0;
    int input_week = 6;
    string rusher = "D.Henry";
    unsigned long long game_id = 0; 
    vector<string> row;
    string word, temp;
    bool skipped_labels = false;
    bool found_game = false;
    map<int, RushPlay> rushes;

    system("curl -o data.csv.gz https://raw.githubusercontent.com/guga31bb/nflfastR-data/master/data/play_by_play_2020.csv.gz");
    system("gunzip data.csv.gz");

    fin.open("data.csv", ios::in);

    while (getline(fin, temp))
    {
        row.clear();

        if(!skipped_labels)
        {
            skipped_labels = true;
            continue;
        }

        istringstream s(temp);

        while (getline(s, word, ','))
        {
            row.push_back(word);
        }

        if (found_game && stoull(row[2], NULL, 0) != game_id)
            break;

        // Week
        week = stoi(row[6]);

        if(week > input_week)
            break;

        string temp = row[171];
        int temp2 = rusher.compare(temp);
        unsigned long long temp3 = stoull(row[2]);
        int temp4 = stoi(row[0]);

        if (week == input_week && rusher.compare(temp) == 0)
        {
            if(!found_game)
            {
                found_game = true;
                game_id = stoull(row[2]);
            }

            RushPlay rp(stoi(row[11]), stoi(row[25]), stoi(row[29]), stoi(row[152]), stoi(row[155]), row[45], stoi(row[116]), stoi(row[142]), stoi(row[143]));

            rushes.insert(pair<int, RushPlay>(rp.line_of_scrimmage, rp));
        }
    }

    remove("data.csv");
    return 0;
}

// TODO: Exit right away if name of rusher is NA