###### Spire Radar Translator ######

# The purpose of this program is to convert the output of the Spire radar to ASTERIX format
# 

# This archive is composed of the following files : 
# - proposedTranslator.py : This is the main program that performs the translation
# - usefulFuncTranslator.c : Contains datablock encoding functions and architecture of ASTERIX
# - usefulFuncTranslator.so : Shared librarie for both python files
# - decodingproposedTranslator.py : Used to decode a datablock into readable data

# proposedTranslator.py contains the structure of C class and one function that take the input data coming from the spire radar in JSON format and output these data as an ASTERIX datablock.
# The __main__ section is just an example of how the create_datablock_from_json use the C functions from the shared librarie. 


# To encode some data, just use proposedTranslator.py, modify the __main__ section, and you'll get 
# an ASTERIX packet
