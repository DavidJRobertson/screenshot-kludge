/* Dropbox Screenshot Tool Thingy
 * ==============================
 * David Robertson - http://davidr.me/
 *
 * Dependencies:
 *   - ImageMagick
 *   - Magick++
 *   - xclip (on linux - on a mac, be sure to change the COPY part of the settings below)
 *   - xdotool (required for taking screenshots of the active window only)
 *   - notify-send (for on-screen notification that a screenshot has just been taken)
 *
 * Compilation & installation:
 *   First, adjust the settings in the source below.
 *   Compile using the following command:
 *     g++ -O2 -o dbss dbss.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`
 *   Next, you probably want to copy the output executable to a directory which is in your path (varies between systems)
 *   Now, set up keyboard shortcuts to trigger the program
 *     - systems which use Gnome (most linux), find 'keyboard shortcuts' in system preferences, add a new shortcut
 *       for each of the commands listed below in the Usage section, and assign a key combination of your choice.
 *
 * Usage:
 *   Run with no arguments to capture the whole screen (`dbss`)
 *   Run with the argument "window" to capture the active window (`dbss window`)
 *   Run with the argument "selection" to make a manual selection of the area to be captured (`dbss selection`)
 *
*/

// Settings:
// =========
// Enclose all of these in double quotes (")
//
// Path to your Dropbox (make sure to add a trailing slash)
#define DROPBOX_PATH "/home/david/Dropbox/"
// Where to put the images - e.g. if this is screenshots, then the images will be put in /path/to/your/dropbox/Public/screenshots
// This directory MUST already exist (Again, add a trailing slash)
#define SCREENSHOT_DIR "screenshots/"
// Your Dropbox user ID
// find this by putting a file in your dropbox Public directory, and getting its public link. The numeric part is what you want.
#define DROPBOX_USER_ID "70152"
// Command to use to copy link to clipboard
// On linux you probably want "xclip -selection c", on a mac "pbcopy".
#define COPY "xclip -selection c"

// Done with the settings

#include <Magick++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <time.h>

#include <iomanip>

using namespace std;
using namespace Magick;

Image takeScreenshot(const char type);


int main( int argc, char* argv[]) {
  unsigned long int sec = time(NULL);
  stringstream out;
  string strtime;
  //sec = sec - 1340000000;
  //out << hex << sec;
  out << sec;
  strtime = out.str();

  string filename = DROPBOX_PATH;
  filename += "Public/";
  filename += SCREENSHOT_DIR;
  filename += strtime;
  filename += ".png";

  Image screenshot;
  if (argc == 2) {
    if (strcmp(argv[1], "selection") == 0) {
      screenshot = takeScreenshot('s');
    } else if (strcmp(argv[1], "window") == 0) {
      screenshot = takeScreenshot('w');
    } else {
      cout << "Invalid arguments." << endl;
      return 0;
    }
  } else {
    screenshot = takeScreenshot('r');
  }

  screenshot.write(filename);

  string shareurl = "http://ss.davidr.me/";
  //shareurl += DROPBOX_USER_ID;
  //shareurl += "/";
  //shareurl += SCREENSHOT_DIR;
  shareurl += strtime;
  //shareurl += ".png";
  cout << shareurl << endl;

  //system("notify-send \"Uploading screenshot to dropbox... URL copied to clipboard.\"");

  string copycmd = "echo \"";
  copycmd += shareurl;
  copycmd += "\" | ";
  copycmd += COPY;

  char *buftwo = new char[copycmd.size()];
  strcpy(buftwo,copycmd.c_str());
  system(buftwo);
  return 0;
}

Image takeScreenshot(const char type) {
  Image screenshot;
  switch (type){
    case 's':
      //User Selection
      screenshot.read("x:");
      break;
    case 'r':
      //Full screen
      screenshot.read("x:root");
      break;
    case 'w':
      //Current Window
      system("xdotool getactivewindow > /tmp/dbssactivewindow.tmp");

      ifstream fh;
      fh.open("/tmp/dbssactivewindow.tmp");
      string wid;
      if(fh.is_open()){
        while(!fh.eof()){
          fh >> wid;
        }
      } else {
        cout << "ERROR!" << endl;
      }
      fh.close();
      screenshot.read("x:" + wid);
      break;
  }
  return screenshot;
}
