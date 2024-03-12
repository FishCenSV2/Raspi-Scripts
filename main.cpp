#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <fstream>

using namespace std;

const int repeats = 100;
const int ms_length = 900000; //60000 = 1 minute

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
        cout << "BASH SUCCESS\n";
    }
    else {
        cout << "BASH FAILED\n";
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
        cout << "something wrong with cam_log file" << endl;
        log.close();
        return 0;
    }
    else {
        cout << "cam_log file opened" << endl;
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

    log.open("/media/FishCenS/KINGSTON/111_OFF_BUTTON.txt", ios::trunc | ios::out);
    if(!log.is_open()) {
        cout << "something wrong with off button" << endl;
        log.close();
        return 0;
    }
    else {
        cout << "off button created" << endl;
        log << "running";
        log.close();
    }

    string gpioSetup = "";
    gpioSetup += "echo \"27\" > /sys/class/gpio/export && "; //enable GPIO27 (YLW LED)
    gpioSetup += "echo \"17\" > /sys/class/gpio/export && "; //enable GPIO17 (RED LED)
    gpioSetup += "echo \"out\" > /sys/class/gpio/gpio27/direction && "; //set GPIO27 as output
    gpioSetup += "echo \"out\" > /sys/class/gpio/gpio17/direction"; //set GPIO17 as output
    res = exec(gpioSetup.c_str());

    const string YLW_ON = "echo \"1\" > /sys/class/gpio/gpio27/value";
    const string YLW_OFF = "echo \"0\" > /sys/class/gpio/gpio27/value";
    const string RED_ON = "echo \"1\" > /sys/class/gpio/gpio17/value";
    const string RED_OFF = "echo \"0\" > /sys/class/gpio/gpio17/value";

    bool running, exitbutton = false, redstate = false;

    for (int i = 1; i <= repeats; i++) {
        res = exec(YLW_ON.c_str());
        running = false;
        while (!running) {
            res = exec(redstate ? RED_ON.c_str() : RED_OFF.c_str());
            redstate = !redstate;
            fname = dateandtime + to_string(i);
            cout << fname << endl;
            strcmd = "cd /media/FishCenS/KINGSTON && ls"; //move to directory of USB drive
            strcmd += " && libcamera-vid -t " + to_string(ms_length) + " --framerate 60 -o " + fname + "-raw.h264 --save-pts " + fname + "-stamps.txt";
                //save video (plus timestamps for later conversion to .mp4)
            strcmd += " 2>&1"; //save stdout and stderr of bash command for res variable
            //strcmd += " && mkvmerge -o " + fname + "-vid.mkv --timecodes 0:" + fname + "-stamps.txt " + fname + "-raw.h264"; //conversion to .mp4
                //excluded due to time loss
            //-cmd code adapted from example code in libcamera-vid section at https://www.raspberrypi.com/documentation/computers/camera_software.html
            cout << strcmd << endl;
            res = exec(strcmd.c_str());
            cout << res << endl;
            running = (res.find("failed") == std::string::npos); //check if something went wrong
            if (!running) { //if something went wrong, put up error lights
                res = exec(RED_ON.c_str());
                res = exec(YLW_OFF.c_str());
                redstate = true;
            }
            log.open("/media/FishCenS/KINGSTON/111_OFF_BUTTON.txt", ios::in); //check off button
            if (!log.is_open()) { //off button deleted
                cout << "OFF BUTTON DISCONNECTED";
                log.close();
                exitbutton = true;
                break;
            }
            else {
                string check;
                log >> check;
                cout << check << endl;
                log.close();
                if (check.find("running") == std::string::npos) { //off button activated
                    cout << "OFF BUTTON ACTIVATED";
                    exitbutton = true;
                    break;
                    }
            }
        }
        if (exitbutton)
            break;
        log.open("/media/FishCenS/KINGSTON/" + dateandtime + "-log.txt", ios::out | ios::app | ios::binary);
        if (!log.is_open()) {
            cout << "something wrong with current log file" << endl;
            log.close();
            return 0;
        }
        else {
            log << res << endl;;
            log.close();
        }

        log.open("/media/FishCenS/KINGSTON/cam_log.txt", ios::out | ios::app | ios::binary);
        if (!log.is_open()) {
            cout << "something wrong with cam_log file" << endl;
            log.close();
            return 0;
        }
        else {
            log << '#';
            log.close();
        }
        //cout << strcmd << endl;
    }

    gpioSetup = "";
    gpioSetup += "echo \"0\" > /sys/class/gpio/gpio27/value && "; //turn off GPIO27
    gpioSetup += "echo \"0\" > /sys/class/gpio/gpio17/value && "; //turn off GPIO17
    gpioSetup += "echo \"27\" > /sys/class/gpio/unexport && "; //disable GPIO27
    gpioSetup += "echo \"17\" > /sys/class/gpio/unexport"; //disable GPIO17
    res = exec(gpioSetup.c_str());

}
