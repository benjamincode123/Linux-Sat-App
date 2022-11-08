#include "fcntl.h"
#include "unistd.h"
#include "iostream"

    struct gpgga
    {
         //uoa = units of altitude
         //type of gps msg, 
         //time = utc time,
         //denotes the checksum
         //lat = latitude,
         //N = north, 
         //lng = longitude,
         //W = west
         //quality of signal 1-2 4-5
         //sta_num = numbrs of satelites,
         //HDOP = horizontal dilution of perception,
         //aoa = altitude of antenna
         //dgs = denotes geoidal seperation 
         //aoc = age of correction
         //dcsID = denotes correction station ID
        std::string type, time, dc, lat, N, lng, W, quality, sat_num,HDOP, aoa, dgs, uoa, aoc, dcsID;
    } gpgga_; 

void rawgpsdatafilter(char data [79], gpgga *msgtype) {
    std::string srchchar = ",";
    std::string newstr;
    int index = 0;
    float lat,lng;
    /*for each index in a gpgga msg it separates them by , and 
    stores the content in the struct structure. it does this based 
    on the msgtype referd to in the functions argument*/
    for (int i=0;i<79;i++) {
        std::string currentletter;
        currentletter = data[i];
        if (currentletter == ",") {
            switch (index)
            {
            case 0:
                msgtype->type = newstr;
                break;
            case 1:
                msgtype->time = newstr;
                break;
            case 2: 
            if (newstr.length() > 0) {
                lat = std::stof(newstr.substr(0,2)) + (std::stof(newstr.substr(2,9)) / 60);
                msgtype->lat = std::to_string(lat);
                msgtype->lat.reserve(10);
            } else {
                msgtype->lat = "No Signal";
            }
                break;
            case 3:
                msgtype->N = newstr;
                break;
            case 4:
            if (newstr.length() > 0) {
                lng = std::stof(newstr.substr(0,3)) + (std::stof(newstr.substr(3,10)) / 60);
                msgtype->lng = std::to_string(lng);
                msgtype->lng.reserve(10);
            } else {
                msgtype->lng = "No Signal";
            }
                break;
            case 5:
                msgtype->W = newstr;
                break;
            case 6:
                msgtype->quality = newstr;
                break;  
            case 7:
                msgtype->sat_num = newstr;
                break;
            case 8:
                msgtype->HDOP = newstr;
                break;
            case 9:
                msgtype->aoa = newstr;
                break;
            case 10:
                msgtype->uoa = newstr;
                break;
            case 11:
                msgtype->dgs = newstr;
                break;
            case 12:
                msgtype->aoc = newstr;
                break;
            case 13:
                msgtype->dcsID = newstr;
                break;
            default:
                break;
            }
            index++;
            newstr = "";
        } else if (index == 15) {
            break;
        } else {
            newstr += currentletter;
        }
    }
}


void readgps() {
    int openres, readdata; //output of opening filestream at "/dev/ttyUSB0", and readata response of attempting to read the data inside openres.
    const char* fpath = "/dev/ttyUSB0";
    char buffer [79];//numbers of chars possible in array
    openres = open(fpath, O_RDONLY, O_NONBLOCK);//opens file for reading only to secure no data can be damaged


    if (openres < 0) {
        std::cout << "failed to execute" << std::endl;
    } else {
        //std::cout << openres << std::endl;
    }

    /*reads file and stores the newest 79 chars in buffer
    and closes openres because then it will search after 
    new data each time it runs open() function*/
    readdata = read(openres, buffer, sizeof(buffer));
    openres = close(openres);

    if (readdata < 0) {
        std::cout << "problems reading file " << fpath << std::endl;
    } else {
        std::string gpsdatastr = buffer;
        std::string str;
        str = buffer[0];
        //checks each letter afterthe start of a msg all gps msg starts with "$"
        if (str == "$") {// i == 0 to solve unreliableity issue that iccures 1/5 times whitout this
            if (gpsdatastr.substr(0+1, gpsdatastr.find(",")-1) == "GPGGA") {
                rawgpsdatafilter(buffer, &gpgga_);
            }
        }
    }
}

int main() {
    for (int i = 0; i<99999999; i++) {

        readgps();
        /*this part is unnessarsary its only for user expreience->*/
        system("clear");
        std::cout << " ________________________________________" << std::endl;
        std::cout << "| Latitude " << " | " << gpgga_.lat << " |" << "     _____      |" << std::flush << std::endl;
        std::cout << "| Longitude" << " | " << gpgga_.lng << " |" << "    /     \\     |" << std::flush << std::endl;
        std::cout << "| Time     " << " | " << gpgga_.time<<"|" << "   | Linux |    |" << std::flush << std::endl;
        std::cout << "| Quality  " << " | " << gpgga_.quality << "         |" << "O--|  Sat  |--O |" << std::flush << std::endl;
        std::cout << "| M/F      " << " | " << gpgga_.uoa << "         |" << "   |  App  |    |" << std::flush << std::endl;
        std::cout << "| Type     " << " | " << gpgga_.type << "    |" << "    \\_____/     |" << std::flush << std::endl;
        std::cout << " ----------------------------------------" << std::flush << std::endl;
        /*this part is unnessarsary its only for user expreience->*/
    }
    return 0;
}