# try-catch block
# "try" test code block
# "Except" block catchs error
# "Finally" block  executes regardles off the outcomes from testing the blocks

# different errors result in different exceptions which can be handled with 
# different catchs

# exceptions - is an error during run time execution that halts the pro
# numeric / indentation / syntax errors


# generic example, Exception w/~ verbose debug will fire if not able
try:
    asdf
except:
    print("the file des not exist")
print("++++++++++++++++++++++++")
# ______________________________

try:                             
    f = open("file")            
except FileNotFoundError:       # this is a specific error catch
   print(FileNotFoundError)
except Exception as e:          # gnric excption (error) handler w/ debug info
    print(e)
finally:         # this catch all will print regardless of actual of error found
    print("this messsage!")

# ======================================================
# Raise Exception
#ex1
n =100
if n == 0:
    raise Exception("n can't be 0!")
if type(n) is not int:
    raise Exception("n must be int!")

print(1/n)
#assertion
# assertion can be used for data verification
# can be used as short if elses, with hard error checking

n = 1
assert(n !=0)    # similiar to raise ex, if assertion fails, prog stops
print(1/n)

