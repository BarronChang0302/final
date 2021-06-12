#include"mbed.h"
#include "bbcar.h"
Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx
DigitalInOut ping(D12);
BufferedSerial xbee(D10, D9);
BBCar car(pin5, pin6, servo_ticker);
DigitalIn encoder(D11);
volatile int steps;
volatile int last;
Thread t1, t2;
Timer t;

int global_state = 0;
char data[35] = {0};
int first = 1;
int angle = 0;
float val;

void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
}

void for_ping(void) {
   while(1) {
      ping.output();
      ping = 0; wait_us(200);
      ping = 1; wait_us(5);
      ping = 0; wait_us(5);
      ping.input();
      while(ping.read() == 0);
      t.start();
      while(ping.read() == 1);
      val = t.read();
      val = val*17700.4f;
      t.stop();
      t.reset();
      ThisThread::sleep_for(5s);
   }
}

void car_control(void) {
    int distance, sign, last_angle = 180, diff, last_change = 2;
    int type;
    int t_x, sign2;
    int flag_line = 0, flag_ap = 0;
    int d_park = 0;
    global_state = 0;
    car.stop();
    printf("Start\n");
    while(1) {
        if(data[22] == 'y') flag_line = 1;
        else if(data[22] == 'n') flag_line = 0;
        if(data[0] == 'Y') flag_ap = 1;
        else if(data[0] == 'N') flag_ap = 0;
        printf("%d, %d\n", global_state, d_park);
        if(global_state == 0) {
            if(flag_line) {
                int diff = 100 * int(data[24] - '0') + 10 * int(data[25] - '0') + int(data[26] - '0');
                int sign_line = int(data[28] - '0');
                printf("%d, %d\n", diff, sign_line);
                if(sign_line == 0) car.twoSpeed(30, -30-diff/1.5f);
                else car.twoSpeed(30+diff/1.5f, -30);                     // 200 200 20,diff/2
            }
            else car.twoSpeed(0, 0);
            ThisThread::sleep_for(200ms);
            first = 1;
            distance = 100 * int(data[6] - '0') + 10 * int(data[7] - '0') + int(data[8] - '0');
            if(distance < 70 && flag_ap == 1 && val < 70) {
                global_state = 1;
                car.stop();
                ThisThread::sleep_for(1000ms);
            }
        }
        else if(global_state == 1) {
            if(flag_ap) {
                if(first == 1) {
                    car.stop();
                    ThisThread::sleep_for(1000ms);
                    car.goStraight(30);   
                    ThisThread::sleep_for(1000ms);
                    car.stop();
                    ThisThread::sleep_for(500ms);

                    distance = 100 * int(data[6] - '0') + 10 * int(data[7] - '0') + int(data[8] - '0');
                    angle = 100 * int(data[10] - '0') + 10 * int(data[11] - '0') + int(data[12] - '0');
                    sign = int(data[14] - '0');
                    if(sign == 2) angle = -angle;

                    last_angle = angle;
                    car.goStraight(30);   
                    ThisThread::sleep_for(2500ms);
                    car.stop();
                    ThisThread::sleep_for(300ms); 

                    distance = 100 * int(data[6] - '0') + 10 * int(data[7] - '0') + int(data[8] - '0');
                    angle = 100 * int(data[10] - '0') + 10 * int(data[11] - '0') + int(data[12] - '0');
                    sign = int(data[14] - '0');
                    if(sign == 2) angle = -angle;
                    diff = angle - last_angle;
                    if(angle >= 0 && last_angle >= 0 && diff < 0) type = 0;  // right vertical
                    else if(angle >= 0 && last_angle >= 0 && diff >= 0) type = 1;   // right slant
                    else if(angle <= 0 && last_angle <= 0 && diff > 0) type = 2;  // left vertical
                    else if(angle <= 0 && last_angle <= 0 && diff <= 0) type = 3;  // left slant
                    else if(angle <= 0 && last_angle >= 0) type = 4; // then do l -> r
                    else if(angle >= 0 && last_angle <= 0) type = 5; // then do r -> l 
                    last_angle = angle;
                }
                else if(!first && distance >= 30 && flag_ap) {
                    if(type == 0 || type == 2) {
                        car.goStraight(30);   
                        if(distance >= 50) ThisThread::sleep_for(3000ms);  
                        else ThisThread::sleep_for(1000ms);  
                    }
                    else if(type == 1) {
                        car.turn(20, 0.1);
                        ThisThread::sleep_for(1000ms);
                    }
                    else if(type == 3) {
                        car.turn(20, -0.05);
                        ThisThread::sleep_for(1000ms);
                    }
                    else if(type == 4) {
                        car.turn(20, -0.05);
                        ThisThread::sleep_for(1000ms);
                    }
                    else if(type == 5) {
                        car.turn(20, 0.1);
                        ThisThread::sleep_for(1000ms);
                    }
                    else if(type == 6) {
                        car.stop();
                    }
                    car.stop();
                    ThisThread::sleep_for(300ms);    
                    if(flag_ap) {
                        distance = 100 * int(data[6] - '0') + 10 * int(data[7] - '0') + int(data[8] - '0');
                        angle = 100 * int(data[10] - '0') + 10 * int(data[11] - '0') + int(data[12] - '0');
                        sign = int(data[14] - '0');
                        if(sign == 2) angle = -angle;
                        t_x = 100 * int(data[16] - '0') + 10 * int(data[17] - '0') + int(data[18] - '0');
                        sign2 = int(data[20] - '0');
                        if(sign2 == 2) t_x = -t_x;
                        diff = angle - last_angle;

                        if(angle >= 0 && last_angle >= 0 && diff < 0) type = 0;  // right vertical
                        else if(angle >= 0 && last_angle >= 0 && diff >= 0) type = 1;   // right slant
                        else if(angle <= 0 && last_angle <= 0 && diff > 0) type = 2;  // left vertical
                        else if(angle <= 0 && last_angle <= 0 && diff <= 0) type = 3;  // left slant
                        else if(angle <= 0 && last_angle >= 0) type = 4; // then do l -> r
                        else if(angle >= 0 && last_angle <= 0) type = 5; // then do r -> l
                        last_angle = angle;
                    }
                }
                else if((t_x > 3 || t_x < -3) && flag_ap) {
                    if(t_x > 0) {
                        car.turn(20, 0.1);
                        type = 5;
                        ThisThread::sleep_for(500ms);
                    }
                    else if(t_x < 0) {
                        car.turn(20, -0.05);
                        type = 4;
                        ThisThread::sleep_for(500ms);
                    }
                    car.stop();
                    ThisThread::sleep_for(200ms);
                    angle = 100 * int(data[10] - '0') + 10 * int(data[11] - '0') + int(data[12] - '0');
                    sign = int(data[14] - '0');
                    if(sign == 2) angle = -angle;
                    t_x = 100 * int(data[16] - '0') + 10 * int(data[17] - '0') + int(data[18] - '0');
                    sign2 = int(data[20] - '0');
                    if(sign2 == 2) t_x = -t_x;
                }
                else {
                    char buffer[26];
                    car.stop();
                    type = 6;
                    angle = 100 * int(data[10] - '0') + 10 * int(data[11] - '0') + int(data[12] - '0');
                    sign = int(data[14] - '0');
                    if(sign == 2) angle = -angle;
                    if(angle > 5) {
                        car.goStraight(-30);   
                        ThisThread::sleep_for(1500ms);
                        car.turn(20, 0.1);
                        ThisThread::sleep_for(3000ms);
                        car.goStraight(30);   
                        ThisThread::sleep_for(500ms);
                        type = 0;
                    }
                    else if(angle < -5) {
                        car.goStraight(-30);   
                        ThisThread::sleep_for(1500ms);
                        car.turn(20, -0.05);
                        ThisThread::sleep_for(3000ms);
                        car.goStraight(30);   
                        ThisThread::sleep_for(500ms);
                        type = 2;
                    }
                    else {
                        if(angle < 0) angle = -angle;
                        while(val > 30) {
                            car.goStraight(30);   
                            ThisThread::sleep_for(500ms);
                            car.stop();
                            ThisThread::sleep_for(500ms);
                        }
                        
                            sprintf(buffer, "Distance %03d, Angle %03d\r\n", int(val), angle);
                            xbee.write(buffer, sizeof(buffer));
                            printf("%s\n", buffer);
                            d_park = val;
                            int id = 10 * int(data[3] - '0') + int(data[4] - '0');
                            if(id == 0) global_state = 2;
                            else if(id == 2) global_state = 4;
                    }
                }
                first = 0;
            }
            else if(flag_ap == 0){
                if(type == 0) {
                //   printf("right vertical over, go right\n");
                    car.turn(20, -0.05);
                    ThisThread::sleep_for(1000ms);
                }
                else if(type == 2) {
                //   printf("left vertical over, go left\n");
                    car.turn(20, 0.1);
                    ThisThread::sleep_for(1000ms);
                }
                else if(type == 1) {
                //   printf("right slant over, go right\n");
                    car.turn(20, -0.05);
                    ThisThread::sleep_for(1000ms);
                }
                else if(type == 3) {
                // printf("left slant over, go left\n");
                    car.turn(20, 0.1);
                    ThisThread::sleep_for(1000ms);
                }
                else if(type == 4) {
                // printf("l -> r over, go left\n");
                    car.turn(20, 0.1);
                    ThisThread::sleep_for(1000ms);
                }
                else if(type == 5) {
                //  printf("r -> l over, go right\n");
                    car.turn(20, -0.05);
                    ThisThread::sleep_for(1000ms);
                }
                else if(type == 6) car.stop();
                car.stop();
                ThisThread::sleep_for(300ms);
                if(flag_ap) {
                    distance = 100 * int(data[6] - '0') + 10 * int(data[7] - '0') + int(data[8] - '0');
                    angle = 100 * int(data[10] - '0') + 10 * int(data[11] - '0') + int(data[12] - '0');
                    sign = int(data[14] - '0');
                    if(sign == 2) angle = -angle;
                    t_x = 100 * int(data[16] - '0') + 10 * int(data[17] - '0') + int(data[18] - '0');
                    sign2 = int(data[20] - '0');
                    if(sign2 == 2) t_x = -t_x;

                    if(angle <= 0 && type == 0) type = 4;     //2222222
                    else if(angle >= 0 && type == 2) type = 5;   //22222222
                    else if(angle >= 0 && type == 0) type = 4; 
                    else if(angle >= 0 && type == 2) type = 5;
                    else if(angle >= 0 && type == 1) type = 0;  
                    else if(angle <= 0 && type == 3) type = 2;  
                    else if(angle >= 0 && type == 4) type = 5; 
                    else if(angle <= 0 && type == 5) type = 4; 
                    last_angle = angle;
                }
            }
        }
        else if(global_state == 2) {
            encoder_ticker.attach(&encoder_control, 10ms);
            steps = 0;
            last = 0;
            car.goStraight(-40);
            while(steps*6.5*3.14/32 < 40 - d_park) ThisThread::sleep_for(10ms);

            car.stop();
            ThisThread::sleep_for(1s);

            car.turn(-200, -0.1);
            ThisThread::sleep_for(850ms);

            car.stop();
            ThisThread::sleep_for(1s);

            steps = 0;
            last = 0;
            car.goStraight(-40);
            while(steps*6.5*3.14/32 < 20) ThisThread::sleep_for(10ms);  
            car.stop();
            printf("Finish Parking\n");
            ThisThread::sleep_for(5s);
            global_state = 3;
        }
        else if(global_state == 3) {
            car.goStraight(30);
            ThisThread::sleep_for(4000ms);
            global_state = 1;
            first = 1;
        }
    }
}

int main(){
   int count = 0;
   uart.set_baud(9600);
   xbee.set_baud(9600);
   t1.start(car_control);
   t2.start(for_ping);
   while(1){
      if(uart.readable()){
         char recv[1];
         uart.read(recv, sizeof(recv));
      //   printf("%c\n", recv[0]);
         if(recv[0] == 'Y' || recv[0] == 'N') {
            count = 0;
         }
         if(count >= 0 && count <= 35) data[count] = recv[0];
         count++;
      /*  if(recv[0] == 'Y' || recv[0] == 'N') {
            printf("%s", data);
            printf("\n");
        }*/
      }
   }
}