#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//FSPEC : 10010001 00000001 10100000
#define DATA_SOURCE_IDENTIFIER 1 // Needed for json structure encoding
#define TARGET_REPORT_DESCRIPTOR 2
#define WARNERR_COND_TGT_CLASSIF 3
#define MEASURED_POSITION_SPC 4 // Needed for json structure encoding
#define CALC_POSITION_CART_COORD 5 //
#define MODE2_CODE_OCT_REPR 6
#define MODE1_CODE_OCT_REPR 7
#define MODE2_CODE_CONF_IND 8
#define MODE1_CODE_CONF_IND 9
#define MODE3_CODE_OCT_REPR 10
#define MODE3_CODE_CONF_IND 11
#define FLIGHT_LEVEL_BIN_REPR 12
#define MODEC_CODE_CONF_IND 13
#define HEIGHT_MEASURED_3D_RADAR 14 //
#define RADIAL_DOPPLER_SPEED 15 // Needed for json structure encoding
#define RADAR_PLOT_CHAR 16 //
#define TIME_DAY 17 // Needed for json structure encoding
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

//////////////////// Function definition for creating usefull Data Item ///////////////////////////////////////

__uint16_t data_source_id(__uint8_t sac, __uint8_t sic) { // ok, we have args, and it works
    return (sac << 8) | sic;
}

__uint32_t calc_position_cart_coord(__uint32_t X, __uint32_t Y) { // We have to calculate them but ok, it works
    return (X << 16) | Y;
}

__uint16_t height_measured_3d_radar(__uint16_t height) { // ok, it works
    return height & 0x3FFF; // Force bits 16 and 15 to be null
}

__uint16_t track_number(__uint16_t trck_nb) { // I have to ask what it is, it works too
    return trck_nb;
}

// radar plot char : too long to code now, skip, and optional :)

__uint8_t track_status_prime(__uint8_t cnf, __uint8_t rad, __uint8_t dou, __uint8_t mah, __uint8_t cdm, __uint8_t fx)  {
    return (cnf << 7) | (rad << 6) | (dou << 4) | (mah << 3) | (cdm << 2) | fx;
}

__uint8_t track_status_extent(__uint8_t tre, __uint8_t gho, __uint8_t sup, __uint8_t tcc, __uint8_t fx) {
    return (tre << 7) | (gho << 6) | (sup << 5) | (tcc << 4) | fx;
}

__uint32_t calc_track_velocity_polar_coord(__uint16_t groundspeed, __uint16_t heading) { // ok, just convert
    return (groundspeed << 16) | heading;
}

__uint32_t track_quality(__uint8_t sigmaX, __uint8_t sigmaY, __uint8_t sigmaV, __uint8_t sigmaH) { // I have to ask
    return (sigmaX << 24) | (sigmaY << 16) | (sigmaV << 8) | sigmaH;
}



//////////////////////////////////// Functions definition for proposed translator ////////////////////////////////////////////////:

// Data_source_id already coded

__uint32_t measured_position_polar(__uint32_t rho, __uint32_t theta) {
    return (rho << 16) | theta;
}

__uint8_t radial_doppler_speed_prim_sub() { // Primary subfield, maybe followed by another subfiled, see FX value
    __uint8_t cal = 1;
    __uint8_t rds = 0;
    __uint8_t fx = 1;
    return (cal << 7) | (rds << 6) | fx;
}

__uint16_t radial_doppler_speed_calc_doppler_speed(__uint16_t cal) {
    __uint8_t d = 0;
    return (d << 15) | cal;
}

// I add a function using both radial_doppler_speed for making data item craft easier

__uint8_t *radial_doppler_speed(__uint16_t cal) {
    __uint8_t *result = (__uint8_t *) malloc(3 * sizeof(__uint8_t));
    if (!result) {
        perror("Failed to allocate memory for radial_doppler_speed");
        exit(EXIT_FAILURE);
    }
    result[0] = radial_doppler_speed_prim_sub();
    __uint16_t tmp = radial_doppler_speed_calc_doppler_speed(cal);
    result[1] = tmp >> 8;
    result[2] = tmp & 0xFF;
    return result;
}


__uint8_t time_of_day(__uint8_t time) {
    return time;
}

////////////////////////////////////////////// FSPEC definition and creation /////////////////////////////////////////////////////


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


///////////////////////////////////// Record definition : struct and creation ////////////////////////////////////////////////


struct RECORD {
    struct FSPEC fspec;
    __uint8_t data_fields[SIZE_ALL_DATA_FIELDS];
};


// This function args are already formed dataItems, so no need to input the dataItems parameters by hand in this function

struct RECORD *create_record(__uint16_t data_source_id, __uint32_t calc_position_cart_coord, __uint16_t height_measured_3d_radar, __uint16_t track_number, __uint8_t track_status_prime, __uint8_t track_status_extent, __uint32_t calc_track_velocity_polar_coord, __uint32_t track_quality) { // A record is composed of a FSPEC, and some data fields
    struct RECORD *record = (struct RECORD *) malloc(sizeof(struct RECORD));
    memset(record, 0, sizeof(struct RECORD));

    // We add the fspec to our record
    struct FSPEC *fspec = create_FSPEC();
    record->fspec = *fspec;

    // We now add our data fields to our record
    //Adding data_source_id to record 
    record->data_fields[0] = (data_source_id >> 8) & 0xFF;//data_source_id;
    record->data_fields[1] = data_source_id & 0xFF;
    // Adding calc_position_cart_coord to record
    record->data_fields[2] = (calc_position_cart_coord >> 24) & 0xFF;
    record->data_fields[3] = (calc_position_cart_coord >> 16) & 0xFF;
    record->data_fields[4] = (calc_position_cart_coord >> 8) & 0xFF;
    record->data_fields[5] = calc_position_cart_coord & 0xFF;

    // Adding height_measured_3d_radar to record
    record->data_fields[6] = (height_measured_3d_radar >> 8) & 0xFF;
    record->data_fields[7] = height_measured_3d_radar & 0xFF;

    // Adding track_number to record
    record->data_fields[8] = (track_number >> 8) & 0xFF;
    record->data_fields[9] = track_number & 0xFF;

    // Adding track_status_prime to record
    record->data_fields[10] = track_status_prime;

    // Adding track_status_extent to record
    record->data_fields[11] = track_status_extent;

    // Adding calc_track_velocity_polar_coord to record
    record->data_fields[12] = (calc_track_velocity_polar_coord >> 24) & 0xFF;
    record->data_fields[13] = (calc_track_velocity_polar_coord >> 16) & 0xFF;
    record->data_fields[14] = (calc_track_velocity_polar_coord >> 8) & 0xFF;
    record->data_fields[15] = calc_track_velocity_polar_coord & 0xFF;

    // Adding track_quality to record
    record->data_fields[16] = (track_quality >> 24) & 0xFF;
    record->data_fields[17] = (track_quality >> 16) & 0xFF;
    record->data_fields[18] = (track_quality >> 8) & 0xFF;
    record->data_fields[19] = track_quality & 0xFF;

    return record;
}

////////////////////////////////////////////// Datablock definition and creation //////////////////////////////////////////////


struct DATABLOCK {
    __uint8_t cat;  // The CATEGORY of ASTERIX
    __uint8_t len;  // length of the datablock, including cat and len
    struct RECORD record;  // I try with one record only, then I will modify to 5 for instance
};

struct DATABLOCK *create_datablock(__uint16_t data_source_id, __uint32_t calc_position_cart_coord, __uint16_t height_measured_3d_radar, __uint16_t track_number, __uint8_t track_status_prime, __uint8_t track_status_extent, __uint32_t calc_track_velocity_polar_coord, __uint32_t track_quality) {

    struct DATABLOCK *datablock = (struct DATABLOCK *) malloc(sizeof(struct DATABLOCK));
    datablock->cat = 0x30;
    datablock->len = sizeof(struct RECORD) + sizeof(char) + sizeof(__uint16_t);
    datablock->record = *create_record(data_source_id, calc_position_cart_coord, height_measured_3d_radar, track_number, track_status_prime, track_status_extent, calc_track_velocity_polar_coord, track_quality);

    return datablock;
}


///////////////////////////////////////// Datablock creation for proposedTranslator.py //////////////////////////////////////

// Define the RECORD_PT structure
struct RECORD_PT {
    struct FSPEC fspec;
    __uint8_t data_fields[9];  // Adjust the size based on actual data fields
};

// Define the DATABLOCK_PT structure
struct DATABLOCK_PT {
    __uint8_t cat;  // The CATEGORY of ASTERIX
    __uint8_t len;  // Length of the datablock, including cat and len
    struct RECORD_PT record;  // Using one record for now
};


// Function to create RECORD_PT
struct RECORD_PT *create_record_proposedTranslator(__uint16_t data_source_id, __uint32_t measured_position_polar, __uint8_t *radial_doppler_speed, __uint8_t *time_of_day) {
    struct RECORD_PT *record = (struct RECORD_PT *) malloc(sizeof(struct RECORD_PT));
    if (!record) {
        perror("Failed to allocate memory for RECORD_PT");
        exit(EXIT_FAILURE);
    }
    memset(record, 0, sizeof(struct RECORD_PT));
    
    struct FSPEC *fspec = create_FSPEC();
    record->fspec = *fspec;
    free(fspec);
    
    record->data_fields[0] = (__uint8_t)(data_source_id >> 8);
    record->data_fields[1] = (__uint8_t)(data_source_id & 0xFF);
    record->data_fields[2] = (__uint8_t)(measured_position_polar >> 24);
    record->data_fields[3] = (__uint8_t)(measured_position_polar >> 16);
    record->data_fields[4] = (__uint8_t)(measured_position_polar >> 8);
    record->data_fields[5] = (__uint8_t)(measured_position_polar & 0xFF);
    record->data_fields[6] = radial_doppler_speed[0];
    record->data_fields[7] = radial_doppler_speed[1];
    record->data_fields[8] = time_of_day[0];
    
    return record;
}

// Function to create DATABLOCK_PT
struct DATABLOCK_PT *create_datablock_proposedTranslator(__uint16_t data_source_id, __uint32_t measured_position_polar, __uint8_t *radial_doppler_speed, __uint8_t *time_of_day) {
    struct DATABLOCK_PT *datablock = (struct DATABLOCK_PT *) malloc(sizeof(struct DATABLOCK_PT));
    if (!datablock) {
        printf("Failed to allocate memory for DATABLOCK_PT");
        exit(EXIT_FAILURE);
    }
    
    datablock->cat = 0x30;  // Set category to 48
    datablock->len = sizeof(struct DATABLOCK_PT);
    
    struct RECORD_PT *record = create_record_proposedTranslator(data_source_id, measured_position_polar, radial_doppler_speed, time_of_day);
    memcpy(&(datablock->record), record, sizeof(struct RECORD_PT));
    free(record);
    
    return datablock;
}