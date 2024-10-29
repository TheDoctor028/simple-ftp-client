#include <fstream>
#include "client.h"
#include "unistd.h"
#include "util.h"
#include <pthread.h>


int calculatePort(std::string str) {
    const auto posStart = str.find('(') + 1;
    const auto posEnd = str.find(')');
    const auto nums = str.substr(posStart, posEnd - posStart);
    const auto port = splitStr(nums, ',');
    return std::atoi(port.at(4).c_str())*256 + std::atoi(port.at(5).c_str());
}

int calculateFileSize(std::string str) {
    const auto posStart = str.find('(') + 1;
    const auto posEnd = str.find(')');
    const auto nums = str.substr(posStart, posEnd - posStart);
    const auto sizeInB = nums.substr(0, nums.find(' '));
    return std::atoi(sizeInB.c_str());
}

std::string getPath(std::string str) {
    const auto posStart = str.find('\"') + 1;
    const auto posEnd = str.find('\"', posStart);
    return str.substr(posStart, posEnd - posStart);
}

char* FTPClient::receiveMsg(int len) {
    memset(this->reply, 0, BUFSIZ);
    recv(this->controlSocket, this->reply, len, 0);
    return this->reply;
}

char* FTPClient::receiveMsgDTP(int len) {
    memset(this->replyDTP, 0, BUFSIZ);
    recv(this->DTPSocket, this->replyDTP, len, 0);
    return this->replyDTP;
}

int FTPClient::createSocket() {
    const int desc = socket(AF_INET, SOCK_STREAM, 0);

    if (desc < 0) {
        perror("Could not create socket");
    }

    return desc;
}

int FTPClient::bindSocket(int socket, int p) const {
    struct sockaddr_in server{};

    server.sin_addr.s_addr = inet_addr(this->serverAddr.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(p);
    const int conn = connect(socket, (struct sockaddr*)&server, sizeof(server));

    return conn;
}

void FTPClient::sendMsg(std::string msg) {
    write(this->controlSocket, msg.c_str(), msg.length());
}

int FTPClient::login(const std::string& user, const std::string& password) {
    std::string command;
    this->sendMsg(command.append("USER ").append(user).append("\n"));
    std::string reply = this->receiveMsg(3);
    if (reply.erase(3, std::string::npos) == "331") {
        this->receiveMsg(BUFSIZ);
        command.clear();
        this->sendMsg(command.append("PASS ").append(password).append("\n"));
        reply = this->receiveMsg(3);
        if (reply.erase(3, std::string::npos) != "230") {
            this->receiveMsg(BUFSIZ);
            return -1;
        }
    } else if (reply.erase(3, std::string::npos) != "230") {
        this->receiveMsg(BUFSIZ);
       return -1;
    }

    this->user = user;
    this->password = password;
    this->receiveMsg(BUFSIZ);
    return 0;
}

int FTPClient::listDir(const std::string &path) {
    std::string command;
    this->openDTPConnection();

    this->sendMsg(command.append("LIST ").append(path).append("\n"));
    this->receiveMsg(BUFSIZ);
    std::cout << this->receiveMsgDTP();
    close(this->DTPSocket);
    this->receiveMsg(BUFSIZ);

    return 0;
}

int FTPClient::changeDir(const std::string &path) {
    std::string command;
    this->sendMsg(command.append("CWD ").append(path).append("\n"));

    if (!this->silent) std::cout << this->receiveMsg(100);
    return 0;
}

void FTPClient::openDTPConnection() {
    this->sendMsg("PASV\n");
    const int port =  calculatePort(this->receiveMsg(100));
    this->DTPSocket = this->createSocket();

    if(this->bindSocket(this->DTPSocket, port) < 0) {
        perror("Cant open DTP connection");
    }
}

int FTPClient::uploadFile(const std::string &localPath, const std::string &remotePath) {
    std::string command;

    this->openDTPConnection();
    this->sendMsg(command.append("STOR ").append(remotePath).append("\n"));
    std::ifstream fstream;
    fstream.open(localPath, std::ios::in | std::ios::binary);

    std::string line;
    while (std::getline(fstream, line)) {
        std::istringstream iss(line);
        if(!(iss)) { break; }
        write(this->DTPSocket, line.append("\n").c_str(), line.length());
    }

    write(this->DTPSocket, "\0", 1);

    this->receiveMsg(BUFSIZ);
    fstream.close();
    close(this->DTPSocket);
    std::cout << "Upload: " <<  remotePath << " Transfer complete!\n";

    return 0;
}

int FTPClient::downloadFile(const std::string& localPath, const std::string& remotePath) {
    std::fstream fstream;
    fstream.open(localPath, std::ios::out | std::ios::binary);

    this->openDTPConnection();

    std::string command;
    this->sendMsg(command.append("RETR ").append(remotePath).append("\n"));

    auto fSize = calculateFileSize(this->receiveMsg(BUFSIZ));

    while (fSize > 0) {
        int s;
        if (fSize <= BUFSIZ) {
            s = fSize;
            fSize = 0;
        } else {
            s = BUFSIZ;
            fSize = fSize - BUFSIZ;
        }
        fstream.write(this->receiveMsgDTP(s), s);
    }

    fstream.close();
    close(this->DTPSocket);
    std::cout << "Download: " << remotePath << " Transfer complete!\n";
    return 0;
}

int FTPClient::PWD(bool silent) {
    std::string command;
    this->sendMsg(command.append("PWD\n"));
    this->pwd = getPath(this->receiveMsg(BUFSIZ));
    if (!silent) std::cout << this->pwd << "\n";
    return 0;
}

void* downloadFileT(void *args) {
    auto opts = (Opts*) args;
    opts->ftpClient->downloadFile(opts->localPath, opts->remotePath);
    opts->ftpClient->closeConnections();
    return nullptr;
}

int FTPClient::downloadFileBackground(const std::string& localPath, const std::string& remotePath) {
    pthread_t thread1;
    auto ftpC = new FTPClient(this->serverAddr, this->serverPort);
    ftpC->login(this->user, this->password);

    this->PWD();
    Opts* payload = new Opts;
    payload->ftpClient = ftpC;
    payload->remotePath = this->pwd.append("/").append(remotePath);;
    payload->localPath = localPath;

    std::cout << "Started downloading file: " << remotePath << "\n";
    pthread_create(&thread1, nullptr, &downloadFileT, payload);
    return 0;
}

void* uploadFileT(void *args) {
    auto opts = (Opts*) args;
    opts->ftpClient->uploadFile(opts->localPath, opts->remotePath);
    opts->ftpClient->closeConnections();
    return nullptr;
}

int FTPClient::uploadFileBackground(const std::string &localPath, const std::string &remotePath) {
    pthread_t thread1;
    auto ftpC = new FTPClient(this->serverAddr, this->serverPort);
    ftpC->login(this->user, this->password);

    this->PWD();
    Opts* payload = new Opts;
    payload->ftpClient = ftpC;
    payload->remotePath = this->pwd.append("/").append(remotePath);
    payload->localPath = localPath;

    std::cout << "Started uploading file: " << localPath << "\n";
    pthread_create(&thread1, nullptr, &uploadFileT, payload);
    return 0;
}

int FTPClient::closeConnections() {
    close(this->controlSocket);
    return 0;
}
