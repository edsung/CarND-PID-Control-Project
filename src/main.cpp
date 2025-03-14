#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

// for convenience
using nlohmann::json;
using std::string;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  PID pid;
  /**
   * TODO: Initialize the pid variable.
   */
  int num_loop = -1;
  
  double best_err = 0.0, err =0.0, tol = 0.2;
  bool jump1 = false, jump2 = false;
  int index = 0;
  
  std::vector<double> param, dparam;
  param = {0.04,0.001,1};

  //param = {1,0,0};
  dparam = {1.0,3.0,0.0};
  
  pid.Init(param[0],param[1],param[2]);

  h.onMessage([&pid,&param,&dparam,&num_loop,&best_err,&err,&tol,&jump1,&jump2,&index](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value;
          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */
         
          /*if(num_loop == -1){
            best_err = cte;
            pid.UpdateError(cte);
            steer_value = pid.TotalError();
          }
          
          if(jump1 == true){
            err = cte;
            pid.UpdateError(cte);
            goto eval;
          }
          
          
          if(jump2 == true){
            err = cte;
            pid.UpdateError(cte);
            goto re_eval;
          }
          
          
          if((dparam[0]+dparam[1]+dparam[2]) > tol && num_loop >= 0){
            err = cte;
            index = (num_loop) % 3;
            param[index] += dparam[index];
            pid.UpdateError(cte);
            pid.SetParam(param);
            steer_value = pid.TotalError();
            jump1 = true;
            goto getCTE;
            
            eval:
            if(err < best_err){
              best_err = err;
              dparam[index] *= 1.1;
              jump1 = false;
            }
            else{
              jump1 = false;
              param[index] -= 2 * dparam[index];
              pid.SetParam(param);
              steer_value = pid.TotalError();
              jump2 = true;
              goto getCTE;
              
              re_eval:
              if(err < best_err){
                best_err = err;
                dparam[index] *= 1.1;
                jump2 = false;
              }
              else{
                param[index] += dparam[index];
                dparam[index] *= 0.9;
                jump2 = false;
              }
            }
            
          }
          
          num_loop++;
          std::cout<<"num_loop: "<<num_loop<<", best err: "<<best_err<<", index: "<<index<<", param: ["<<param[0]<<","<<param[1]<<","<<param[2]<<"], dparam :["<<dparam[0]<<","<<dparam[1]<<","<<dparam[2]<<"]"<<std::endl;
          getCTE:*/     
          // DEBUG
          pid.UpdateError(cte);
          steer_value = pid.TotalError();
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value 
                    << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  
  h.run();
}