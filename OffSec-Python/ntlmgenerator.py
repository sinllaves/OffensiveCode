import hashlib,binascii
import os
def inputs():
        print("-----------------------")
        s = input("Enter input  ") 
        print (s)

        hash = hashlib.new('md4', s.encode('utf-16le')).digest()
        print ("NT hash::")
        print (binascii.hexlify(hash))
        #os.system("pause")
        inputs()
inputs()
