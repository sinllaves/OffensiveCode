#Booleans&Operators

print(10 % 3)
print(10 * 3)
print(10 ** 3)

#Algebra
x = 10

print(x)

x = x + 1

print(x)

# Alt Versions of Alg. example from above

x += 1
print(x)

x -= 1
print(x)

x *= 5
print(x)

x /= 5
print(x)

### Binary and BitWise 

x = 13
y = 5

#Print Binaries
print(bin(x))

#Binaries with right adjust and removing 0b from
#the start
print(bin(x)[2:].rjust(4,"0"))
print(bin(y)[2:].rjust(4,"0"))

# 13 (Bitwise)AND 5 = 5   Compares bit 13 to bit 5
print(bin(x & y)[2:].rjust(4,"0"))

# 13 (Bitwise) OR 5 = 5
print(bin(x | y)[2:].rjust(4,"0"))

# Bitwise shift
y = 13
print(bin(y >> 1)[2:].rjust(4,"0"))

