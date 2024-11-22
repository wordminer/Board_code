#include "handle_data.h"

#define bus_pin 2
#define button_pin A1

uint8_t client_stt = 0b1111;
byte client_key = 3;

byte client_recive, work_recive;
uint16_t time_recive;
byte first_range, second_range;

bool have_click_button = false;
int time_pressent = millis(), time_current = millis();
int Time_change;

uint16_t Time_press = 0;

byte Work_send[4] = {
  0b1010, // reset 
  0b1110, // checking client 
  0b1001, // finnal winner
  0b1101  //start new loop
};

//test

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(bus_pin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A1, INPUT_PULLUP);
  digitalWrite(bus_pin, LOW);
  
}

void loop() {
  // Serial.println(digitalRead(3));
  // take_time_change();
  // recive_mess(time_recive, client_recive, work_recive);
  // handle_mess(client_recive, work_recive, time_recive);
  // //Serial.println("here");
  // time_recive = 0;
  // client_recive = 0;
  // work_recive = 0;
  take_time_change();
  if (!have_click_button && !digitalRead(button_pin)){
    Time_press = Time_change;
    have_click_button = true;
  }

  recive_mess(&client_recive, &work_recive, &time_recive, bus_pin);
  //int start = millis();
  //if (client_recive != 0){Serial.println(work_recive);}
  reset_interrupt();
  handle_mess(&client_recive, &work_recive, &time_recive);
}


uint16_t adjust_time(uint16_t time_change, uint16_t time_change_benchmark, uint16_t time_adjust){
  return time_adjust;
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

void handle_mess(uint8_t *client, uint8_t *work, uint16_t *time){
  if (*work == Work_send[3]){
    time_pressent = millis(); // reset the time
    Time_press = 0;
    have_click_button = false;
  }
  if (*client == 0b1000 || *client == client_stt){
    if (*work == Work_send[0]){
      light_me_up(LED_BUILTIN, 50);
      time_pressent = millis(); // reset the time
    }
    if (*work == Work_send[1]){
      //light_me_up(LED_BUILTIN, 200);
      // uint16_t need_send = convert_to_byte(send);
      if (Time_press == 0){sending_mess(client_stt, Work_send[1], *time, bus_pin, true, true);}
      else {sending_mess(client_stt, Work_send[1], Time_press, bus_pin, true, true); }
      delayMicroseconds(10);
    }
    if (*work == Work_send[2]){
      // Serial.println(*work);
      light_me_up(LED_BUILTIN, 2000);
    }
  }
  *client = 0;
  *work = 0;
  *time = 0;
}

uint16_t convert_to_byte(int value_converted){
  uint16_t bitMask;
  uint16_t value_return;
  for (bitMask = 0x01; bitMask; bitMask <<= 1){
    if (bitMask & value_converted){value_return |= bitMask;}
  }
  // Serial.println(value_return);
  // Serial.println(sizeof(value_return));
  return value_return;
}

void reset_interrupt(){
  noInterrupts();
  delayMicroseconds(5);
  interrupts();
}

// void handle_mess(uint8_t &client, uint8_t &work, uint16_t &time){
//   // for client board
//   if (client == 0b1000 || client == client_stt){
//     if (work == Work_send[0]){
//       Is_in_work = true;
//       time_pressent = millis(); // reset the time
//       Time_press = 0;
//     }
//     if (work == Work_send[1]){
//       uint16_t time_sending = adjust_time(Time_change, time, Time_press);
//       sending_mess(3210, 1);
//       //light_me_up(LED_BUILTIN, 1000);
//     }
//   }
// }

// void sending_mess(uint16_t time_sent, byte work_key){
//   // time_set <= 2^12
//   uint8_t information_send = client_stt << 4 ;
//   information_send |= Work_send[work_key];
//   split_argument(time_sent, first_range, second_range);
//   //send work and client key
//   write(information_send, bus_pin, true);
//   //send time for 2 time
//   delayMicroseconds(15);
//   write(first_range, bus_pin, true);
//   delayMicroseconds(12);
//   write(second_range, bus_pin, true); 
//   // special after sending that must put it into recive wait
//   pinMode(bus_pin, INPUT);
//   digitalWrite(bus_pin, HIGH);
//   Serial.println(first_range);
//   Serial.println(second_range);
  
// }

// void recive_mess(uint16_t &time_rc, uint8_t &client_rc, uint8_t &work_rc){
//   while (1){
//     if (!digitalRead(bus_pin)){
//       uint8_t information = read(bus_pin);
//       uint8_t first_time = read(bus_pin);
//       uint8_t second_time = read(bus_pin);
//       time_rc = second_time << 8;
//       time_rc |= first_time;
//       split_byte(information, work_rc, client_rc);
//       // Serial.println(information);
//       break;
//     }
//   }
// }

// void split_argument(uint16_t value, uint8_t &first_byte, uint8_t &second_byte){
//   uint8_t bitMask;
//   first_byte = 0;
//   second_byte = 0;
//   for (bitMask = 0x01; bitMask; bitMask <<= 1){
//     if (bitMask & value){first_byte |= bitMask;}
//   }
//   for (bitMask = 0x01; bitMask; bitMask <<= 1){
//     if (bitMask<<8 & value){second_byte |= bitMask;}
//   }
// }

// void split_byte(uint8_t byte_split, uint8_t &first_4b, uint8_t &second_4b){
//   uint8_t bitMask;
//   first_4b = 0;
//   second_4b = 0;
//   for (bitMask = 0x01; bitMask < 0x01<<4; bitMask <<= 1){
//     if (bitMask & byte_split){first_4b |= bitMask;}
//   }
//   for (bitMask = 0x01; bitMask < 0x01<<4; bitMask <<= 1){
//     if (bitMask<<4 & byte_split){second_4b |= bitMask;}
//   }
// }
