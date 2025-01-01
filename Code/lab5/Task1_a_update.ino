#include <Wire.h>
#include "Mona_ESP_lib.h"
#include <ESP32Encoder.h> 

ESP32Encoder right_encoder;
ESP32Encoder left_encoder;


int right_new_pos, right_old_pos, left_new_pos, left_old_pos;
float right_vel, left_vel, right_ref_vel, left_ref_vel;


// set sample time
float Ts = 0.01;
float start_time, current_time, elapsed_time;


// Wheel radius is 15mm, every revolute is 3500 pulse
const float robot_radius = 40;
const float wheel_radius = 15;

// The post-gearbox resolution is 3500 pulse per wheel revolution and 

// the radius of wheel is 15mm. In this case, the following scale_encoder

// value should be set so that the measurement results show the linear displacement

// in mm at the Serial Monitor/Plotter

const float scale_encoder = 360.0 / 3500; //rad
const float pi = 3.14159; 

int dead_zone;

float target_right_vel = 0.01; // the target velocity of right wheel (rad/s)
float target_left_vel = 0.1;  // the target velocity of left wheel (rad/s)

float control,err;

void setup(){

  Mona_ESP_init();

  Serial.begin(115200);

  dead_zone = 0;

  //为右轮和左轮的编码器指定对应的引脚，这样编码器可以读取轮子的旋转信息
  right_encoder.attachHalfQuad ( Mot_right_feedback, Mot_right_feedback_2 );
	left_encoder.attachHalfQuad( Mot_left_feedback_2, Mot_left_feedback );


	//清除编码器的计数器，确保从零开始读取数据
	right_encoder.clearCount();
	left_encoder.clearCount();


	// set the lastToggle
	current_time = millis();
  start_time = current_time;
  // start_time = millis();  // 固定 start_time 在 setup 中初始化

  // Serial.print("the start_time is ");
  // Serial.println(start_time); 

  // set the initial positions
  right_old_pos = 0;
  left_old_pos = 0;

  delay(5000);

  //PWM 40%
  // Left_mot_forward(102);
  // Right_mot_forward(102);

  // Left_mot_forward(29);
  // Right_mot_forward(29);

  // //PWM 60%
  // Left_mot_forward(153);
  // Right_mot_forward(153);

  // //PWM 80%
  // Left_mot_forward(204);
  // Right_mot_forward(204);

  // //PWM 100%
  // Left_mot_forward(255);  // 左轮全速
  // Right_mot_forward(255);  // 右轮全速
}


//Task5
float Kp = 0.8;  // Initial proportional gain
float Ki = 0.2; // Initial integral gain
float intError = 0;     // Integral error
const float target_angle = 90;
float right_angle = 0;


void position(){

  elapsed_time = (millis()-current_time)/1000;    // elapsed time in seconds

  right_new_pos = right_encoder.getCount();
  left_new_pos = left_encoder.getCount();

  right_vel = (((right_new_pos - right_old_pos)/elapsed_time)/wheel_radius)*(2*pi/3500);
  left_vel = (((left_new_pos - left_old_pos)/elapsed_time)/wheel_radius)*(2*pi/3500);

  right_angle = right_new_pos * scale_encoder;
  Serial.print(right_new_pos);
  Serial.print(",");
  Serial.print(right_angle);
  Serial.print(",");
  Serial.print(target_angle);
  Serial.print(",");

  //将当前的编码器位置存储为旧位置
  right_old_pos = right_new_pos; 
  left_old_pos = left_new_pos;

}



void PI_control(float ref, float act, float Kp, float Ki) {
  err = ref - act;                      // Calculate error
  intError += err * Ts;                 // Update integral error
  intError = bound(intError, -128, 128); // Bound integral error
  control = Kp * err + Ki * intError;   // Compute control signal
  Serial.print(control);
  Serial.print(",");
  Serial.print(intError);
}

float bound(float x, float x_min, float x_max) {
  if (x < x_min) return x_min;
  if (x > x_max) return x_max;
  return x;
}

// void loop() {
//     Serial.print("Right Encoder Count: ");
//     Serial.println(right_encoder.getCount());
//     delay(500);
// }


void loop(){
  Serial.println("intError,Control,right_angle,target_angle");
  while((current_time-start_time)<10000){
    position();
    PI_control(target_angle, right_angle, Kp, Ki);
    
    if (control > 0) {
        Right_mot_forward(control);
    } else {
        Right_mot_backward(control);
    }

    // Right_mot_forward(100); // 固定输出100的PWM信号

    current_time = millis();
    delay(Ts*1000);
    Serial.println();
  }
  // Serial.print("out of floop");
  Serial.println();
  Motors_stop();
  // if((current_time-start_time)>=6000){
  //   Serial.end();
  // }
  Serial.end();
}







//Task4
// void Propotional(float ref, float act, float Kp){
//   err = (ref-act);
//   control = Kp*err;
// }

// void loop(){
//   // current_time = millis(); 
//   Serial.print(current_time-start_time);
//   while((current_time-start_time)<7000){
//     position();
//     Propotional(target_right_vel, right_vel, 200);
//     Right_mot_forward(control+44);
//     Propotional(target_left_vel, left_vel, 200);
//     Left_mot_forward(control+37);
//     Serial.print(right_vel);
//     Serial.print("\t");
//     Serial.print(left_vel);
//     Serial.println();
//     current_time = millis();
//     delay(Ts*1000);
//   }
//   // Serial.print("out of floop");
//   Serial.println();
//   Motors_stop();
//   // if((current_time-start_time)>=6000){
//   //   Serial.end();
//   // }
//   Serial.end();
// }


//Task3
// void loop(){
//   position();
//   if (right_vel<=0){
//     dead_zone = dead_zone + 1;
//     Right_mot_forward(dead_zone);
//   }
//   else{
//     Motors_stop();
//     Serial.end();
//     delay(3000);
//   }
//   Serial.print(dead_zone);
//   Serial.println();
//   current_time = millis();
//   delay(Ts*100);
// }



// void loop(){
//   //在10秒内，反复执行循环体内容

//   while (current_time-start_time<10000){
//     //每次循环更新左右编码器的位置信息
//     position();
//     //将编码器的脉冲计数（乘以刻度比例）输出到串行监视器
//     Serial.print("right_old_pos:");
//     Serial.print(scale_encoder*right_old_pos);
//     Serial.print("\t");
//     Serial.print("left_old_pos:");
//     Serial.print(scale_encoder*left_old_pos);
//     Serial.println();
//     Serial.print(right_wheel_data);
//     Serial.print("\t");  
//     Serial.println(left_wheel_data);


//     current_time = millis();
//     // Serial.print("the current_time is ");
//     // Serial.println(current_time);  // 输出 current_time 并换行
//     //设置采样时间间隔为 0.01*1000=10ms
//     delay(Ts*1000);
//   }


//   //停止左右电机
//   Motors_stop();
//   //结束串行通信
//   Serial.end();
// }