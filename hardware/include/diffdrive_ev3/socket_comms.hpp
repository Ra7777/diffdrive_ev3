#ifndef DIFFDRIVE_EV3_ARDUINO_COMMS_HPP
#define DIFFDRIVE_EV3_ARDUINO_COMMS_HPP

// #include <cstring>
#include <sstream>
// #include <cstdlib>
#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>


class SocketComms
{

public:

  SocketComms() = default;

  void connectS(const char *addr, int32_t port, int32_t timeout_ms)
  {  
    std::cerr << addr << ' ' << port << ' ' << timeout_ms << std::endl;
    timeout_ms_ = timeout_ms;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(addr);
    int err = connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    error = err;
    char err_buffer[ 256 ];
    char * errorMsg = strerror_r( errno, err_buffer, 256 );
    std::cerr << "connection " << errorMsg << std::endl;  
  }

  void disconnect()
  {
    close(client_socket);
  }

  bool connected() const
  {
    return error >= 0;
  }


  std::string send_msg(const std::string &msg_to_send, bool print_output = true)
  {
    send(client_socket, msg_to_send.data(), msg_to_send.size(), 0);

    std::string response;
    char buf[30];
    //try
    //{
      // Responses end with \r\n so we will read up to (and including) the \n.
      //serial_conn_.ReadLine(response, '\n', timeout_ms_);
      recv(client_socket, buf, sizeof(buf), 0);
      std::cerr << buf << std::endl; 
      response.append(buf);
    //}
    //catch (const LibSerial::ReadTimeout&)
    //{
    //    std::cerr << "The ReadByte() call has timed out." << std::endl ;
    //}

    if (print_output)
    {
      std::cerr << "Sent: " << msg_to_send << " Recv: " << response << std::endl;
    }

    return response;
  }


  void send_empty_msg()
  {
    std::string response = send_msg("");
  }

  void read_encoder_values(int &val_1, int &val_2)
  {
    std::string response = send_msg("e                             ");

    std::string delimiter = " ";
    size_t del_pos = response.find(delimiter);
    std::string token_1 = response.substr(0, del_pos);
    std::string token_2 = response.substr(del_pos + delimiter.length());

    val_1 = std::atoi(token_1.c_str());
    val_2 = std::atoi(token_2.c_str());
  }
  void set_motor_values(double val_1, double val_2)
  {
    int spaces = std::to_string(val_1).length() + std::to_string(val_2).length();
    std::cerr << "spaces: " << spaces << std::endl;
    std::stringstream ss;
    ss << "m " << val_1 << ' ' << val_2 << std::string(27-spaces, ' ');
    send_msg(ss.str());
  }

  void set_pid_values(int k_p, int k_d, int k_i, int k_o)
  {
    std::stringstream ss;
    ss << "u " << k_p << ":" << k_d << ":" << k_i << ":" << k_o << "\r";
    send_msg(ss.str());
  }

private:
    int client_socket;
    struct sockaddr_in server_address;
    int timeout_ms_;
    int error = 0;
};

#endif // DIFFDRIVE_EV3_ARDUINO_COMMS_HPP