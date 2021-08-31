#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <algorithm> // std::find

using namespace std;
/** takes brainfuck program as input, outputs the output of that program
 * maybe-TODO: compile bf into cpp
 * TODO: more tests
 */

//globals//////
bool verbose = false;
///////////////
void handleINC(vector<uint8_t> &tape, uint &ptr);
void handleINC(vector<uint8_t> &tape, uint &ptr)
{
    // cout << tape.size() << " " << ptr << endl;
    tape.at(ptr)++;
}
void handleDEC(vector<uint8_t> &tape, uint &ptr)
{
    // cout << tape.size() << " " << ptr << endl;
    tape.at(ptr)--;
}
void makeTapeRoom(vector<uint8_t> &tape, uint &ptr)
{
    if (ptr >= tape.size())
    {
        tape.push_back(0);
    }
}
bool isInputRoom(vector<uint8_t> &input_bytes, uint &input_ptr)
{
    if (input_ptr >= input_bytes.size())
    {
        cout << "ERR: access too many inputs, exiting..";
        return false;
    }
    return true;
}
////////////////////////////////////////////////////////////////

// bool input_flag = false;
int recursiveMainLoop(bool original, vector<uint8_t> &input_bytes, vector<uint8_t> &bytes, vector<uint8_t> &tape, uint &ptr, char &byte, ifstream &input_file, ofstream &output_file, uint &input_ptr, uint &loop_lookahead_index, uint8_t &curr_byte, uint8_t &bytes_index)
{
    int16_t loop_start_index = bytes_index - 1;
    while (bytes_index < bytes.size())
    {

        curr_byte = bytes.at(bytes_index);
        if (verbose)
        {
            cout << curr_byte << endl;
            for (auto &i : tape)
            {
                if (i > 9)
                    cout << i << " ";
                else
                    cout << +i << " ";
                // if (i != 0)
                //     cout << i << " |";
                // else
                //     cout << "  |";
            }
            cout << endl;
            for (int i = 0; i < tape.size(); i++)
            {
                if (i == ptr)
                {
                    cout << "^";
                }
                else
                {
                    cout << "  ";
                }
                // cout << "x";
            }
            cout << endl;
        }
        switch (curr_byte)
        {
        case '+':
            // cout << "inc  ";
            // makeTapeRoom(tape, ptr);
            handleINC(tape, ptr);
            break;
        case '-':
            // cout << "dec  ";
            // makeTapeRoom(tape, ptr);
            handleDEC(tape, ptr);
            break;
        case '.':
            // cout << "prn ";
            // cout << tape.at(ptr) << endl;
            output_file << tape.at(ptr); // prints int instead of char
            break;
        case ',':
            // cout << "inp ";
            if (!isInputRoom(input_bytes, input_ptr))
                exit(-1);
            tape.at(ptr) = input_bytes.at(input_ptr++);
            // cout << tape.at(ptr) << endl;
            break;
        case '<':
            ptr--;
            break;
        case '>':
            ptr++;
            makeTapeRoom(tape, ptr);
            break;
        case '[':
            if (tape.at(ptr) == 0)
            {
                // cout << "---------loop bypassed" << endl;
                // i think i don't need lookahead anymore
                loop_lookahead_index = 1;
                for (std::vector<uint8_t>::const_iterator it = bytes.begin() + bytes_index; it != bytes.end(); it++)
                {
                    if (*it == (uint8_t)']')
                    {
                        bytes_index += loop_lookahead_index;
                        continue;
                    }
                    loop_lookahead_index++;
                }
            }
            else
            { // enter loop
                if (loop_start_index != -1)
                { // fun times begin
                    // cout << "recursive loop start" << endl;
                    recursiveMainLoop(0, input_bytes, bytes, tape, ptr, byte, input_file, output_file, input_ptr, loop_lookahead_index, curr_byte, ++bytes_index);
                }
                else
                {
                    // cout << "---------loop start" << endl;
                    loop_start_index = bytes_index;
                }
            }
            break;
        case ']':
            if (tape.at(ptr) == 0)
            { // we exit the loop
                // cout << "---------loop end" << endl;

                if (original)
                {
                    bytes_index++;
                    loop_start_index = -1;
                    continue;
                }
                else
                    return 1;
            }
            else
            { // back to the loop start
                // cout << "-----back to loop start" << endl;
                // when we get here loop start index is 255 for some reason??
                bytes_index = loop_start_index;
            }
            break; // likely just exited a layer of recursion
        }
        bytes_index++;
        // cout << endl;
    }
    return 1;
}
////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    string filename("source");
    string outfilename("output");

    // using std::find with vector and iterator:
    std::vector<string> myvector(argv, argv + argc);
    std::vector<string>::iterator it;

    it = find(myvector.begin(), myvector.end(), "-h");
    if (it != myvector.end())
    {
        myvector.erase(it);
        cout << "./brainfuck[source][-o output][-v][-h | --help] " << endl;
        verbose = true;
        return 1;
        // std::cout
        //     << "Element found in myvector: " << *it << '\n';
    }
    else
    {
        it = find(myvector.begin(), myvector.end(), "--help");
        if (it != myvector.end())
        {
            myvector.erase(it);
            cout << "./brainfuck [source] [-o output] [-v] [-h | --help] " << endl;
            verbose = true;
            return 1;
            // std::cout
            //     << "Element found in myvector: " << *it << '\n';
        }
    }
    it = find(myvector.begin(), myvector.end(), "-v");
    if (it != myvector.end())
    {
        myvector.erase(it);
        cout << "(v)erbose tape printing on" << endl;
        verbose = true;
        // std::cout
        //     << "Element found in myvector: " << *it << '\n';
    }
    it = find(myvector.begin(), myvector.end(), "-o");
    if (it != myvector.end())
    {
        myvector.erase(it); // erase -o
        cout << "(o)utput filename: " << *it << endl;

        outfilename = *it;
        outfilename += ".txt";
        myvector.erase(it); // erase outfilename
    }
    else
    {
        if (verbose)
        {
            cout << "no output filename specified, using default: output.txt" << endl;
        }
    }
    if (myvector.size() == 1)
    {
        cerr << "specify source filename" << endl;
        return -1;
    }

    filename = myvector.at(1); // last thing left after we parse flags is the source file
    filename += ".b";

    vector<uint8_t>
        input_bytes,
        bytes, tape{0, 0, 0, 0, 0};
    uint ptr = 0;  // index of head on tape
    char byte = 0; // current instruction

    if (argc > 3)
    {
        for (int i = 0; i < strlen(argv[3]); i++)
        {
            input_bytes.push_back(argv[3][i]);
        }
    }

    ifstream input_file(filename);
    if (!input_file.is_open())
    {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        return -1;
    }

    ofstream output_file(outfilename);
    // output_file.open("example.txt");
    if (!output_file.is_open())
    {
        cerr << "Could not open the file - '"
             << outfilename << "'" << endl;
        return -1;
    }
    // const vector<uint8_t> &retVal = tape;
    bool input_flag = false;
    while (input_file.get(byte))
    {
        if (byte == '!')
        {
            // cout << "input section of bf program reached\n";
            input_flag = true;
            continue;
        }
        if (input_flag)
        {
            input_bytes.push_back(byte);
        }
        else
        {
            bytes.push_back(byte);
        }
    }
    uint input_ptr = 0;
    uint loop_lookahead_index = 1;
    int16_t loop_start_index = -1;
    uint8_t curr_byte, bytes_index;
    bytes_index = 0;
    // for (auto &byte : bytes)
    // pass 1 for outer loop so it doesn't return until eof
    recursiveMainLoop(1, input_bytes, bytes, tape, ptr, byte, input_file, output_file, input_ptr, loop_lookahead_index, curr_byte, bytes_index);

    // cout << endl;

    input_file.close();
    output_file.close();
    return 0;
}
