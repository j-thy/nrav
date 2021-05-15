/*
 *  Jonathan Ting
 *  Professor Plank
 *  COSC 494
 *  12 October 2020
 *  Lab 3: Jgraph
 *  Overview: NFL Rushing Attempts Visualization Tool
 */
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include "nrav.h"


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
        if (week == input_week && rusher.compare(row[177]) == 0)
        {
            // Marks that the section with the correct game is found.
            if(!found_game)
            {
                found_game = true;
                game_id = stoull(row[2]);
            }

            // Stores all the data pertaining to the rush play, keyed and sorted by the line of scrimmage.
            RushPlay rp(rusher,          // Rusher
                        year,            // Year
                        input_week,
                        row); // Fumble
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
