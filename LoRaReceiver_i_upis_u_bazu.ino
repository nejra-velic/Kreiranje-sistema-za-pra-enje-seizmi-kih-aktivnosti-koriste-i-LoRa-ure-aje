#include <SPI.h>
#include <LoRa.h>
#include <Ethernet.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
double prvaVr,drugaVr,trecaVr;
String query,StrLon,StrLat,StrJac,StrMsg;

IPAddress server_addr(192,168,200,104);  // IP od MySQL servera
char user[] = "bmt";              // MySQL login username
char password[] = "najjaci";        // MySQL login password

char INSERT_SQL[150] = "INSERT INTO podaci (Longituda,Latituda,Jacina) VALUES (1,1,1)";

EthernetClient client;
MySQL_Connection conn((Client *)&client);
MySQL_Cursor *cur_mem;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
    while (!Serial); 
  Ethernet.begin(mac_addr);
  Serial.println("Connecting...");
  if (conn.connect(server_addr, 3306, user, password)) {
    cur_mem = new MySQL_Cursor(&conn);
    delay(1000);
    Serial.println("slusam");
     
  }
  else
    Serial.println("Connection failed.");
}
void upisiUBazu(String prvaVr, String drugaVr , String trecaVr){
    query ="INSERT INTO projekt.podaci (Longituda, Latituda, Jacina) VALUES ('" + String(prvaVr) + "','" + String(drugaVr) + "','" + String(trecaVr) +"')";
    Serial.println(query);
    query.toCharArray(INSERT_SQL, 150);
    cur_mem->execute(INSERT_SQL);
  }

void loop() {
  // parsieanje paketa
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    
    Serial.print("Primljeni paket '");
   
    StrMsg = "";
    // čitanje paketa
    while (LoRa.available()) {
    
      StrMsg = StrMsg + String((char)LoRa.read());
    }
    Serial.println(StrMsg);
    if(StrMsg.indexOf("Ubrzanje: ") >= 0){
         StrMsg.replace("Ubrzanje: ","");
         StrJac = StrMsg;
    }
        if(StrMsg.indexOf("Longitude: ") >= 0){
         StrMsg.replace("Longitude: ","");
         StrLon = StrMsg;
         upisiUBazu(StrLon,StrLat,StrJac);
          StrLon = "";
    StrLat = "";
    StrJac = "";
    }
            if(StrMsg.indexOf("Latitude: ") >= 0){
         StrMsg.replace("Latitude: ","");
         StrLat = StrMsg;
         
    }
    
  Serial.println(StrJac);
  Serial.println(StrLon);
  Serial.println(StrLat);
  }
}
