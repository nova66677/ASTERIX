from ctypes import *
import ctypes 
# Let's create a data block in ASTERIX encoding

so_file = "/home/n0va/Documents/Thales/ASTERIX/usefulFunc.so"
func = CDLL(so_file)

# Creation of data source id
sac = 5
sic = 4

func.data_source_id.restype = ctypes.c_uint16
func.data_source_id.argtypes = [ctypes.c_uint8, ctypes.c_uint8]
data_source_id = func.data_source_id(sac, sic)

# Creation of calc_position_cart_coord

X = 5
Y = 5
calc_position_cart_coord = func.calc_position_cart_coord(X, Y)

# Creation of height_measured_3d_radar

height = 5
height_measured_3d_radar = func.height_measured_3d_radar(height)

# Creation of track_number

track_number = 5
track_number = func.track_number(track_number)

# Creation of track_status

cnf = 0
rad = 0
dou = 0
mah = 0
cdm = 0
tre = 0
mah = 0
gho = 0
sup = "no"
tcc = 0

track_status_prime = func.track_status_prime(cnf, rad, dou, mah, cdm, 1)  # FX is set to 1 because of track_status_extend presence
track_status_extent = func.track_status_extent(tre, gho, sup, tcc, 0)  # FX is set to 0 because there is no other track_status

# Creation of calc_track_velocity_polar_coord

heading = 5
ground_speed = 5
calc_track_velocity_polar_coord = func.calc_track_velocity_polar_coord(ground_speed, heading)

# Creation of track_quality
sigmaX = 5
sigmaY = 5
sigmaV = 5
sigmaH = 5

track_quality = func.track_quality(sigmaX, sigmaY, sigmaV, sigmaH)


# define your FSPEC structure
class fspecStruct(ctypes.Structure):
    _fields_ = [('FSPEC', ctypes.c_uint8 * 3)]


# define your record structure
class recordStruct(ctypes.Structure):
    _fields_ = [('fspec', fspecStruct),
                ('data_fields', ctypes.c_uint8*20)] # SIZE_ALL_DATA_FIELDS


# define your datablock structure
class datablockStruct(ctypes.Structure):
    _fields_ = [('cat', ctypes.c_uint8),
                ('len', ctypes.c_uint8),
                ('record', recordStruct)]


################################################## Test zone on FSPEC struct #####################################################

fspecPtr = ctypes.POINTER(fspecStruct)

# next, define the interface to your functions:
func.create_FSPEC.restype = fspecPtr
func.create_FSPEC.argtypes = [] # void, no arguments


fspec = func.create_FSPEC()
for i in range(3):
    print("FSPEC", i, ' : ', hex(fspec.contents.FSPEC[i]))
print("\n")


################################################## Test zone for record Struct #################################################

recordPtr = ctypes.POINTER(recordStruct)

# next, define the interface to your functions:
func.create_record.restype = recordPtr
func.create_record.argtypes = [ctypes.c_uint16, ctypes.c_uint32, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint32, ctypes.c_uint32] # void, no arguments


record = func.create_record(data_source_id, calc_position_cart_coord, height_measured_3d_radar, track_number, track_status_prime, track_status_extent, calc_track_velocity_polar_coord, track_quality)
for i in range(3):
    print("record.fspec[",i,"] :", hex(record.contents.fspec.FSPEC[i]))


################################################## Test zone for datablock struct ##############################################

datablockPtr = ctypes.POINTER(datablockStruct)

# next, define the interface to your functions:
func.create_datablock.restype = datablockPtr
func.create_datablock.argtypes = [ctypes.c_uint16, ctypes.c_uint32, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint32, ctypes.c_uint32] # void, no arguments


datablock = func.create_datablock(data_source_id, calc_position_cart_coord, height_measured_3d_radar, track_number, track_status_prime, track_status_extent, calc_track_velocity_polar_coord, track_quality)
# Lets's see if the data correspond to what we want :)

print("CAT : ", datablock.contents.cat)
print("LEN : ", datablock.contents.len)
print("record : ")
for i in range(3):
    print("record.fspec : ", hex(datablock.contents.record.fspec.FSPEC[i]))
print("")
for i in range(20):
    print("record.data_fields[",i,"] : ", hex(datablock.contents.record.data_fields[i]))

print("\n")
############################################### Decoding the datablock ############################################################

from decodingAsterix import * 

decode_datablock(datablock.contents)
