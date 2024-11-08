#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include "usefulFuncTranslator.c"

// Ethernet header length
#define ETHERNET_HEADER_LEN 14

// IP header length
#define IP_HEADER_LEN 20

// UDP header length
#define UDP_HEADER_LEN 8

// Ethernet header structure
struct ethernet_header {
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t eth_type;
};

// IPv4 header structure
struct ipv4_header {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t length;
    uint16_t id;
    uint16_t fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dest_ip;
};

// UDP header structure
struct udp_header {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
};

// Function to calculate IP checksum
uint16_t calculate_checksum(uint16_t *buf, int nwords) {
    uint32_t sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (uint16_t)(~sum);
}

void create_ethernet_header(struct ethernet_header *eth_hdr, uint8_t *dest_mac, uint8_t *src_mac, uint16_t eth_type) {
    memcpy(eth_hdr->dest_mac, dest_mac, 6);
    memcpy(eth_hdr->src_mac, src_mac, 6);
    eth_hdr->eth_type = htons(eth_type);
}

void create_ipv4_header(struct ipv4_header *ip_hdr, uint32_t src_ip, uint32_t dest_ip, uint16_t total_length) {
    ip_hdr->version_ihl = 0x45; // Version 4, IHL 5 (no options)
    ip_hdr->tos = 0;
    ip_hdr->length = htons(total_length);
    ip_hdr->id = htons(54321); // Example ID
    ip_hdr->fragment_offset = 0;
    ip_hdr->ttl = 64; // Default TTL
    ip_hdr->protocol = IPPROTO_UDP; // UDP protocol
    ip_hdr->checksum = 0;
    ip_hdr->src_ip = htonl(src_ip);
    ip_hdr->dest_ip = htonl(dest_ip);
    ip_hdr->checksum = calculate_checksum((uint16_t *)ip_hdr, IP_HEADER_LEN / 2);
}

void create_udp_header(struct udp_header *udp_hdr, uint16_t src_port, uint16_t dest_port, uint16_t data_length) {
    udp_hdr->src_port = htons(src_port);
    udp_hdr->dest_port = htons(dest_port);
    udp_hdr->length = htons(UDP_HEADER_LEN + data_length);
    udp_hdr->checksum = 0; // Optional, can be omitted
}

struct asterixPacket {
    struct ethernet_header eth_hdr;
    struct ipv4_header ip_hdr;
    struct udp_header udp_hdr;
    uint8_t data[1500];
};

void createPacket(struct asterixPacket* asterix, struct ethernet_header* eth_hdr, struct ipv4_header* ip_hdr, struct udp_header* udp_hdr, struct DATABLOCK_PT* datablock) {
    if (!asterix) {
        perror("Failed to allocate memory for asterixPacket");
        exit(EXIT_FAILURE);
    }
    memcpy(&asterix->eth_hdr, eth_hdr, sizeof(struct ethernet_header));
    memcpy(&asterix->ip_hdr, ip_hdr, sizeof(struct ipv4_header));
    memcpy(&asterix->udp_hdr, udp_hdr, sizeof(struct udp_header));
    memcpy(&asterix->data, datablock, sizeof(struct DATABLOCK_PT));
}

