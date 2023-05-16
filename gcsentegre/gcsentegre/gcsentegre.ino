#include "tesseractGCS.h"
#define E32_TTL_1W
#define FREQUENCY_868
#include "LoRa_E32.h"
#include <HardwareSerial.h>

HardwareSerial Seriallora(2);

LoRa_E32 e32ttl100(&Seriallora, 15, 2,4); //  AUX M0 M1
//void printParameters(struct Configuration configuration) ;//active conf

void setup()
{
  Serial.begin(115200); //19200

  // Startup all pins and UART
  e32ttl100.begin();
  
  Seriallora.begin(9600,SERIAL_8N1,16,17);
  
  //Set and get lora config 
  ResponseStructContainer c;
  c = e32ttl100.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;
  configuration.ADDL = 3;
  configuration.ADDH = 0;
  configuration.CHAN = 0x1F;
  configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
  configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;

  configuration.OPTION.fec = FEC_1_ON;
  configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
  configuration.OPTION.transmissionPower = POWER_30;

  configuration.SPED.airDataRate =  AIR_DATA_RATE_010_24; //AIR_DATA_RATE_101_192;
  configuration.SPED.uartBaudRate = UART_BPS_9600; // UART_BPS_19200;
  configuration.SPED.uartParity = MODE_00_8N1;

  e32ttl100.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE); //gcs conf okey
  //printParameters(configuration);//write to screen conf parameters
  c.close();
  
  //Set and get lora config 
  e32ttl100.setMode(MODE_0_NORMAL);

}

Variables_t         variable = { .telemTimer = 0 , .firstinit = true  , .u8_buffer = { 0 } , .u8_counter = 0};
HYI_t               hyi_data;
GcsPaket_t          gcsStructPaket;
//payloadDataPaket_t  payload_data;
roketDataPaket_t    roket_data;
roketDataPaket_t    roket_data_second;
BoolFlags_t         booleanVariables;



typedef struct
{
  uint8_t startIDX;
  uint8_t endIDX;
  uint8_t isFinded;
  uint8_t isStartIDX_VALID;
    
}parser_t;

parser_t parser = {.startIDX = 0  , .endIDX = 0 , .isFinded = 0 , .isStartIDX_VALID = 0};


int8_t parseData( )
{
  memset( &parser , 0 , sizeof( parser ) );

  for ( uint8_t i = 0 ; i < sizeof( variable.u8_buffer ) -4  ; i++ )
  {
    if ( variable.u8_buffer[ i ] == 0xFF &&  variable.u8_buffer[ i+1 ] == 0xFF && variable.u8_buffer[ i+2 ] == 0x54 && variable.u8_buffer[ i+3 ] == 0x52 )
    {
      parser.startIDX = i;
      parser.isStartIDX_VALID = true;
      parser.isFinded = true;
      break;
    }
  }


  
  if ( parser.isStartIDX_VALID )
  {
    for ( uint8_t i = parser.startIDX ; i < 253 ; i++ )
    {
      if ( variable.u8_buffer[ i ] == 0x0D && variable.u8_buffer[ i+1 ] == 0x0A )
      {
        parser.endIDX = i;
        parser.isFinded = true;
      }
    }
  }
  

  if ( parser.isFinded )
  {
    
    return parser.startIDX;
  }
  return -1; 

}



uint32_t pTimer   = 0;
uint32_t n1Timer  = 0;
uint32_t n2Timer  = 0;

uint16_t available_data = 0;
uint8_t arrCT=0;

void loop()
{
  

  if ( variable.firstinit )
  {
      initDataPaket( &gcsStructPaket , &hyi_data , GCS_NODE );
      variable.firstinit  = false;
      memset( &booleanVariables , 0  , sizeof( BoolFlags_t ) );
      variable.telemTimer = millis();
      //Serial.print("first init ok");
  }

  if ( Seriallora.available() > 0 ) 
  {
      
      //Serial.print("****************************\n");
      //Serial.println("seriallora activated\n");
      //Serial.print("****************************\n")
      variable.u8_counter = 0;
      while(Seriallora.available() > 0)
      {
        variable.u8_buffer[ variable.u8_counter++ ] = (uint8_t)Seriallora.read();
      }
  
          int8_t isValidatedIDX = parseData();
          if ( isValidatedIDX != -1 )

          {
              uint8_t identifierIDX = isValidatedIDX + 4;
              switch ( variable.u8_buffer[ identifierIDX ] ) 
              {
                  case ROCKET_NODE:
                      {
                          memcpy( roket_data.u8_array , variable.u8_buffer+isValidatedIDX , sizeof( roketDataPaket_t ) );
                          booleanVariables.incomingData   = ROCKET_NODE;
                          booleanVariables.isAllDataCame  += 1;
                          veriPaketle( &hyi_data , roket_data.u8_array );
                          //parser.isStartIDX_VALID = false;
                          //parser.isFinded = false;
                          //Serial.print("[DT] - [N1] = ");
                          //Serial.println(millis() - n1Timer);
                          //Serial.print("\n rocket node data parse okey\n");
                      
                          //n1Timer = millis();
                      }
                      break;
                  case ROCKET_NODE_2:
                      {
                          memcpy( roket_data_second.u8_array , variable.u8_buffer+isValidatedIDX , sizeof( roketDataPaket_t ) );
                          booleanVariables.incomingData   = ROCKET_NODE_2;
                          booleanVariables.isAllDataCame  += 1;
                          veriPaketle( &hyi_data , roket_data_second.u8_array );
                          //parser.isStartIDX_VALID = false;
                          //parser.isFinded = false;
                          //Serial.print("[DT] - [N2] ");
                          //Serial.println(millis() - n2Timer);
                          //n2Timer = millis();
                          //Serial.print("\n rocket node2 data parse okey\n");
                      }
                      break;
                      /*
                  case PAYLOAD_NODE:
                      {
                          memcpy( payload_data.u8_array , variable.u8_buffer+isValidatedIDX , sizeof( payloadDataPaket_t ) );
                          booleanVariables.incomingData   = PAYLOAD_NODE;
                          booleanVariables.isAllDataCame  += 1;
                          veriPaketle( &hyi_data , payload_data.u8_array );
                          parser.isStartIDX_VALID = false;
                          parser.isFinded = false;
                          //Serial.print("[DT] - [P] ");
                          //Serial.println(millis() - pTimer);
                          //pTimer = millis();
                      }
                      break;*/
                  default:
                      break;
              }
          }
  }  
        
        


  
  if ( booleanVariables.isAllDataCame >= 1 )
  {
    hyi_data.u8_CRC = check_sum_hesapla( hyi_data.u8_array , 4 , sizeof( HYI_t )-3 );
    verileriYolla( hyi_data.u8_array , sizeof( HYI_t ) );
    //Serial.println(sizeof( HYI_t ));
    hyi_data.u8_PaketSayisi += 1;
    booleanVariables.isAllDataCame = 0;
    memset( variable.u8_buffer , 0 , sizeof( variable.u8_buffer ) );
  }
  
  
}
