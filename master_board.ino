#include "handle_data.h"

#define bus_pin 2
#define button_reset A1

int time_current = millis(), time_pressent = millis();
int time_current_wait = millis(), time_pressent_wait = millis(); 
int Time_change;
int time_check = 2000; //milisecond
int time_wait_recive = 200; //mili

bool Is_in_game = true;

byte winner_client;
uint16_t winner_time = 0;

byte client_recive, work_recive;
uint16_t time_recive;

byte Client_address[6] = {
  0b1000,
  0b1011, //11
  0b1001, //9
  0b1111, //15
};
byte number_of_client = 4;

byte Work_send[4] = {
  0b1010, // reset 
  0b1110, // checking client 
  0b1001, // finnal winner
  0b1101  //start new loop
};


void setup(){
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(A1, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(2, HIGH);

  
  light_me_up(LED_BUILTIN, 4000);

  // sending_mess(0, 0, 0);
  // Is_in_check = true;
  // time_pressent = millis();

  // delay(3000);
  
  // take_time_change();
  // // Serial.println(Time_change);
  // sending_mess(1, Time_change, 1);
  // recive_mess(time_recive, client_recive, work_recive);
  // Serial.println(client_recive);
  // Serial.println(time_recive);

  // delay(3000);
  
  // take_time_change();
  // // Serial.println(Time_change);
  // sending_mess(1, Time_change, 1);
  //recive_mess(time_recive, client_recive, work_recive);

  // Is_in_check = true;
  //loop_game();
  // recive_mess(client_recivce, work_recive, time_recive, bus_pin, 0);
}

void loop(){
  if (!digitalRead(button_reset)){Is_in_game = true;}
  if (Is_in_game){loop_game();}
}

void loop_game(){
  light_me_up(LED_BUILTIN, 2000);
  sending_mess(Client_address[0], Work_send[3], 1, bus_pin, true, false);
  time_pressent = millis();
  winner_client = 0;
  winner_time = 0;

  while (1){
    //loop checking client
    take_time_change();
    if (Time_change > time_check){
      // start to checking client by client
      for (int client = 1; client < number_of_client; client ++){
        delay(1);
        time_pressent_wait = millis();
        sending_mess(Client_address[client], Work_send[1], Time_change, bus_pin, true, true);
        //waiting for recive
        while (1){
          time_current_wait = millis();
          if (time_current_wait - time_pressent_wait > time_wait_recive){break;}
          recive_mess(&client_recive, &work_recive, &time_recive, bus_pin);
          if (client_recive != 0){
            // delay(1);
            // if (client_recive == Client_address[client]){Serial.println("1");}
            // if (work_recive == Work_send[1]){Serial.println("2");}
            break;}
        }
        pinMode(bus_pin, OUTPUT);
        digitalWrite(bus_pin, HIGH);

        uint8_t checking = Is_data_integrity(Client_address[client], Work_send[1], Time_change, client_recive, work_recive, time_recive);
        //handle recive mess
        //Serial.println(client_recive);
        if (checking == 1){Serial.println("right");}
        if (checking == 2 & time_recive > winner_time){
          winner_time = time_recive;
          winner_client = client;
        }
        //reset_argument
        client_recive = 0;
        work_recive = 0;
        time_recive = 0;
        // sending_mess(Client_address[0], Work_send[0], 1, bus_pin, true, false);
        
      }
      //send to winner
      delay(5);
      if (winner_time != 0){
        Serial.println(winner_client);
        Is_in_game = false;
        break;
      }
      reset_interrupt();
      //light_me_up(LED_BUILTIN, 500);
      sending_mess(Client_address[0], Work_send[0], 1, bus_pin, true, false);
      
      time_pressent = millis();
    }
  }
  sending_mess(Client_address[winner_client], Work_send[2], Time_change, bus_pin, true, false);
}


bool checking_time(uint16_t time){
  take_time_change();
  // Serial.println(Time_change);
  if (Time_change > time){
    return true;
  }
  return false;
}

uint8_t Is_data_integrity(uint8_t client_send, uint8_t work_send, uint16_t time_send, uint8_t client_recive, uint8_t work_recive, uint16_t time_recive){
  if (client_send != client_recive || work_send != work_recive){return 0;}
  if (time_send == time_recive){return 1;}
  return 2;
}

void handle_mess(uint8_t &client, uint8_t &work, uint16_t &time){
  // for master board
  if (work == Work_send[1]){
    if (time != 0){
      if (time > winner_time){
        winner_client = client;
        winner_time = time;
      }
    }
  }
}

void showing_bit_argument(uint32_t v){
  uint32_t bitMask;
  for (bitMask = 0x01; bitMask; bitMask <<= 1){
    if (bitMask & v){
      Serial.print(1);
    }
    else{Serial.print(0);}
  }
  Serial.println("");
}


void take_time_change(){
  time_current = millis();
  Time_change = time_current - time_pressent;
}

void light_me_up(byte pin, int delay_mili){
  digitalWrite(pin, HIGH);
  delay(delay_mili);
  digitalWrite(pin, LOW);
}

void reset_interrupt(){
  noInterrupts();
  delayMicroseconds(5);
  interrupts();
}

// void loop_game(){
//   sending_mess(0, 0, 0);
//   time_pressent = millis();

//   winner_client = 0;
//   winner_time = 0;

//   while (1){
//     if (checking_time(time_check)){
//       for (int client = 1; client < 4; client ++){
//         take_time_change();
//         sending_mess(client, Time_change, 1);
//         recive_mess(time_recive, client_recive, work_recive);
//         handle_mess(client_recive, work_recive, time_recive);
//         delay(1);
//       }

//       // if (winner_client != 0){
//       //   Is_in_check = false;
//       //   break;
//       // }
//       sending_mess(0, Time_change, 0);
//       time_pressent = millis();
//     } 
//   }
// }


// void sending_mess(byte client_key, uint16_t time_sent, byte work_key){
//   // time_pressent <= 2^12
//   uint8_t information_send = Client_address[client_key] << 4 ;
//   information_send |= Work_send[work_key];
//   split_argument(time_sent, first_range, second_range);
//   //send work and client key
//   write(information_send, bus_pin, true);
//   //send time for 2 time
//   delayMicroseconds(15);
//   write(first_range, bus_pin, true);
//   delayMicroseconds(15);
//   write(second_range, bus_pin, true); 

//   pinMode(bus_pin, INPUT);
//   digitalWrite(bus_pin, HIGH);

// }

// void recive_mess(uint16_t &time_rc, uint8_t &client_rc, uint8_t &work_rc){
//   int time_start = millis();
//   int time_end = millis();
//   while (1){
//     time_end = millis();
//     if (time_end - time_start > 200){break;}
//     if (!digitalRead(bus_pin)){
//       // Serial.println("here");

//       uint8_t information = read(bus_pin);
//       uint8_t first_time = read(bus_pin);
//       uint8_t second_time = read(bus_pin);
//       Serial.println(second_time);

//       time_rc = second_time << 8;
//       time_rc |= first_time;
//       split_byte(information, work_rc, client_rc);

//       break;
//     }
//   }
// }

