import ctypes

# Define the FSPEC structure
class FSPEC(ctypes.Structure):
    _fields_ = [("FSPEC", ctypes.c_uint8 * 3)]

# Define the RECORD structure
class RECORD(ctypes.Structure):
    _fields_ = [
        ("fspec", FSPEC),
        ("data_fields", ctypes.c_uint8 * 20)
    ]

# Define the DATABLOCK structure
class DATABLOCK(ctypes.Structure):
    _fields_ = [
        ("cat", ctypes.c_uint8),
        ("len", ctypes.c_uint16),
        ("record", RECORD)
    ]

# Decoding function
def decode_datablock(datablock):
    if not datablock:
        print("Datablock is None")
        return
    
    print(f"Category: 0x{datablock.cat:02X}")
    print(f"Length: {datablock.len}")

    record = datablock.record

    # Decode FSPEC
    print(f"FSPEC: 0x{record.fspec.FSPEC[0]:02X} 0x{record.fspec.FSPEC[1]:02X} 0x{record.fspec.FSPEC[2]:02X}")

    # Decode data fields
    data_source_id = (record.data_fields[0] << 8) | record.data_fields[1]
    print(f"Data Source ID: 0x{data_source_id:04X}")

    calc_position_cart_coord = (
        (record.data_fields[2] << 24) |
        (record.data_fields[3] << 16) |
        (record.data_fields[4] << 8) |
        record.data_fields[5]
    )
    print(f"Position (Cartesian Coordinates): 0x{calc_position_cart_coord:08X}")

    height_measured_3d_radar = (record.data_fields[6] << 8) | record.data_fields[7]
    print(f"Height Measured by 3D Radar: 0x{height_measured_3d_radar:04X}")

    track_number = (record.data_fields[8] << 8) | record.data_fields[9]
    print(f"Track Number: 0x{track_number:04X}")

    track_status_prime = record.data_fields[10]
    print(f"Track Status Prime: 0x{track_status_prime:02X}")

    track_status_extent = record.data_fields[11]
    print(f"Track Status Extent: 0x{track_status_extent:02X}")

    calc_track_velocity_polar_coord = (
        (record.data_fields[12] << 24) |
        (record.data_fields[13] << 16) |
        (record.data_fields[14] << 8) |
        record.data_fields[15]
    )
    print(f"Track Velocity (Polar Coordinates): 0x{calc_track_velocity_polar_coord:08X}")

    track_quality = (
        (record.data_fields[16] << 24) |
        (record.data_fields[17] << 16) |
        (record.data_fields[18] << 8) |
        record.data_fields[19]
    )
    print(f"Track Quality: 0x{track_quality:08X}")


if __name__ == "__main__":
    # Example usage with ctypes to call the C function (assuming you've loaded the shared library)
    from ctypes import CDLL
    my_lib = CDLL("/home/n0va/Documents/Thales/ASTERIX/usefulFunc.so")
    my_lib.create_datablock.restype = ctypes.POINTER(DATABLOCK)

    # Create a DATABLOCK instance (example data)
    data_source_id = 0x0504
    calc_position_cart_coord = 0x12345678
    height_measured_3d_radar = 0x1234
    track_number = 0x5678
    track_status_prime = 0x12
    track_status_extent = 0x34
    calc_track_velocity_polar_coord = 0x87654321
    track_quality = 0x89ABCDEF

    # Assuming create_datablock is the function to create a datablock
    datablock_ptr = my_lib.create_datablock(data_source_id, calc_position_cart_coord, height_measured_3d_radar, track_number, track_status_prime, track_status_extent, calc_track_velocity_polar_coord, track_quality)

    # Decode the datablock
    decode_datablock(datablock_ptr.contents)
