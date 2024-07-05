if True:
    print("True")

# not false = True
if not False:
    print("not false")

if False:
    print("False")

#++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++++++++++++++++

#Comparisons- if a statement is true it will stop, use "else" as a catch all

if 1 < 1:
    print("1 < 1")
elif 1 <= 1:
    print("1 <= 1")
    print("here")
else:
    print("else 1")
#++++++++++++++++++++++++++++++++++++++++++++++++++
#++++++++++++++++++++++++++++++++++++++++++++++++++
if 1 < 1:
    print("1 < 1")
elif 1 < 1:
    print(" 1<= 1")
elif 2 < 2:
    print("2 <= 2")

#--------------------------------------------

# Comparing Conditions
if 1 > 0 and 0 < 1:
    print("ITs true ")
else:
    print("and is a nope")

if 1 < 0 or 0 < 1 :
    print("if this or that")

else:
    print(" OR nope")

#one of these options must be ture
if (1 > 0 and 0 < 1) or 1 == 0 :
    print("both ops look good")
else:
    print(" nah to comined")

# iternary operator
#Example 1
if 1 >= 1:
    print("1 >= 1")
else:
    print("1 < 1")

#written in itenrnary short
if 0 < 1: print("0 < 1")
print("1 >= 1") if 1 >= 1 else print("1 < 1")

#----------------
#Example2

if 0 < 0:
    print("1")
elif 0 > 1:
    print("2")
else:
    print("3")


# short form
print("1") if 0 > 1 else print("2") if 0 < 1 else print("3")

