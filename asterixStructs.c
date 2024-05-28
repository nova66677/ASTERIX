#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_SOURCE_IDENTIFIER 1
#define TARGET_REPORT_DESCRIPTOR 2
#define WARNERR_COND_TGT_CLASSIF 3
#define MEASURED_POSITION_SPC 4
#define CALC_POSITION_CART_COORD 5
#define MODE2_CODE_OCT_REPR 6
#define MODE1_CODE_OCT_REPR 7
#define MODE2_CODE_CONF_IND 8
#define MODE2_CODE_CONF_IND 9
#define MODE3_CODE_OCT_REPR 10
#define MODE3_CODE_CONF_IND 11
#define FLIGHT_LEVEL_BIN_REPR 12
#define MODEC_CODE_CONF_IND 13
#define HEIGHT_MEASURED_3D_RADAR 14
#define RADIAL_DOPPLER_SPEED 15
#define RADAR_PLOT_CHAR 16
#define TIME_DAY 17
#define TRACK_PLOT_NUMBER 18
#define TRACK_STATUS 19
#define CALC_TRACK_VELOCITY_POLAR 20
#define TRACK_QUALITY 21
#define AIRCRAFT_ADDRESS 22
#define COMM_ACAS_CAPABILITY_FLIGHT_STATUS 23
#define AIRCRAFT_ID 24
#define MODE_S_MB_DATA 25
#define ACAS_RESOLUTION_ADVISORY_REPORT 26



typedef struct { //  This struct is at the good format, on 2 bytes
    char SAC;
    char SIC
} data_source_id;

typedef struct { // too long to code now, incomplete
    char TYP;
    char SIM;
    char RDP;
    char SPI;
    char RAB;
    char FX;
    __uint8_t result;
} tgt_report_descriptor;

typedef struct { // bits 8-2 : Code ; bit 1 : FX 
    char code;
    char FX;
    char result;
} warnerr_cond_tgt_classif;

typedef struct {
    int result;
} measured_position_polar;

__uint32_t measured_position_polar(__uint32_t rho, __uint32_t theta) {
    return (rho << 16) | theta;
}

__uint32_t calc_position_cart_coord(__uint32_t X, __uint32_t Y) {
    return (X << 16) | Y;
}

__uint16_t mode2_code_oct_repr(__uint16_t v, __uint16_t g, __uint16_t l, __uint16_t mode2 ) {
    return (v << 15) | (g << 14) | (l << 13) | mode2;
}

__uint8_t mode1_code_oct_repr(__uint8_t v, __uint8_t g, __uint8_t l, __uint8_t mode1) {
    return (v << 7) | (g << 6) | (l << 5) | mode1;
}

__uint16_t mode2_code_conf_ind(char quality[12]) { // quality represent the quality pulse at index i
    __uint16_t result = 0;
    for (int i = 0; i < 12; i++) {
        result = result | (quality[i] << (11 - i));
    }
    return result;
}

__uint8_t mode1_code_conf_ind(char quality[5]) {
    __uint8_t result = 0;
    for (int i = 0; i < 5; i++) {
        result = result | (quality[i] << (4 - i));
    }
    return result;
}