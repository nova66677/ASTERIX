from ctypes import *
from scapy.all import wrpcap, Raw, Ether, IP, UDP
import socket
import struct

# Load the shared library
so_file = "/home/n0va/Documents/Thales/ASTERIX/usefulFuncTranslator.so"
func = CDLL(so_file)

# Define the data types for the FSPEC, RECORD_PT, and DATABLOCK_PT structures
class FSPEC(Structure):
    _fields_ = [('fields', c_uint8 * 3)]

class RECORD_PT(Structure):
    _fields_ = [('fspec', FSPEC),
                ('data_fields', c_uint8 * 12)]

class DATABLOCK_PT(Structure):
    _fields_ = [('cat', c_uint8),
                ('len', c_uint16),
                ('record', RECORD_PT)]

# Define return types and argument types for the C functions
func.data_source_id.restype = c_uint16
func.data_source_id.argtypes = [c_uint8, c_uint8]

func.measured_position_polar.restype = c_uint32
func.measured_position_polar.argtypes = [c_uint32, c_uint32]

func.radial_doppler_speed.restype = POINTER(c_uint8)
func.radial_doppler_speed.argtypes = [c_uint16]

func.time_of_day.restype = POINTER(c_uint8)
func.time_of_day.argtypes = [c_uint32]

func.create_datablock_proposedTranslator.restype = POINTER(DATABLOCK_PT)
func.create_datablock_proposedTranslator.argtypes = [c_uint16, c_uint32, POINTER(c_uint8), POINTER(c_uint8)]

# Create a function to create the packet
def create_packet():
    sac = 0xCA
    sic = 0xFE
    data_source_id = func.data_source_id(sac, sic)

    rho = 0xBEEF
    theta = 0xDEAD
    measured_position_polar = func.measured_position_polar(rho, theta)

    cal = 0xDEAD
    radial_doppler_speed_ptr = func.radial_doppler_speed(cal)
    radial_doppler_speed = bytes(radial_doppler_speed_ptr[:3])

    time = 0xDEADBEEF
    time_of_day_ptr = func.time_of_day(time)
    time_of_day = bytes(time_of_day_ptr[:3])

    datablock_ptr = func.create_datablock_proposedTranslator(data_source_id, measured_position_polar, radial_doppler_speed_ptr, time_of_day_ptr)
    datablock = datablock_ptr.contents

    # Free allocated memory for radial_doppler_speed and time_of_day
    func.free(radial_doppler_speed_ptr)
    func.free(time_of_day_ptr)

    # Prepare the packet using Scapy
    eth = Ether(dst="ff:ff:ff:ff:ff:ff", src="00:0c:29:9d:2a:c3", type=0x0800)
    ip = IP(src="192.168.1.2", dst="192.168.1.255", ttl=64, proto="udp")
    udp = UDP(sport=15000, dport=47806)

    payload = bytes(datablock)
    packet = eth / ip / udp / Raw(load=payload)

    return packet

# Write the packet to a pcap file
packet = create_packet()
wrpcap('datablock.pcap', [packet])
