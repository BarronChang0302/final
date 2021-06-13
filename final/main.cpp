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
int pre_state = 0;

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
     // printf("%f\n", val);
      t.stop();
      t.reset();
      ThisThread::sleep_for(10ms);
   }
}

void car_control(void) {
    int distance, sign, last_angle = 180, diff, last_change = 2;
    int type;
    int t_x, sign2;
    int flag_line = 0, flag_ap = 0;
    int d_park = 0;
    char buffer[16]={0};
    float en_d = 0.0;
    int avoid = 0;

    global_state = 0;
    car.stop();
    sprintf(buffer, "Start         \r\n");
    xbee.write(buffer, sizeof(buffer));
    while(1) {
        if(data[22] == 'y') flag_line = 1;
        else if(data[22] == 'n') flag_line = 0;
        if(data[0] == 'Y') flag_ap = 1;
        else if(data[0] == 'N') flag_ap = 0;
 //       printf("%d, %d\n", global_state, d_park);
        if(global_state == 0) {
            if(flag_line) {
                int diff = 100 * int(data[24] - '0') + 10 * int(data[25] - '0') + int(data[26] - '0');
                int sign_line = int(data[28] - '0');
            //    printf("%d, %d\n", diff, sign_line);
                if(sign_line == 0) car.twoSpeed(25, -25-diff/1.5f);
                else car.twoSpeed(25+diff/1.5f, -25);                     // 200 200 20,diff/2
            }
            else car.twoSpeed(0, 0);
            ThisThread::sleep_for(200ms);
            first = 1;
            distance = 100 * int(data[6] - '0') + 10 * int(data[7] - '0') + int(data[8] - '0');
            if(pre_state == 0 && distance < 60 && flag_ap == 1 && distance > 20) {
                global_state = 1;
                car.stop();
                sprintf(buffer, "Line finish   \r\n");
                xbee.write(buffer, sizeof(buffer));
                ThisThread::sleep_for(1000ms);
            }
            else if(pre_state == 6) {
                if(val < 50) {
                    global_state = 7;
                    car.stop();
                    sprintf(buffer, "Line finish   \r\n");
                    xbee.write(buffer, sizeof(buffer));
                    ThisThread::sleep_for(1000ms);
                }
            }
        }
        else if(global_state == 1) {
         //   printf("AP:%d\n", flag_ap);
            if(flag_ap) {
                if(first == 1) {
                    avoid = 0;
                    car.stop();
                    ThisThread::sleep_for(1000ms);
                    car.goStraight(30);   
                    ThisThread::sleep_for(500ms);  //1000
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
                    if(diff < 2 && diff > -2) type = -1;
                    else if(angle >= 0 && last_angle >= 0 && diff < 0) type = 0;  // right vertical
                    else if(angle >= 0 && last_angle >= 0 && diff >= 0) type = 1;   // right slant
                    else if(angle <= 0 && last_angle <= 0 && diff > 0) type = 2;  // left vertical
                    else if(angle <= 0 && last_angle <= 0 && diff <= 0) type = 3;  // left slant
                    else if(angle <= 0 && last_angle >= 0) type = 4; // then do l -> r
                    else if(angle >= 0 && last_angle <= 0) type = 5; // then do r -> l 
                    last_angle = angle;
                }
                else if(!first && distance >= 30) {   //modify
                //else if(!first && (distance >= 30 || val >= 30) && flag_ap ) {   //modify
                    car.stop();
                    ThisThread::sleep_for(300ms);  
                    if(type == -1) {
                        car.goStraight(30); 
                        if(distance >= 35) ThisThread::sleep_for(1500ms);  
                        else ThisThread::sleep_for(500ms);  
                    }
                    else if(type == 0 || type == 2) {
                        car.goStraight(30);   
                        if(distance >= 35) ThisThread::sleep_for(1500ms);  
                        else ThisThread::sleep_for(500ms);  
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
                        ThisThread::sleep_for(800ms);
                    }
                    else if(type == 5) {
                        car.turn(20, 0.1);
                        ThisThread::sleep_for(800ms);
                    }
                    else if(type == 6) {
                        car.stop();
                    }
                    car.stop();
                    ThisThread::sleep_for(1000ms);   
                    if(data[0] == 'Y') flag_ap = 1;
                    else if(data[0] == 'N') flag_ap = 0;
                    if(flag_ap) {
                        distance = 100 * int(data[6] - '0') + 10 * int(data[7] - '0') + int(data[8] - '0');
                        angle = 100 * int(data[10] - '0') + 10 * int(data[11] - '0') + int(data[12] - '0');
                        sign = int(data[14] - '0');
                        if(sign == 2) angle = -angle;
                        t_x = 100 * int(data[16] - '0') + 10 * int(data[17] - '0') + int(data[18] - '0');
                        sign2 = int(data[20] - '0');
                        if(sign2 == 2) t_x = -t_x;
                        diff = angle - last_angle;

                        if(/*!avoid &&*/ diff < 2 && diff > -2 && angle <= 2 && angle >= -2 && t_x <= 2 && t_x >= -2) type = -1;
                        else if(/*!avoid &&*/ diff < 2 && diff > -2 && (angle > 2 || t_x >= 2)) type = 5;
                        else if(/*!avoid &&*/ diff < 2 && diff > -2 && (angle < -2 || t_x <= -2)) type = 4;
                        else if(angle >= 0 && last_angle >= 0 && diff < 0) type = 0;  // right vertical
                        else if(angle >= 0 && last_angle >= 0 && diff >= 0) type = 1;   // right slant
                        else if(angle <= 0 && last_angle <= 0 && diff > 0) type = 2;  // left vertical
                        else if(angle <= 0 && last_angle <= 0 && diff <= 0) type = 3;  // left slant
                        else if(angle <= 0 && last_angle >= 0) type = 4; // then do l -> r
                        else if(angle >= 0 && last_angle <= 0) type = 5; // then do r -> l
                        last_angle = angle;
                        printf("     A:%d, Dif:%d, TX:%d, typ:%d\n",angle, diff, t_x, type);
                    }
                    avoid = 0;
             //       printf("%d\n", type);
                }
                else if((t_x > 2 || t_x < - 2)) {
                    if(t_x > 0) {
                        car.turn(20, 0.1);
                        type = 5;
                        ThisThread::sleep_for(500ms);
                     //   printf("A\n");
                    }
                    else if(t_x < 0) {
                        car.turn(20, -0.05);
                        type = 4;
                        ThisThread::sleep_for(500ms);
                     //   printf("B\n");
                    }
                    car.stop();
                    ThisThread::sleep_for(500ms);
                    angle = 100 * int(data[10] - '0') + 10 * int(data[11] - '0') + int(data[12] - '0');
                    sign = int(data[14] - '0');
                    if(sign == 2) angle = -angle;
                    t_x = 100 * int(data[16] - '0') + 10 * int(data[17] - '0') + int(data[18] - '0');
                    sign2 = int(data[20] - '0');
                    if(sign2 == 2) t_x = -t_x;
                }
                else {
              //      char buffer[26];
                    car.stop();
                    ThisThread::sleep_for(500ms);  
                    type = 6;
                    if(angle > 3) {    //2
                        car.goStraight(-30);   
                        ThisThread::sleep_for(1500ms);
                        car.turn(20, 0.1);
                        ThisThread::sleep_for(3000ms); //3000
                        car.goStraight(30);   
                        ThisThread::sleep_for(500ms);
                        car.stop();   
                        ThisThread::sleep_for(1000ms);
                        type = 0;   // -1
                        avoid = 1;
                    }
                    else if(angle < -3) {
                        car.goStraight(-30);   
                        ThisThread::sleep_for(1500ms);
                        car.turn(20, -0.05);
                        ThisThread::sleep_for(3000ms);
                        car.goStraight(30);   
                        ThisThread::sleep_for(500ms);
                        car.stop();   
                        ThisThread::sleep_for(1000ms);
                        type = 2;  //-1
                        avoid = 1;
                    }
                    angle = 100 * int(data[10] - '0') + 10 * int(data[11] - '0') + int(data[12] - '0');
                    sign = int(data[14] - '0');
                    if(sign == 2) angle = -angle;
                    last_angle = angle;
                    if(angle <= 2 && angle >= -2) { 
                      //  if(angle < 0) angle = -angle;
                            int id = 10 * int(data[3] - '0') + int(data[4] - '0');
                            sprintf(buffer, "Tag finish,ID%01d\r\n", id);
                            if(id != 1) xbee.write(buffer, sizeof(buffer));
                         //   sprintf(buffer, "%03d\r\n");
                         //   xbee.write(buffer, sizeof(buffer));
                            d_park = val;
                         //   int id = 10 * int(data[3] - '0') + int(data[4] - '0');
                            if(id == 0) global_state = 2;
                            else if(id == 2) global_state = 4;
                            else if(id == 3) global_state = 6;
                            en_d = steps*6.5*3.14/32;
                    }
                }
                first = 0;
            }
            else if(flag_ap == 0){
            //    printf("OOps\n");
                if(type == -1) {
                  //  sprintf(buffer, "GG            \r\n");
                  //          xbee.write(buffer, sizeof(buffer));
                  car.goStraight(-30);   
                        ThisThread::sleep_for(1500ms);
                }
                else if(type == 0) {
                //   printf("right vertical over, go right\n");
                    car.turn(20, -0.05);
                    ThisThread::sleep_for(1500ms);
                }
                else if(type == 2) {
                //   printf("left vertical over, go left\n");
                    car.turn(20, 0.1);
                    ThisThread::sleep_for(1000ms);
                }
                else if(type == 1) {
                //   printf("right slant over, go right\n");
                    car.turn(20, -0.05);
                    ThisThread::sleep_for(1500ms);
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
                    ThisThread::sleep_for(1500ms);
                }
                else if(type == 6) car.stop();
                car.stop();
                ThisThread::sleep_for(300ms);
                if(data[0] == 'Y') flag_ap = 1;
                else if(data[0] == 'N') flag_ap = 0;
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
                    else if(/*angle >= 0 && */type == 4) {
                        car.turn(20, -0.05);
                        ThisThread::sleep_for(1500ms);
                        car.goStraight(30);   
                        ThisThread::sleep_for(1000ms);
                        car.stop();
                        type = 5;   // 5 
                    }
                    else if(/*angle <= 0 && */type == 5) {
                        car.turn(20, 0.1);
                        ThisThread::sleep_for(1000ms);
                        car.goStraight(30);   
                        ThisThread::sleep_for(1000ms);
                        car.stop();
                        type = 4;   // 4
                    }
                    last_angle = angle;
                }
            }
            printf("D:%d, Dif:%d, TX:%d, typ:%d\n",distance, diff, t_x, type);
        }
        else if(global_state == 2) {
            encoder_ticker.attach(&encoder_control, 10ms);
            steps = 0;
            last = 0;
            car.goStraight(-40);
            while(steps*6.5*3.14/32 < 35 - d_park) ThisThread::sleep_for(10ms);

            car.stop();
            ThisThread::sleep_for(1s);

            car.turn(-200, -0.1);
            ThisThread::sleep_for(850ms);

            car.stop();
            ThisThread::sleep_for(1s);

            steps = 0;
            last = 0;
            car.goStraight(-40);
            while(steps*6.5*3.14/32 < 22) ThisThread::sleep_for(10ms);  
            car.stop();
       //     printf("Finish Parking\n");
            sprintf(buffer, "Parking finish\r\n");
            xbee.write(buffer, sizeof(buffer));
            ThisThread::sleep_for(5s);
            global_state = 3;
        }
        else if(global_state == 3) {
            car.goStraight(30);
            ThisThread::sleep_for(5000ms);
            global_state = 1;
            first = 1;
        }
        else if(global_state == 4) {
            while(val > 20) {
                car.goStraight(30);
                ThisThread::sleep_for(500ms);
            }
          //  car.goStraight(30);
          //  ThisThread::sleep_for(1500ms);
            car.stop();
            ThisThread::sleep_for(1000ms);
            car.turn(200, -0.05);
            ThisThread::sleep_for(755ms);
            car.stop();
            ThisThread::sleep_for(1000ms);
            while(val >= 20) {
                car.goStraight(30);
                ThisThread::sleep_for(10ms);
            }
            car.stop();
            ThisThread::sleep_for(1000ms);
            car.turn(200, -0.05);
            ThisThread::sleep_for(755ms);
            car.stop();
            ThisThread::sleep_for(1000ms);
            while(val >= 15) {
                car.goStraight(30);
                ThisThread::sleep_for(10ms);
            }
            car.stop();
            ThisThread::sleep_for(1000ms);

            global_state = 5;
        }
        else if (global_state == 5) {
            int i = 0;
            int label;
            for(i = 0; i < 5; i++) {
                int label = int(data[28] - '0');
                if(label == 0) sprintf(buffer, "Cat           \r\n");
                else sprintf(buffer, "Dog           \r\n");
                xbee.write(buffer, sizeof(buffer));
                printf("%s\n", buffer);
                ThisThread::sleep_for(1000ms);
            }
            car.turn(200, -0.05);
            ThisThread::sleep_for(755ms);
            car.stop();
            ThisThread::sleep_for(1000ms);
            int sign_line = int(data[28] - '0');
            diff = 100 * int(data[24] - '0') + 10 * int(data[25] - '0') + int(data[26] - '0');
            while(diff > 5) {
                if(sign_line == 0) car.turn(20, 0.1);
                else car.turn(20, -0.05);
                ThisThread::sleep_for(500ms);
                car.stop();
                sign_line = int(data[28] - '0');
                diff = 100 * int(data[24] - '0') + 10 * int(data[25] - '0') + int(data[26] - '0');
            }
            car.stop();
            ThisThread::sleep_for(1000ms);
            global_state = 0;
            steps = 0;
            last = 0;
        }
        else if(global_state == 6) {
            car.twoSpeed(50, 50); 
            ThisThread::sleep_for(2400ms);
            car.stop();
            ThisThread::sleep_for(1000ms);
            int sign_line = int(data[28] - '0');
            diff = 100 * int(data[24] - '0') + 10 * int(data[25] - '0') + int(data[26] - '0');
            while(diff > 5) {
                if(sign_line == 0) car.turn(20, 0.1);
                else car.turn(20, -0.05);
                ThisThread::sleep_for(300ms);
                car.stop();
                sign_line = int(data[28] - '0');
                diff = 100 * int(data[24] - '0') + 10 * int(data[25] - '0') + int(data[26] - '0');
            }
            car.stop();
            ThisThread::sleep_for(1000ms);
            sprintf(buffer, "Turn finish   \r\n");
            xbee.write(buffer, sizeof(buffer));
            global_state = 0;
            steps = 0;
            last = 0;
            pre_state = 6;
        }
        else if(global_state == 7) {
            while(val > 20) {
                car.goStraight(30);
                ThisThread::sleep_for(10ms);
            }
            car.stop();
            ThisThread::sleep_for(1000ms);
            car.turn(200, 0.05);
            ThisThread::sleep_for(730ms);
            car.stop();
            ThisThread::sleep_for(1000ms);
            while(val > 25) {
                car.goStraight(30);
                ThisThread::sleep_for(10ms);
            }
            car.stop();
            ThisThread::sleep_for(1000ms);

            global_state = 8;
        }
        else if(global_state == 8) {
            int i = 0;
            int label;
            for(i = 0; i < 5; i++) {
                int label = int(data[28] - '0');
                if(label == 0) sprintf(buffer, "Cat           \r\n");
                else sprintf(buffer, "Dog           \r\n");
                xbee.write(buffer, sizeof(buffer));
                printf("%s\n", buffer);
                ThisThread::sleep_for(1000ms);
            }
            sprintf(buffer, "All Done      \r\n");
            xbee.write(buffer, sizeof(buffer));
            global_state = 9;
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