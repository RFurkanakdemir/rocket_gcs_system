#include "inttypes.h"
//#include <RadioLib.h>
#include "Arduino.h"
typedef struct Roket
{
    float   irtifa;
    float   roket_gps_irtifa;
    float   roket_gps_enlem;
    float   roket_gps_boylam;
    float   gyro_x;
    float   gyro_y;
    float   gyro_z;
    float   acc_x;
    float   acc_y;
    float   acc_z;
    float   angle;
    uint8_t status;

}__attribute__( ( packed ) ) Roket_t;


typedef struct Payload
{
  
    float   payload_gps_irtifa;
    float   payload_gps_enlem;
    float   palyoad_gps_boylam;
    float   payload_nem;
    float   payload_basinc;
    float   payload_sicaklik;
    float   accelz1;
    float   accelz2;

}__attribute__( ( packed ) ) Payload_t;



typedef struct AllData
{
    float irtifa;
    float roket_gps_irtifa;
    float roket_gps_enlem;
    float roket_gps_boylam;
    float yuk_gps_irtifa;
    float yuk_gps_enlem;
    float yuk_gps_boylam;
    float kademe_gps_irtifa;
    float kademe_gps_enlem;
    float kademe_gps_boylam;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float acc_x;
    float acc_y;
    float acc_z;
    float angle;
    uint8_t status;
    
}__attribute__( ( packed ) ) AllData_t;





enum NODE_INFORMATION
{
    ROCKET_NODE     = 0xAA,
    ROCKET_NODE_2   = 0xAB,
    PAYLOAD_NODE    = 0xBB,
    GCS_NODE        = 0xCC
};

typedef union roketDataPaket
{

    struct
    {
        uint32_t        u32_start_header; // 4
        uint8_t         u8_node_information;  // 1
        uint8_t         u8_package_length;    // 1
        Roket_t         data;         
        uint8_t         u8_crc_data;          // 1
        uint16_t        u16_end_header;   // 2
    }__attribute__( ( packed ) );

    uint8_t u8_array[ sizeof( Roket_t ) + ( 4 + 1 + 1 + 1 + 2 ) ];
}roketDataPaket_t;


typedef union payloadDataPaket
{

    struct
    {
        uint32_t        u32_start_header; // 4
        uint8_t         u8_node_information;  // 1
        uint8_t         u8_package_length;    // 1
        Payload_t       data;         
        uint8_t         u8_crc_data;          // 1
        uint16_t        u16_end_header;   // 2
    }__attribute__( ( packed ) );

    uint8_t u8_array[ sizeof( Payload_t ) + ( 4 + 1 + 1 + 1 + 2 ) ];
}payloadDataPaket_t;


typedef union HYI
{
    struct
    {
        uint32_t    u32_UNUSED; // Ilk 4 byte Hakemler tarafÄ±ndan belirlendi.
        uint8_t     u8_ID;      // 1
        uint8_t     u8_PaketSayisi; // 1
        Roket_t     rocket_node;
        Roket_t     rocket_node_2;
        //Payload_t   payload_data;
        uint8_t     u8_CRC;     // 1
        uint16_t    u16_UNUSED; // Son 2 byte hakemler tarafÄ±ndan belirlendi.
        
    }__attribute__( ( packed ) );
    
     //uint8_t u8_array[  sizeof( Roket_t )*2 + sizeof( Payload_t ) + ( 4 + 1 + 1 + 1 + 2 )  ]; //123byte falan
     uint8_t u8_array[  sizeof( Roket_t )*2 + ( 4 + 1 + 1 + 1 + 2 )  ];
}HYI_t;



typedef struct Variables
{
    uint32_t    telemTimer;
    uint8_t     firstinit;
    uint8_t     u8_buffer[ 255 ];
    uint8_t     u8_counter;
}Variables_t;


enum GCS_COMMANDS
{
    RESEND_TELEMETRY = 0xBA
};

typedef struct GcsPaket
{
    uint32_t        u32_start_header; // 4
    uint8_t         u8_node_information;  // 1
    uint8_t         u8_package_length;    // 1
    uint8_t         u8_package_receiver;
    uint8_t         u8_command;
    uint16_t        u16_end_header;   // 2
}__attribute__( ( packed ) ) GcsPaket_t;


typedef struct BoolFlags
{
    uint8_t incomingData;
    uint8_t isHaveErrorCRC;
    uint8_t isAllDataCame;
}BoolFlags_t;


uint8_t check_sum_hesapla( const uint8_t * const ptVeri , const uint8_t START_IDX , const uint8_t END_IDX );
void veriPaketle ( HYI_t * const pkt , const uint8_t * const data );
void verileriYolla (  uint8_t * ptVeri , const uint8_t veriLength );
void initDataPaket( GcsPaket_t * const pkt , HYI_t * const ptHYI ,const uint8_t WHICH_NODE );


//extern SX1276 radio;
