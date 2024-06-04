from ctypes import *

# Load the shared library
so_file = "/home/n0va/Documents/Thales/ASTERIX/usefulFuncTranslator.so"
func = CDLL(so_file)

# Define the data types for the FSPEC, RECORD_PT, and DATABLOCK_PT structures
class FSPEC(Structure):
    _fields_ = [('fields', c_uint8 * 3)]  # Adjust size as needed

class RECORD_PT(Structure):
    _fields_ = [('fspec', FSPEC),
                ('data_fields', c_uint8 * 9)]  # Adjust size as needed

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

# Define the decoding function
def decodingProposedTranslator(datablock):
    # Check the created data block
    print("CAT:", datablock.contents.cat)
    print("LEN:", datablock.contents.len)
    print("record:")
    print("FSPEC:", hex(datablock.contents.record.fspec.fields[0]))
    for i in range(9):
        print(f"data_fields[{i}]:", hex(datablock.contents.record.data_fields[i]))

# Example usage
if __name__ == "__main__":
    # Create the necessary data (similar to previous code)
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

    # Decode the data block
    decodingProposedTranslator(datablock)

    # Free allocated memory
    func.free(radial_doppler_speed_ptr)
    func.free(time_of_day_ptr)
    func.free(datablock)
