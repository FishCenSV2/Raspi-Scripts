#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <fstream>

using namespace std;

const int repeats = 100;
const int ms_length = 10000; //60000 = 1 minute

//-exec function obtained from working-solution at https://stackoverflow.com/questions/52164723/how-to-execute-a-command-and-get-return-code-stdout-and-stderr-of-command-in-c
string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    auto pipe = popen(cmd, "r");

    if (!pipe) throw runtime_error("popen() failed.");

    while (!feof(pipe)) {
        if (fgets(buffer.data(), 128, pipe) != nullptr)
            result += buffer.data();
    }

    auto rc = pclose(pipe);

    if (rc == EXIT_SUCCESS) {
        cout << "SUCCESS\n";
    }
    else {
        cout << "FAILED\n";
    }

    return result;
}

int main()
{
    //get and output date and time for verification purposes
    string res, strcmd, dateandtime, fname;
    string test;
    test = "date +%Y%b%d--%I-%M-%S%P-";
    dateandtime = exec(test.c_str());//"date +%Y%b%d--%I-%M-%S%P-");
    dateandtime.pop_back(); //to remove \n
    cout << dateandtime << endl;


    fstream log;
    log.open("/media/FishCenS/KINGSTON/cam_log.txt", ios::out | ios::in | ios::app | ios::binary);
    if (!log.is_open()) {
        cout << "something wrong with file" << endl;
        log.close();
        return 0;
    }
    else {
        cout << "file opened" << endl;
        log.seekg(-1, ios::end);
        while (log.peek() == '\n') {
            log.seekg(-1, ios::cur);
        }
        int symcount = 0;
        while(log.peek() == '#') {
            symcount++;
            log.seekg(-1, ios::cur);
        }
        log << '\n' << symcount << " time(s)" << endl << endl << dateandtime << " length = " << ms_length << "ms" << endl;
        log.close();
    }


    for (int i = 1; i <= repeats; i++) {
        fname = dateandtime + to_string(i);
        cout << fname << endl;
        strcmd = "cd /media/FishCenS/KINGSTON && ls";
        strcmd += " && libcamera-vid -t " + to_string(ms_length) + " -o " + fname + "-raw.h264 --save-pts " + fname + "-stamps.txt";
        strcmd += " 2>&1";
        //strcmd += " &>> /media/FishCenS/KINGSTON/" + dateandtime + "_log.txt";
        //strcmd += " && mkvmerge -o " + fname + "-vid.mkv --timecodes 0:" + fname + "-stamps.txt " + fname + "-raw.h264";
        //-cmd code adapted from example code in libcamera-vid section at https://www.raspberrypi.com/documentation/computers/camera_software.html
        cout << strcmd << endl;
        res = exec(strcmd.c_str());
        cout << res << endl;

        log.open("/media/FishCenS/KINGSTON/" + dateandtime + "-log.txt", ios::out | ios::app | ios::binary);
        if (!log.is_open()) {
            cout << "something wrong with file" << endl;
            log.close();
            return 0;
        }
        else {
            log << res << endl;;
            log.close();
        }

        log.open("/media/FishCenS/KINGSTON/cam_log.txt", ios::out | ios::app | ios::binary);
        if (!log.is_open()) {
            cout << "something wrong with file" << endl;
            log.close();
            return 0;
        }
        else {
            log << '#';
            log.close();
        }
        //cout << strcmd << endl;
    }

}
