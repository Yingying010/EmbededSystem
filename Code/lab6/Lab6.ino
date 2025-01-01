#include <Wire.h>
#include "Mona_ESP_lib.h"
#include <ESP32Encoder.h> 

ESP32Encoder right_encoder;
ESP32Encoder left_encoder;


float lin_vel; 
float ang_vel;
float w_r; 
float w_l; 

const float pi = 3.14159;
float Kp = 0.8;
float Ki = 0.1; 

int right_new_pos, right_old_pos, left_new_pos, left_old_pos;
float right_vel, left_vel, right_ref_vel, left_ref_vel, w_r_act, w_l_act;
float intError = 0;     // Integral error
float control,err;
float start_time, current_time, elapsed_time;
float Ts = 0.01;
const float wheel_radius = 30;






void setup(){

  Mona_ESP_init();

  Serial.begin(115200);


  right_encoder.attachHalfQuad ( Mot_right_feedback, Mot_right_feedback_2 );
	left_encoder.attachHalfQuad( Mot_left_feedback_2, Mot_left_feedback );


	right_encoder.clearCount();
	left_encoder.clearCount();

  right_old_pos = 0;
  left_old_pos = 0;


  delay(5000);


}




void Propotional(float ref, float act, float Kp){
  err = (ref-act);
  control = Kp*err;
}

void PI_control(float ref, float act, float Kp, float Ki) {
  err = ref - act;   
  intError += err * Ts; 
  intError = bound(intError, -128, 128);
  control = Kp * err + Ki * intError;
}

float bound(float x, float x_min, float x_max) {
  if (x < x_min) return x_min;
  if (x > x_max) return x_max;
  return x;
}





void inverseKinematics(float lin_vel, float ang_vel, float&w_r, float&w_l){
    float L = 80.0;
    float R = 30.0;
    w_r = (lin_vel + (ang_vel * L / 2)) / R;
    w_l = (lin_vel - (ang_vel * L / 2)) / R;
}



float control_speed(float ref_speed, float act_speed, float Kp, float Ki, bool use_PI) {
    if (use_PI) {
        PI_control(ref_speed, act_speed, Kp, Ki);
        return control;
    } else {
        Propotional(ref_speed, act_speed, Kp);
        return control;
    }
}


const float scale_encoder = 0.0269279;  
void position(){

  elapsed_time = (millis()-current_time)/1000; 

  right_new_pos = right_encoder.getCount();
  left_new_pos = left_encoder.getCount();

  right_vel = (((right_new_pos - right_old_pos)/elapsed_time)/wheel_radius)*(2*pi/3500);
  left_vel = (((left_new_pos - left_old_pos)/elapsed_time)/wheel_radius)*(2*pi/3500);

  Serial.print(right_new_pos);
  Serial.print("\t");
  Serial.print(left_new_pos);
  Serial.println();

  right_old_pos = right_new_pos; 
  left_old_pos = left_new_pos;

}


void Move(float lin_vel, float ang_vel, float t, float Kp, float Ki, float deadZone_r, float deadZone_l, float wheelRadius, float wheelbase) {
    unsigned long start_time = millis();
    unsigned long elapsed_time = 0;


    while (elapsed_time < t * 1000) { 

      inverseKinematics(lin_vel, ang_vel, w_r, w_l);

      float w_r_ref = w_r;
      float w_l_ref = w_l;

      position();

      w_r_act = right_vel;
      w_l_act = left_vel;

      // float correction = 0;
      // if (ang_vel == 0) {
      //   int encoder_diff = right_encoder.getCount() - left_encoder.getCount();
      //   float correction_gain = 0.7;
      //   correction = encoder_diff * correction_gain;
      // }

      float control_r = control_speed(w_r, w_r_act, Kp, Ki, true);
      float control_l = control_speed(w_l, w_l_act, Kp, Ki, true);

      if (ang_vel == 0) {
        int encoder_diff = right_encoder.getCount() - left_encoder.getCount();
        float correction_gain = 0.5;
        float correction = encoder_diff * correction_gain;
        control_r -= correction;
        control_l += correction;
      }

      if(control_r >= 0){
        Right_mot_forward(control_r + deadZone_r);
      }else{
        Right_mot_backward(-control_r + deadZone_r);
        }

      if(control_l >= 0){
        Left_mot_forward(control_l + deadZone_l);
      }else{
        Left_mot_backward(-control_l + deadZone_l);
      }

      elapsed_time = millis() - start_time;

    }
}


//task3
int segment = 0;
void loop(){
  switch(segment) {
    case 0:  // Go straight
      Move(40, 0, 1.22, 7.0, 1.5, 44, 37, 30.0, 80.0); 
      Motors_stop();
      delay(10);
      right_encoder.clearCount();
      left_encoder.clearCount();
      segment = 1;  
      break;
    case 1:  // Turn left 90 degrees
      Move(0, 0.5, 2.1, 1.0, 0.002, 44, 37, 30.0, 80.0);
      segment = 2;  
      Motors_stop();
      delay(10);
      right_encoder.clearCount();
      left_encoder.clearCount();
      break;
    case 2:  // Go straight
      Move(40, 0, 1.2, 7.0, 0.5, 44, 37, 30.0, 80.0);  
      segment = 3;  
      Motors_stop();
      delay(10);
      right_encoder.clearCount();
      left_encoder.clearCount();
      break;
    case 3:  // Turn left 90 degrees
      Move(0, 0.5, 2.3, 1.0, 0.002, 44, 37, 30.0, 80.0);
      segment = 4; 
      Motors_stop();
      delay(10);
      right_encoder.clearCount();
      left_encoder.clearCount();
      break;
    case 4:  // Go straight
      Move(40, 0, 0.75, 7.0, 1.5, 44, 37, 30.0, 80.0); 
      segment = 5; 
      Motors_stop();
      delay(10);
      right_encoder.clearCount();
      left_encoder.clearCount();      
      break;
    case 5:  // Turn left 90 degrees
      Move(0, 0.5, 2.0, 1.0, 0.002, 44, 37, 30.0, 80.0);
      segment = 6;
      Motors_stop();
      delay(10); 
      right_encoder.clearCount();
      left_encoder.clearCount(); 
      break;
    case 6: 
      Move(40, 0, 0.7, 7.0, 1.5, 44, 37, 30.0, 80.0); 
      segment = 7;
      Motors_stop();
      delay(10); 
      right_encoder.clearCount();
      left_encoder.clearCount();
      break;
    case 7:  // Turn left 90 degrees
      Move(0, 0.5, 2.05, 1.0, 0.002, 44, 37, 30.0, 80.0);
      segment = -1;
      Motors_stop();
      delay(10);
      right_encoder.clearCount();
      left_encoder.clearCount();
      break;
    case -1:  // Stop
      while(1);  
      break;
    default: 
      delay(1000); 
      break;
  }
  
}


//Task2
// void loop() {
  //float lin_vel, float ang_vel, float t, float Kp, float Ki, float deadZone_r, float deadZone_l, float wheelRadius, float wheelbase

    // Test 1: Go to Blue Square
    // Serial.println("Test Case 1: Moving to Blue Square");
    // Move(55, 1.0, 2.7, 2.0, 1.0, 44, 37, 15.0, 80.0);
    // delay(2000); 
    // Serial.println("end1");

    // // Test 2: Go to Green Square
    // Serial.println("Test Case 2: Moving to Green Square");
    // Move(50, 0, 1.8, 3.7, 1.0, 44, 37, 15.0, 80.0);
    // delay(2000);
    // Serial.println("end2");

    // Serial.println("stop");
    // Motors_stop();

//     while (true) {
//     }
// }


//Task 1 inverseKinematics
// void loop(){
//   // test1
//     lin_vel = 0;
//     ang_vel = pi/4;
//     inverseKinematics(lin_vel, ang_vel, w_r, w_l);
//     Serial.print("Test Case 1 - Right Wheel Velocity: ");
//     Serial.print(w_r);
//     Serial.print(", Left Wheel Velocity: ");
//     Serial.println(w_l);

//     delay(1000);

//     // test2
//     lin_vel = 0;
//     ang_vel = -pi/4;
//     inverseKinematics(lin_vel, ang_vel, w_r, w_l);
//     Serial.print("Test Case 2 - Right Wheel Velocity: ");
//     Serial.print(w_r);
//     Serial.print(", Left Wheel Velocity: ");
//     Serial.println(w_l);

//     delay(1000); 

//     // test3
//     lin_vel = 50;
//     ang_vel = 0;
//     inverseKinematics(lin_vel, ang_vel, w_r, w_l);
//     Serial.print("Test Case 3 - Right Wheel Velocity: ");
//     Serial.print(w_r);
//     Serial.print(", Left Wheel Velocity: ");
//     Serial.println(w_l);

//     delay(1000);

//     Motors_stop();
//     Serial.end();
// }

