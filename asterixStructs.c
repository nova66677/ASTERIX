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

__uint16_t data_source_id(__uint8_t sac, __uint8_t sic) {
    return (sac << 8) | sic;
}

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

__uint16_t mode3_code_oct_repr(__uint16_t v, __uint16_t g, __uint16_t l, __uint16_t mode3) {
    return (v << 15) | (g << 14) | (l << 13) | mode3;
}

__uint16_t mode3_code_conf_ind(char quality[12]) {
    __uint16_t result = 0;
    for (int i = 0; i < 12; i++) {
        result = result | (quality[i] << (11 - i));
    }
    return result;
}

__uint16_t flight_level_bin_repr(__uint16_t v, __uint16_t g, __uint16_t flight_level) { // Not sure as flight_level is coded on 14 bits
    return (v << 15) | (g << 14) | flight_level;
}

__uint32_t modec_code_conf_ind(__uint32_t v, __uint32_t g, __uint32_t modec, __uint32_t quality[12]) {
    __uint32_t result = 0;
    for (int i = 0; i < 12; i++) {
        result = result | (quality[i] << (11 - i));
    }
    return (v << 31) | (g << 30) | (modec << 16) | result;
}

__uint16_t height_measured_3d_radar(__uint16_t height) {
    return height ^ 0xC000; // Force bits 16 and 15 to be null
}

__uint8_t radial_doppler_speed_prim_sub(__uint8_t cal, __uint8_t rds, __uint8_t fx) { // Primary subfield, maybe followed by another subfiled, see FX value
    return (cal << 7) | (rds << 6) | fx;
}

__uint16_t radial_doppler_speed_calc_doppler_speed(__uint8_t d, __uint16_t cal) {
    return (d << 15) | cal;
}


__uint8_t *radial_doppler_speed_raw_doppler_speed(__uint8_t rep, __uint16_t dop, __uint16_t amb, __uint16_t frq) {
    __uint8_t result[7] = malloc(7);
    result[0] = rep;
    result[1] = dop;
    result[3] = amb;
    result[5] = frq;
    return result;
}

// radar plot char : too long to code now, skip

__uint8_t *time_of_day(__uint8_t time[3]) {
    return time;
}

__uint16_t track_number(__uint16_t trck_nb) {
    return trck_nb;
}

//track status, 2 parts

__uint8_t track_status_prime(__uint8_t cnf, __uint8_t rad, __uint8_t dou, __uint8_t mah, __uint8_t cdm, __uint8_t fx)  {
    return (cnf << 7) | (rad << 6) | (dou << 4) | (mah << 3) | (cdm << 2) | fx;
}

__uint8_t track_status_extent(__uint8_t tre, __uint8_t gho, __uint8_t sup, __uint8_t tcc, __uint8_t fx) {
    return (tre << 7) | (gho << 6) | (sup << 5) | (tcc << 4) | fx;
}

__uint32_t calc_track_velocity_polar_coord(__uint16_t groundspeed, __uint16_t heading) {
    return (groundspeed << 16) | heading;
}

__uint32_t track_quality(__uint8_t sigmaX, __uint8_t sigmaY, __uint8_t sigmaV, __uint8_t sigmaH) {
    return (sigmaX << 24) | (sigmaY << 16) | (sigmaV << 8) | sigmaH;
}

__uint8_t *aircraft_address(__uint32_t modeS_adress) {
    __uint8_t *result = malloc(3);
    result[0] = (modeS_adress >> 16) & 0xFF;
    result[1] = (modeS_adress >> 8) & 0xFF;
    result[2] = modeS_adress & 0xFF;
    return result;
}

__uint16_t comm_acas_capability_flight_status(__uint8_t com, __uint8_t stat, __uint8_t si, __uint8_t mssc, __uint8_t arc, __uint8_t aic, __uint8_t b1a, __uint8_t b1b) {
    __uint16_t result =  (com << 15) | (stat << 12) | (si << 9) | (mssc << 7) | (arc << 6) | (aic << 5) | (b1a << 4) | b1b;
    result &= 0xFEFF; // We put 0 in bit 9
    return result;
}


__uint8_t *aircraft_id(__uint8_t character[8] ) { // Don't know how to code on 6 bits
    __uint8_t *result = malloc(6);

}

__uint8_t *acas_resolution_advisory_report( __uint8_t acasra[7]) {
    __uint8_t *result = malloc(7);
    for (int i = 0; i < 7; i++) {
        result[i] = acasra[i];
    }
    return result;
}

