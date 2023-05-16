#include "tesseractGCS.h"
#include "string.h"
#include "Arduino.h"


#define ss 18
#define rst 14
#define dio0 26
#define dio1 33



SX1276 radio = new Module(ss, dio0, rst, dio1);



uint8_t check_sum_hesapla( const uint8_t * const ptVeri , const uint8_t START_IDX , const uint8_t END_IDX )
{
    uint32_t check_sum = 0; 
    for( uint8_t i = START_IDX ; i < END_IDX ; i++ )
    {
        check_sum += ptVeri[i];
    }
    return ( uint8_t ) ( check_sum % 256 ) ;
}

void initDataPaket( GcsPaket_t * const pkt , HYI_PAYLOAD_t * const ptHYI ,const uint8_t WHICH_NODE )
{
    pkt->u32_start_header = 0x5254FFFF; // 4
    pkt->u8_node_information = WHICH_NODE;  // 1
    pkt->u8_package_length = sizeof( GcsPaket_t );    // 1
    pkt->u8_package_receiver = ROCKET_NODE ;
    pkt->u8_command = RESEND_TELEMETRY ;
    pkt->u16_end_header = 0x0A0D;   // 2


    memset( ptHYI->u8_array , 0 , sizeof ( HYI_PAYLOAD_t ) );
    ptHYI->u32_UNUSED = 0x5254FFFF; // 4
    ptHYI->u16_UNUSED = 0x0A0D;   // 2
    ptHYI->u8_ID = 12;
    ptHYI->u8_PaketSayisi = 1;
}


void verileriYolla (  uint8_t * ptVeri , const uint8_t veriLength )
{
    Serial.write( ptVeri, veriLength );
}

void veriPaketle ( HYI_PAYLOAD_t * const pkt , const uint8_t * const data )
{
    switch ( data[ 4 ] )
    {
        
        case PAYLOAD_NODE:
            {
                payloadDataPaket_t * payloadData = (payloadDataPaket_t *) data;
                memcpy( &( pkt->payload_data ), &( payloadData->data ) , sizeof( Payload_t ) );
                //Serial.println("veri paketlendi");
            }
            break;
        default:
            break;
    }
}
