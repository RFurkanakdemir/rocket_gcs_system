#include "tesseractGCS.h"


void setup()
{

    // COM9
    Serial.begin(19200);


    int state = radio.begin(883.0, 250.0, 7, 5, 0xAA, 20, 6, 1);
    if (state == RADIOLIB_ERR_NONE) 
    {
         Serial.println(F("RadioLib success init!"));
    } 
    else 
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
    }

    radio.setDio0Action(setFlag);

    // start listening for LoRa packets
    Serial.print(F("[SX1278] Starting to listen ... "));
    state = radio.startReceive();
    if (state == RADIOLIB_ERR_NONE) 
    {
        Serial.println(F("success!"));
    } else 
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);   
    }
    /*
    state = radio.setCurrentLimit(240);
    if (state == RADIOLIB_ERR_NONE) {
      Serial.println(F("success!"));
    } else 
    {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);   
    }
    */


}


// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;
volatile bool allreadyinInterrupt = false;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  // check if the interrupt is enabled
    if(!enableInterrupt) 
    {
        allreadyinInterrupt = true;
      return;
    }

    // we got a packet, set the flag
    receivedFlag = true;
}




Variables_t         variable = { .telemTimer = 0 , .firstinit = true  , .u8_buffer = { 0 } , .u8_counter = 0};
//HYI_t               hyi_data;
GcsPaket_t          gcsStructPaket;
payloadDataPaket_t  payload_data;
//roketDataPaket_t    roket_data;
//roketDataPaket_t    roket_data_second;
BoolFlags_t         booleanVariables;
HYI_PAYLOAD_t       hyi_payload_data;


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







void loop()
{
    if ( variable.firstinit )
    {
        initDataPaket( &gcsStructPaket , &hyi_payload_data , GCS_NODE );
        variable.firstinit  = false;
        memset( &booleanVariables , 0  , sizeof( BoolFlags_t ) );
        variable.telemTimer = millis();
        //Serial.print("first init ok");
    }

    //uint8_t i=0;
    if ( receivedFlag ) 
    {
        // disable the interrupt service routine while
        // processing the data
        enableInterrupt = false;

        // reset flag
        receivedFlag = false;

        

        // you can also read received data as byte array
        /*
        byte byteArr[8];
        int state = radio.readData(byteArr, 8);
        */

        int state = radio.readData(variable.u8_buffer, 0);
        //Serial.println(variable.u8_buffer[i],HEX);
        //i++;
        if (state == RADIOLIB_ERR_NONE) 
        {
            // packet was successfully received
            //Serial.println(F("[SX1278] Received packet!"));

            // // print data of the packet
             //Serial.print(F("[SX1278] Data Node From :\t\t"));
             //Serial.println(variable.u8_buffer[ 4 ],HEX);

            int8_t isValidatedIDX = parseData();
            //Serial.println(isValidatedIDX);
            if ( isValidatedIDX != -1 )
            {
                uint8_t identifierIDX = isValidatedIDX + 4;
                switch ( variable.u8_buffer[ identifierIDX ] ) 
                {
                    case ROCKET_NODE:
                        {   /*
                            memcpy( roket_data.u8_array , variable.u8_buffer+isValidatedIDX , sizeof( roketDataPaket_t ) );
                            booleanVariables.incomingData   = ROCKET_NODE;
                            booleanVariables.isAllDataCame  += 1;
                            veriPaketle( &hyi_data , roket_data.u8_array );
                            Serial.print("[DT] - [N1] = ");
                            //Serial.println(millis() - n1Timer);
                            //n1Timer = millis();
                            */
                            //Serial.println("come rocket node");
                        }
                        break;
                    case ROCKET_NODE_2:
                        {   /*
                            memcpy( roket_data_second.u8_array , variable.u8_buffer+isValidatedIDX , sizeof( roketDataPaket_t ) );
                            booleanVariables.incomingData   = ROCKET_NODE_2;
                            booleanVariables.isAllDataCame  += 1;
                            veriPaketle( &hyi_data , roket_data_second.u8_array );
                            //Serial.print("[DT] - [N2] ");
                            //Serial.println(millis() - n2Timer);
                            //n2Timer = millis();
                            */
                            //Serial.println("come rocket node 2");
                        }
                        break;
                    case PAYLOAD_NODE:
                        {
                            memcpy( payload_data.u8_array , variable.u8_buffer+isValidatedIDX , sizeof( payloadDataPaket_t ) );
                            booleanVariables.incomingData   = PAYLOAD_NODE;
                            booleanVariables.isAllDataCame  += 1;
                            veriPaketle( &hyi_payload_data , payload_data.u8_array );
                            //Serial.print("[DT] - [P] ");
                            //Serial.println(millis() - pTimer);
                            //pTimer = millis();
                        }
                        break;
                    default:
                        break;
                }
            }
            else
            {
                //Serial.println("INCOMING MESSAGE DON'T MATCH WITH 5254FFFF");
                /*
                Serial.println("*******************************************************");
                for( uint8_t i = 0 ; i < 255 ; i++ )
                {
                    Serial.print("IDX ");
                    Serial.print(i);
                    Serial.print("Data: ");
                    if ( variable.u8_buffer[i] == ROCKET_NODE || variable.u8_buffer[i] == ROCKET_NODE_2 || variable.u8_buffer[i] == PAYLOAD_NODE )
                    {
                        Serial.print("***->>> ");
                    }
                    Serial.println(variable.u8_buffer[i],HEX);
                }
                Serial.println("*******************************************************");
                */
            }
        } 
        else if (state == RADIOLIB_ERR_CRC_MISMATCH) 
        {
            // packet was received, but is malformed
            //Serial.println(F("[SX1278] CRC error!"));

        } 
        else 
        {
            // some other error occurred
            //Serial.print(F("[SX1278] Failed, code "));
            //Serial.println(state);

        }

        
        // put module back to listen mode
        radio.startReceive();

        // we're ready to receive more packets,
        // enable interrupt service routine
        enableInterrupt = true;
    }
    
    if ( booleanVariables.isAllDataCame >= 1 )
    {
        hyi_payload_data.u8_CRC = check_sum_hesapla( hyi_payload_data.u8_array , 4 , sizeof( HYI_PAYLOAD_t )-3 );
        verileriYolla( hyi_payload_data.u8_array , sizeof( HYI_PAYLOAD_t ) );
        hyi_payload_data.u8_PaketSayisi += 1;
        booleanVariables.isAllDataCame = 0;
        memset( variable.u8_buffer , 0 , sizeof( variable.u8_buffer ) );

        //Serial.println("Data has been sended..");
    }




}
