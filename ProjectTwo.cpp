#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Course
{
    string number;                // e.g., "CSCI300"
    string title;                 // e.g., "Introduction to Algorithms"
    vector<string> prerequisites; // e.g., ["CSCI200", "MATH201"]
};

static string trim(const string &s)
{
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start])))
        start++;
    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1])))
        end--;
    return s.substr(start, end - start);
}

static string toUpperStr(string s)
{
    for (char &ch : s)
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    return s;
}

// Splits a CSV line on commas (simple CSV; no quoted commas expected for this assignment)
static vector<string> splitCSVLine(const string &line)
{
    vector<string> tokens;
    string token;
    stringstream ss(line);
    while (getline(ss, token, ','))
    {
        tokens.push_back(trim(token));
    }
    return tokens;
}

static void printMenu()
{
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
}

static bool loadCoursesFromFile(const string &filename,
                                unordered_map<string, Course> &courses)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error: Could not open file \"" << filename << "\"." << endl;
        return false;
    }

    courses.clear();

    string line;
    int lineNumber = 0;

    while (getline(file, line))
    {
        lineNumber++;
        line = trim(line);
        if (line.empty())
            continue;

        vector<string> parts = splitCSVLine(line);

        // Need at least courseNumber and title
        if (parts.size() < 2)
        {
            cout << "Warning: Skipping invalid line " << lineNumber
                 << " (missing course number or title)." << endl;
            continue;
        }

        Course c;
        c.number = toUpperStr(parts[0]);
        c.title = parts[1];

        // Remaining parts are prerequisites (0..N)
        for (size_t i = 2; i < parts.size(); i++)
        {
            string prereq = toUpperStr(parts[i]);
            if (!prereq.empty())
            {
                c.prerequisites.push_back(prereq);
            }
        }

        // Insert/overwrite by course number
        courses[c.number] = c;
    }

    cout << "Loaded " << courses.size() << " courses." << endl;
    return true;
}

static vector<string> getSortedCourseNumbers(const unordered_map<string, Course> &courses)
{
    vector<string> keys;
    keys.reserve(courses.size());
    for (const auto &kv : courses)
        keys.push_back(kv.first);

    sort(keys.begin(), keys.end()); // alphanumeric (CSCI100 ... MATH201)
    return keys;
}

static void printCourseList(const unordered_map<string, Course> &courses)
{
    vector<string> sorted = getSortedCourseNumbers(courses);

    cout << "Here is a sample schedule:" << endl;
    for (const string &num : sorted)
    {
        auto it = courses.find(num);
        if (it != courses.end())
        {
            cout << it->second.number << ", " << it->second.title << endl;
        }
    }
}

static void printSingleCourse(const unordered_map<string, Course> &courses)
{
    cout << "What course do you want to know about? ";
    string input;
    getline(cin, input);

    string key = toUpperStr(trim(input));
    auto it = courses.find(key);

    if (it == courses.end())
    {
        cout << "Course not found: " << key << endl;
        return;
    }

    const Course &c = it->second;
    cout << c.number << ", " << c.title << endl;

    if (c.prerequisites.empty())
    {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";
    for (size_t i = 0; i < c.prerequisites.size(); i++)
    {
        const string &pnum = c.prerequisites[i];
        cout << pnum;

        // Also print prerequisite title if we have it
        auto pit = courses.find(pnum);
        if (pit != courses.end())
        {
            cout << " (" << pit->second.title << ")";
        }

        if (i + 1 < c.prerequisites.size())
            cout << ", ";
    }
    cout << endl;
}

int main()
{
    unordered_map<string, Course> courses;
    bool loaded = false;

    cout << "Welcome to the course planner." << endl;

    while (true)
    {
        printMenu();
        cout << "What would you like to do? ";

        string choiceLine;
        getline(cin, choiceLine);
        choiceLine = trim(choiceLine);

        int choice = -1;
        try
        {
            choice = stoi(choiceLine);
        }
        catch (...)
        {
            choice = -1;
        }

        if (choice == 1)
        {
            cout << "Enter file name: ";
            string filename;
            getline(cin, filename);
            filename = trim(filename);

            if (filename.empty())
            {
                cout << "Error: Filename cannot be empty." << endl;
                loaded = false;
                continue;
            }

            loaded = loadCoursesFromFile(filename, courses);
        }
        else if (choice == 2)
        {
            if (!loaded)
            {
                cout << "Please load the data structure first (Option 1)." << endl;
            }
            else
            {
                printCourseList(courses);
            }
        }
        else if (choice == 3)
        {
            if (!loaded)
            {
                cout << "Please load the data structure first (Option 1)." << endl;
            }
            else
            {
                printSingleCourse(courses);
            }
        }
        else if (choice == 9)
        {
            cout << "Thank you for using the course planner!" << endl;
            break;
        }
        else
        {
            if (choiceLine.empty())
            {
                cout << " is not a valid option." << endl;
            }
            else
            {
                cout << choiceLine << " is not a valid option." << endl;
            }
        }
    }

    return 0;
}