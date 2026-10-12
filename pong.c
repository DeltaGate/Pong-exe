#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>


#pragma comment(lib, "Ws2_32.lib")


    WSADATA wsaData;
    int iResult;


int main(){

    // initalise Ws2 DLL and winsock2 lib, check for errors

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult !=0) {
    printf("WSAStartup failed: %d\n", iResult);
    return 1;
} else {
    printf("WSAStartup Successful\n");
}

    // create 3 addrinfo strcuts - or 2 pointers and 1 full, these hold adress infomation for connection
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family     = AF_INET; // defines if connection accepts IPv4 or IPv6 - I chose unspec so it can be either
    hints.ai_socktype   = SOCK_DGRAM; // Telling it to use UDP socket
    hints.ai_protocol   = IPPROTO_UDP; // Telling it to use UDP protocol
    hints.ai_flags    = AI_PASSIVE; // listening program

    printf("Adress Objects Created\n");

    #define DEFAULT_PORT "27015"

    // request address info from user using default port assgined above

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult !=0){
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    printf("Generating Socket...\n");
    // generate a Socket
    SOCKET ConnectSocket = INVALID_SOCKET;

    ptr = result;

    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);


    // checking socket is valid
    if (ConnectSocket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
    }

    // bind to port so I can listen for packets.
    iResult = bind(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
    printf("Failed to bind to socket: %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
    }

    printf("Successfully Binded\n");

    // setting varibles for reciving ping packet
    char recvBuffer[4];
    int bufferLength = 4;
    struct sockaddr_in whoFrom;
    int senderAddrSize = sizeof (whoFrom);

    // Recieve a PING packet via UDP through assgined socket
    iResult = recvfrom(ConnectSocket, recvBuffer, bufferLength, 0, (SOCKADDR *)&whoFrom, &senderAddrSize);
    if(iResult == SOCKET_ERROR){
        printf("Packet Reciving Failure %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    printf("Received from External: %s\n", recvBuffer);

    //setting varibles for Pong Packet Sending
    char sendBuffer[] = "PONG";


    // now send a PONG back using the address that I receved the PING from and address size recieved
    iResult = sendto(ConnectSocket, sendBuffer, bufferLength, 0, (SOCKADDR *)&whoFrom, senderAddrSize);
    if(iResult == SOCKET_ERROR){
        printf("Packet Sending Failure %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    printf("Bytes sent: %d\n", iResult);
    printf("Pong Sent\n");


    //clean up in required order ofc - my comments getting more causal the longer I go lol
    freeaddrinfo(result);
    closesocket(ConnectSocket);
    WSACleanup();

    printf("Process fully complete\n");
    return 0;
}