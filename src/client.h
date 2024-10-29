#include <string>
#include <utility>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <iostream>
#include <map>

#ifndef LPROG_HF_2022O_THEDOCTOR028_CLIENT_H
#define LPROG_HF_2022O_THEDOCTOR028_CLIENT_H

class FTPClient {
public:
    FTPClient(std::string serverAddr, std::string serverPort, bool silent = true) {
        this->silent = silent;

        this->serverAddr = std::move(serverAddr);
        this->serverPort = std::move(serverPort);

        this->controlSocket = createSocket();

        const int controlS = this->bindSocket(this->controlSocket, std::stoi(this->serverPort));
        if (controlS < 0) {
            perror("Cannot connect to server!\n");
        }

        if (std::strcmp( this->receiveMsg(), "220") == 0) {
            if (!silent) std::cout << "Connected!\n";
            this->receiveMsg(BUFSIZ);
        } else {
            controlSocket = -1;
        }
    }

    std::string serverAddr;
    std::string serverPort;

    void sendMsg(std::string msg);
    char* receiveMsg(int len = 3);
    char* receiveMsgDTP(int len = BUFSIZ);

    int login(const std::string& user, const std::string& password);
    int listDir(const std::string& path = "");
    int changeDir(const std::string& path);
    int uploadFile(const std::string& localPath, const std::string& remotePath);
    int uploadFileBackground(const std::string& localPath, const std::string& remotePath);
    int downloadFile(const std::string& localPath, const std::string& remotePath);
    int downloadFileBackground(const std::string& localPath, const std::string& remotePath);
    int closeConnections();
    int PWD(bool silent = true);

    bool isConnected() const {
        return this->controlSocket > 0;
    }


protected:
    int controlSocket;
    int DTPSocket;
    bool silent = true;
    void openDTPConnection();

    char reply[BUFSIZ];
    char replyDTP[BUFSIZ];

    std::string user;
    std::string password;
    std::string pwd;

    int createSocket();
    int bindSocket(int socket, int p) const;
};

struct Opts {
    FTPClient *ftpClient;
    std::string remotePath;
    std::string localPath;
};

#endif //LPROG_HF_2022O_THEDOCTOR028_CLIENT_H
