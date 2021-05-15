/*
 *  Jonathan Ting
 *  Professor Plank
 *  COSC 494
 *  12 October 2020
 *  Lab 3: Jgraph
 *  Overview: NFL Rushing Attempts Visualization Tool
 */
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include "nrav.h"

int main(int argc, char *argv[])
{
    multimap<int, RushPlay> rushes;
    map<string, vector<string>> team_details;
    vector<string> row;
    string line;
    string output_file;
    string rusher;
    string temp;
    unsigned long long game_id = 0;
    int input_week = 0;
    int week = 0;
    int year = 0;
    bool found_game = false;
    bool skipped_labels = false;

    // Sets the arguments from the command-line.
    output_file = argv[1];
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

    if (access("jgraph/jgraph", X_OK))
    {
        printf("Making jgraph executable...");
        chmod("jgraph/jgraph", S_IXUSR | S_IXGRP | S_IXOTH);
    }
    else
    {
        printf("Already executable...");
    }

    // Runs curl to download the archived csv file of the NFL play-by-play data from nflfastR-data on GitHub.
    oss << "curl -s -S -L -o data.csv.gz https://raw.githubusercontent.com/guga31bb/nflfastR-data/master/data/play_by_play_" << year << ".csv.gz";
    system(oss.str().c_str());

    // Unzips the archived csv file into usable csv data.
    system("gunzip data.csv.gz");

    // Reads in the csv data to begin parsing.
    ifstream fin("data.csv");

    if (fin.fail())
    {
        fprintf(stderr, "ERROR: Couldn't find the data file for the given year.\n");
        remove("data.csv.gz");
        return 1;
    }

    // Parses through the csv file for plays belonging to the specified player in the specified week.
    parse_data(rushes, fin, input_week, rusher, year);

    if (rushes.size() == 0)
    {
        fprintf(stderr, "Rushing plays could not be found for the given inputs.\n");
        remove("data.csv");
        return 1;
    }

    // Make jgraph executable.
    system("curl -L -o team_data.csv https://raw.githubusercontent.com/nflverse/nflfastR-data/master/teams_colors_logos.csv");

    // Reads in the csv data to begin parsing.
    ifstream fin2("team_data.csv");

    parse_team_details(team_details, fin2);

    // Creates the jgraph using the rushing data.
    create_jgraph(rushes, team_details);

    ostringstream oss2;

    // Runs the jgraph command, creating an image with the given output file name.
    oss2 << "./jgraph/jgraph -P fbf.jgr | ps2pdf - | convert -density 300 - -quality 100 " << output_file << ".jpg";
    system(oss2.str().c_str());

    // Deletes the temporary jgraph file and csv data after finished and return.
    remove("fbf.jgr");
    remove("data.csv");
    return 0;
}
