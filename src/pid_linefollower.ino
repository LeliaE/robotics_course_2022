// IR Sensors
int sensor0 = 0;      // Left most sensor
int sensor1 = 1;  
int sensor2 = 2;
int sensor3 = 3;
int sensor4 = 4;      // Right most sensor

// Initial Values of Sensors
int sensor[5] = {0, 0, 0, 0, 0};

// Motor Variables
int ENA = 5; //left
int motorInput1 = 7;
int motorInput2 = 8;
int ENB = 6; //right

//Initial Speed of Motor
int initial_motor_speed = 140;

// PID Constants
float Kp = 25;
float Ki = 0;
float Kd = 15;

float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0, previous_I = 0;

int flag = 0;

void setup()
{
  pinMode(sensor0, INPUT);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);

  pinMode(motorInput1, OUTPUT);
  pinMode(motorInput2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  Serial.begin(9600);                     //setting serial monitor at a default baund rate of 9600
  delay(500);
  Serial.println("Started !!");
  delay(1000);
}
void loop()
{
  read_sensor_values();
  //Serial.println(error);
  calculate_pid();
  motor_control();
}

void read_sensor_values()
{
  sensor[0] = !digitalRead(sensor0);
  sensor[1] = !digitalRead(sensor1);
  sensor[2] = !digitalRead(sensor2);
  sensor[3] = !digitalRead(sensor3);
  sensor[4] = !digitalRead(sensor4);


  if ((sensor[0] == 1) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 0) && (sensor[4] == 0))
    error = 3;
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[3] == 0) && (sensor[4] == 0))
    error = 2;
  else if ((sensor[0] == 0) && (sensor[1] == 1) && (sensor[3] == 0) && (sensor[4] == 0))
    error = 1;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 1) && (sensor[3] == 0) && (sensor[4] == 0))
    error = 0;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[3] == 1) && (sensor[4] == 0))
    error = -1;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[3] == 1) && (sensor[4] == 1))
    error = -2;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 0) && (sensor[4] == 1))
    error = -3;
}

void calculate_pid()
{
  P = error;
  I = I + previous_I;
  D = error - previous_error;

  PID_value = (Kp * P) + (Ki * I) + (Kd * D);

  previous_I = I;
  previous_error = error;
}

void motor_control()
{
  // Calculating the effective motor speed:
  int left_motor_speed = initial_motor_speed - PID_value;
  int right_motor_speed = initial_motor_speed + PID_value;

  // The motor speed should not exceed the max PWM value
  left_motor_speed = constrain(left_motor_speed, 0, 255);
  right_motor_speed = constrain(right_motor_speed, 0, 255);

  Serial.print(PID_value);
    Serial.print("\t");
    Serial.print(left_motor_speed);
    Serial.print("\t");
    Serial.println(right_motor_speed);

  analogWrite(ENA, left_motor_speed); //Left Motor Speed
  analogWrite(ENB, right_motor_speed - 30); //Right Motor Speed

  //following lines of code are to make the bot move forward
  forward();
}

void forward()
{
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, HIGH);
}
