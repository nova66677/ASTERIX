#include <stdlib.h>
#include <stdio.h>

#define DATA_SOURCE_IDENTIFIER 1 //
#define TARGET_REPORT_DESCRIPTOR 2
#define WARNERR_COND_TGT_CLASSIF 3
#define MEASURED_POSITION_SPC 4
#define CALC_POSITION_CART_COORD 5 //
#define MODE2_CODE_OCT_REPR 6
#define MODE1_CODE_OCT_REPR 7
#define MODE2_CODE_CONF_IND 8
#define MODE2_CODE_CONF_IND 9
#define MODE3_CODE_OCT_REPR 10
#define MODE3_CODE_CONF_IND 11
#define FLIGHT_LEVEL_BIN_REPR 12
#define MODEC_CODE_CONF_IND 13
#define HEIGHT_MEASURED_3D_RADAR 14 //
#define RADIAL_DOPPLER_SPEED 15
#define RADAR_PLOT_CHAR 16 //
#define TIME_DAY 17
#define TRACK_PLOT_NUMBER 18 //
#define TRACK_STATUS 19 //
#define CALC_TRACK_VELOCITY_POLAR 20 //
#define TRACK_QUALITY 21 //
#define AIRCRAFT_ADDRESS 22
#define COMM_ACAS_CAPABILITY_FLIGHT_STATUS 23
#define AIRCRAFT_ID 24
#define MODE_S_MB_DATA 25
#define ACAS_RESOLUTION_ADVISORY_REPORT 26

#define SIZE_ALL_DATA_FIELDS 20 // Static size, as we know which data fields will be in each of our records

// Function definition for creating usefull Data Item

__uint16_t data_source_id(__uint8_t sac, __uint8_t sic) {
    return (sac << 8) | sic;
}

__uint32_t calc_position_cart_coord(__uint32_t X, __uint32_t Y) {
    return (X << 16) | Y;
}

__uint16_t height_measured_3d_radar(__uint16_t height) {
    return height ^ 0xC000; // Force bits 16 and 15 to be null
}

__uint16_t track_number(__uint16_t trck_nb) {
    return trck_nb;
}

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

// FSPEC definition

// We need 4 FSPEC (bytes) to encode DATA FIELD's presence (7 Data Field per byte, one FX bit)
// For this version, I suppose the field presence is constant, so we only need 3 bytes, as no field are set on the last byte

struct FSPEC { // We create 3 bytes to store the presence of our data fields
    __uint8_t FSPEC[3];
};

struct FSPEC *create_FSPEC() { // We can statically indicate the presence of our data field, as they are always the same
    struct FSPEC *fspec = (struct FSPEC *) malloc(sizeof(struct FSPEC));
    fspec->FSPEC[0] = 0x89;
    fspec->FSPEC[1] = 0x01;
    fspec->FSPEC[2] = 0x5E;
    return fspec;
}

struct RECORD {
    struct FSPEC *fspec;
    __uint8_t data_fields[SIZE_ALL_DATA_FIELDS];
};

__uint8_t fill_data_fields() {

}

struct RECORD *create_record() { // A record is composed of a FSPEC, and some data fields
    struct RECORD *record = (struct RECORD *) malloc(sizeof(struct RECORD));

    // We add the fspec to our record
    struct FSPEC *fspec = create_FSPEC();
    record->fspec = fspec;

    // We now add our data fields to our record
    record->data_fields[0] = fill_data_fields();

    return record;
}

struct DATABLOCK {
    char cat;  // The CATEGORY of ASTERIX
    __uint8_t len;  // length of the datablock, including cat and len
    struct RECORD *record;  // I try with one record only, then I will modify to 5 for instance
};

struct DATABLOCK *create_datablock() {
    struct DATABLOCK *datablock = (struct DATABLOCK *) malloc(sizeof(struct DATABLOCK));
    datablock->cat = 0x30;
    datablock->len = sizeof(struct RECORD) + sizeof(char) + sizeof(__uint16_t);
    datablock->record = create_record();
    return datablock;
}
