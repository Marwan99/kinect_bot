#include <ros.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float32.h>

ros::NodeHandle  nh;

std_msgs::Int16 left;
std_msgs::Int16 right;

/*Circuit connections:
 * =========Right Motor==========
  -Motor yellow --> Arduino D2 (encoder phase A)
  -Motor green  --> Arduino D3 (encoder phase B)

  ==========Left Motor===========
  -Motor yellow --> Arduino D19 (encoder phase A)
  -Motor green  --> Arduino D18 (encoder phase B)
  
  ============Both================
  -Motor red   --> OUT1 H-bridge
  -Motor white --> OUT2 H-bridge
  -Motor blue  --> 3.3V (encoder power)
  -Motor black --> GND (encoder power)
  -H-bridge IN1 --> Arduino D10
  -H-bridge IN2 --> Arduino D9*/

const byte R_encoderPinA = 2;
const byte R_encoderPinB = 3;
const byte L_encoderPinA = 18;
const byte L_encoderPinB = 19;

const byte r_motor = 9;
const byte l_motor = 10;

volatile long R_pulse_count;
volatile long L_pulse_count;

long prev_millis;

void R_encoderA_ISR()
{

  if (digitalRead(R_encoderPinA) == HIGH)
    (digitalRead(R_encoderPinB) == LOW) ? R_pulse_count++ : R_pulse_count--;
  else
    (digitalRead(R_encoderPinB) == HIGH) ? R_pulse_count++ : R_pulse_count--;
}

void R_encoderB_ISR()
{
  if (digitalRead(R_encoderPinB) == HIGH)
    (digitalRead(R_encoderPinA) == HIGH) ? R_pulse_count++ : R_pulse_count--;
  else
    (digitalRead(R_encoderPinA) == LOW) ? R_pulse_count++ : R_pulse_count--;
}

void L_encoderA_ISR()
{

  if (digitalRead(L_encoderPinA) == HIGH)
    (digitalRead(L_encoderPinB) == LOW) ? L_pulse_count++ : L_pulse_count--;
  else
    (digitalRead(L_encoderPinB) == HIGH) ? L_pulse_count++ : L_pulse_count--;
}

void L_encoderB_ISR()
{
  if (digitalRead(L_encoderPinB) == HIGH)
    (digitalRead(L_encoderPinA) == HIGH) ? L_pulse_count++ : L_pulse_count--;
  else
    (digitalRead(L_encoderPinA) == LOW) ? L_pulse_count++ : L_pulse_count--;
}

//======================ROS===========================
void l_messageCb(const std_msgs::Float32& l_cmd){
  analogWrite(l_motor, int(l_cmd.data));
}

void r_messageCb(const std_msgs::Float32& r_cmd){
  analogWrite(r_motor, int(r_cmd.data));
}

ros::Subscriber<std_msgs::Float32> l_sub("l_motor_cmd", &l_messageCb );
ros::Subscriber<std_msgs::Float32> r_sub("r_motor_cmd", &r_messageCb );

ros::Publisher lwheel("lwheel", &left);
ros::Publisher rwheel("rwheel", &right);


void setup()
{
  nh.initNode();
  nh.advertise(lwheel);
  nh.advertise(rwheel);
  nh.subscribe(l_sub);
  nh.subscribe(r_sub); 

  pinMode(R_encoderPinA, INPUT_PULLUP);
  pinMode(R_encoderPinA, INPUT_PULLUP);
  pinMode(L_encoderPinB, INPUT_PULLUP);
  pinMode(L_encoderPinB, INPUT_PULLUP);

  pinMode(r_motor, OUTPUT);
  pinMode(l_motor, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(R_encoderPinA), R_encoderA_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(R_encoderPinB), R_encoderB_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(L_encoderPinA), L_encoderA_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(L_encoderPinB), L_encoderB_ISR, CHANGE);
}

void loop()
{
    left.data = L_pulse_count;
    right.data = R_pulse_count;
    lwheel.publish(&left);
    rwheel.publish(&right);
    nh.spinOnce();
    delay(10);
}
