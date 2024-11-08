#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE_ALL_DATA_FIELDS 20

__uint16_t data_source_id(__uint8_t sac, __uint8_t sic);
__uint32_t measured_position_polar(__uint32_t rho, __uint32_t theta);
__uint8_t radial_doppler_speed_prim_sub();
__uint16_t radial_doppler_speed_calc_doppler_speed(__uint16_t cal);
__uint8_t *radial_doppler_speed(__uint16_t cal);
__uint8_t *time_of_day(__uint32_t time);

struct FSPEC {
    __uint8_t FSPEC[3];
};

struct RECORD_PT {
    struct FSPEC fspec;
    __uint8_t data_fields[12];
};

struct DATABLOCK_PT {
    __uint8_t cat;
    __uint16_t len;
    struct RECORD_PT record;
};

__uint16_t data_source_id(__uint8_t sac, __uint8_t sic) {
    return (sac << 8) | sic;
}

__uint32_t measured_position_polar(__uint32_t rho, __uint32_t theta) {
    return (rho << 16) | (theta & 0xffff);
}

__uint8_t radial_doppler_speed_prim_sub() {
    __uint8_t cal = 1;
    __uint8_t rds = 0;
    __uint8_t fx = 1;
    return (cal << 7) | (rds << 6) | fx;
}

__uint16_t radial_doppler_speed_calc_doppler_speed(__uint16_t cal) {
    __uint8_t d = 0;
    return (d << 15) | cal;
}

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

__uint8_t *time_of_day(__uint32_t time) {
    __uint8_t *result = (__uint8_t *) malloc(3 * sizeof(__uint8_t));
    if (!result) {
        perror("Failed to allocate memory for time_of_day");
        exit(EXIT_FAILURE);
    }
    result[0] = time >> 16;
    result[1] = time >> 8;
    result[2] = time;
    return result;
}

struct FSPEC *create_FSPEC() {
    struct FSPEC *fspec = (struct FSPEC *) malloc(sizeof(struct FSPEC));
    if (!fspec) {
        perror("Failed to allocate memory for FSPEC");
        exit(EXIT_FAILURE);
    }
    fspec->FSPEC[0] = 0x91;
    fspec->FSPEC[1] = 0x01;
    fspec->FSPEC[2] = 0xA0;
    return fspec;
}

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

    printf("Data Source ID: %x\n", data_source_id);
    printf("Measured Position Polar: %x\n", measured_position_polar);

    record->data_fields[0] = (__uint8_t)(data_source_id >> 8);
    record->data_fields[1] = (__uint8_t)(data_source_id & 0xFF);
    record->data_fields[2] = (__uint8_t)(measured_position_polar >> 24);
    record->data_fields[3] = (__uint8_t)(measured_position_polar >> 16);
    record->data_fields[4] = (__uint8_t)(measured_position_polar >> 8);
    record->data_fields[5] = (__uint8_t)(measured_position_polar & 0xFF);
    record->data_fields[6] = radial_doppler_speed[0];
    record->data_fields[7] = radial_doppler_speed[1];
    record->data_fields[8] = radial_doppler_speed[2];
    record->data_fields[9] = time_of_day[0];
    record->data_fields[10] = time_of_day[1];
    record->data_fields[11] = time_of_day[2];

    for (int i = 0; i < 12; i++) {
        printf("Data Field[%d]: %x\n", i, record->data_fields[i]);
    }
    
    return record;
}

struct DATABLOCK_PT *create_datablock_proposedTranslator(__uint16_t data_source_id, __uint32_t measured_position_polar, __uint8_t *radial_doppler_speed, __uint8_t *time_of_day) {
    struct DATABLOCK_PT *datablock = (struct DATABLOCK_PT *) malloc(sizeof(struct DATABLOCK_PT));
    if (!datablock) {
        perror("Failed to allocate memory for DATABLOCK_PT");
        exit(EXIT_FAILURE);
    }
    
    datablock->cat = 0x30;  // Set category to 48
    datablock->len = sizeof(struct DATABLOCK_PT);
    
    struct RECORD_PT *record = create_record_proposedTranslator(data_source_id, measured_position_polar, radial_doppler_speed, time_of_day);
    memcpy(&(datablock->record), record, sizeof(struct RECORD_PT));
    free(record);
    
    return datablock;
}
