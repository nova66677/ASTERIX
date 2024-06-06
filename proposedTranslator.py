from ctypes import *
import json
# Load the shared library
so_file = "/home/n0va/Documents/Thales/ASTERIX/usefulFuncTranslator.so"
func = CDLL(so_file)

# Define the data types for the FSPEC, RECORD_PT, and DATABLOCK_PT structures
class FSPEC(Structure):
    _fields_ = [('fields', c_uint8 * 3)]  # Adjust size as needed

class RECORD_PT(Structure):
    _fields_ = [('fspec', FSPEC),
                ('data_fields', c_uint8 * 12)]  # Adjust size as needed

class DATABLOCK_PT(Structure):
    _fields_ = [('cat', c_uint8),
                ('len', c_uint8),
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

# Function to create a datablock from a JSON file
def create_datablock_from_json(data):
    content = data['Content']['tgt']
    time_stamp_str = content['TimeStamp']
    time_of_day_seconds = (int(time_stamp_str[:2]) * 3600 + 
                           int(time_stamp_str[2:4]) * 60 + 
                           int(time_stamp_str[4:6]))
    time_of_day_ptr = func.time_of_day(time_of_day_seconds)

    divisor = content['Divisor']

    for tgt in content['data']:
        data_source_id = func.data_source_id(5, 4)  # Example values for SAC and SIC

        range_ = tgt['Range']
        azimuth = tgt['Azimuth']
        speed = tgt['Speed']
        
        measured_position_polar = func.measured_position_polar(range_, azimuth)
        radial_doppler_speed_ptr = func.radial_doppler_speed(speed)
        
        # Create the data block
        datablock = func.create_datablock_proposedTranslator(data_source_id, measured_position_polar, radial_doppler_speed_ptr, time_of_day_ptr)

        # Print or handle the datablock 
        print("CAT:", datablock.contents.cat)
        print("LEN:", datablock.contents.len)
        print("record:")
        print("FSPEC:", hex(datablock.contents.record.fspec.fields[0]))
        for i in range(12):
            print(f"data_fields[{i}]:", hex(datablock.contents.record.data_fields[i]))

        # Free allocated memory
        func.free(radial_doppler_speed_ptr)
        func.free(datablock)

    func.free(time_of_day_ptr)

    return datablock

if __name__ == "__main__":

    sac = 5
    sic = 4
    data_source_id = func.data_source_id(sac, sic)

    rho = 5
    theta = 5
    measured_position_polar = func.measured_position_polar(rho, theta)

    speed = 5
    radial_doppler_speed_ptr = func.radial_doppler_speed(speed)

    heure = 5
    minutes = 5
    secondes = 5
    time = heure * 3600 + minutes * 60 + secondes
    time_of_day_ptr = func.time_of_day(time)

    # Create the data block
    datablock = func.create_datablock_proposedTranslator(data_source_id, measured_position_polar, radial_doppler_speed_ptr, time_of_day_ptr)

    # Check the created data block
    print("CAT:", datablock.contents.cat)
    print("LEN:", datablock.contents.len)
    print("record:")
    print("FSPEC:", hex(datablock.contents.record.fspec.fields[0]))
    for i in range(12):
        print(f"data_fields[{i}]:", hex(datablock.contents.record.data_fields[i]))

    # Free allocated memory
    func.free(radial_doppler_speed_ptr)
    func.free(time_of_day_ptr)
    func.free(datablock)