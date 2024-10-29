#ifndef LPROG_HF_2022O_THEDOCTOR028_CLI_H
#define LPROG_HF_2022O_THEDOCTOR028_CLI_H
#include <string>
#include <map>
#include "vector"
#include "client.h"
using namespace std;

typedef vector<string> Arguments;
typedef function<int(Arguments*)> CommandHandler;
//using CommandHandler = int (Arguments*);

class CLI {

public:
    void start();

    CLI() {
        initCommands();
    }

protected:
    map<string,  CommandHandler> commands;
    void handleCommands(string command);
    void initCommands();

    FTPClient* ftpClient;
private:
    CommandHandler connect();
    CommandHandler help();
    CommandHandler ls();
    CommandHandler cd();
    CommandHandler up();
    CommandHandler dw();
    CommandHandler login();
    CommandHandler pwd();
    CommandHandler quit();
};

#endif //LPROG_HF_2022O_THEDOCTOR028_CLI_H
