#include <stdlib.h>
#include <stdio.h>

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