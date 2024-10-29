#include "iostream"
#include "cli.h"
#include "vector"
#include "util.h"

#define EOL "\n"
#define INDENT "\t"

using namespace std;

CommandHandler CLI::help() {
    return [this] (Arguments* args) {
        cout << "Available commands: " << EOL;
        cout << INDENT << "help - Prints the available commands" << EOL;
        cout << INDENT << "quit - Exits the application" << EOL;
        cout << INDENT << "connect <addr> [port] - Connects the FTP server" << EOL;
        cout << "Can be used after connected to server:" << EOL;
        cout << INDENT << "login <username> <password> - Authenticates to the ftp server" << EOL;
        cout << INDENT << "cd <path> - Navigates to the given directory" << EOL;
        cout << INDENT << "ls - Lists the content of current directory" << EOL;
        cout << INDENT << "dw <pathToFileRemote> <pathLocal> - Downloads the given file from the remote path to the given local path." << EOL;
        cout << INDENT << "up <pathLocal> <pathRemote> - Uploads the given file from local path to the given remote path." << EOL;
        cout << INDENT << "pwd - Current remote path." << EOL;
        return 0;
    };
}

CommandHandler CLI::connect() {
    return [this] (Arguments* args) {
        this->ftpClient = new FTPClient(args->at(1), args->at(2), false);
        if (!ftpClient->isConnected()) {
            free(this->ftpClient);
            this->ftpClient = nullptr;
        }
        return 0;
    };
}

CommandHandler CLI::up() {
    return [this] (Arguments* args) {
        if (args->size() == 3) {
            this->ftpClient->uploadFileBackground(args->at(1), args->at(2));
        }

        return 0;
    };
}

CommandHandler CLI::dw() {
    return [this] (Arguments* args) {
        if (args->size() == 3) {
            this->ftpClient->downloadFileBackground(args->at(2), args->at(1));
        }
        return 0;
    };
}

CommandHandler CLI::ls() {
    return [this] (Arguments* args) {
        if(args->size() < 2) {
            this->ftpClient->listDir("");
        } else {
            this->ftpClient->listDir(args->at(1));
        }
        return 0;
    };
}

CommandHandler CLI::cd() {
    return [this] (Arguments* args) {
        if (args->size() < 2) {

        } else {
            this->ftpClient->changeDir(args->at(1));
        }
        return 0;
    };
}

CommandHandler CLI::pwd() {
    return [this] (Arguments* args) {
        this->ftpClient->PWD(false);
        return 0;
    };
}

CommandHandler CLI::login() {
    return [this] (Arguments* args) {
        const int res = this->ftpClient->login(args->at(1), args->at(2));
        if (res == 0) {
            cout << "Login Success!" << EOL;
        } else if (res == -1) {
            cout << "Login failed! Wrong username or password!" << EOL;
        } else {
            cout << "Login failed!" << EOL;
        }
        return res;
    };
}

CommandHandler CLI::quit() {
    return [this] (Arguments* args) {
        this->ftpClient->closeConnections();
        exit(0);
        return 0;
    };
}

void CLI::initCommands() {
        // TOP Lvl commands available all the time
        this->commands["help"] = this->help();
        this->commands["connect"] = this->connect();

        // FTP commands, available only if connected to ftp server
        this->commands["login"] = this->login();
        this->commands["up"] = this->up();
        this->commands["dw"] = this->dw();
        this->commands["ls"] = this->ls();
        this->commands["cd"] = this->cd();
        this->commands["pwd"] = this->pwd();
        this->commands["quit"] = this->quit();
}

void CLI::handleCommands(string command) {
   Arguments args = splitStr(command,' ');

    const auto f = this->commands.find(args.at(0));
    if(f != this->commands.end()) {
        f->second(&args);
    }
}

void CLI::start() {
    std::cout << "Welcome to FTP Client!" << EOL << "To see available commands: help" << EOL;
    while (1) {
           string line;
           getline(cin, line);
           this->handleCommands(line);
    }
}
