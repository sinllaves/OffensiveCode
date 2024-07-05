#number iteration, each dighit plus one

a = 1
print(a)
a += 1
print(a)

# while - keeps iterating until something is true, always pay attention to exit criteria 

a = 1
while a < 5:
    a += 1
    print(a)

# For Loop - works for a set amount

for i in [0, 1, 2, 3, 4]:
    print(i+6)

print("---------")

# range 5 is the same as 0-4 list
for i in range(5):
    print(i+6)

#nested loop
for i in range(3):
    for j in range(3):
        print(i,j)

# Loop Control statements
# BREAK stops loop at

for i in range(5):
    if i ==2:
        break
    print(i)

print("-----")

# Continue    - skip a number that is in iteration
for i in range(5):
    if i == 2:
        continue
    print(i)

print("-----")

# PaSS -- ?
for i in range(5):
    if i == 2:
        pass
    print(i)
print("-----")


# Iterate over string
for c in "string":
    print(c)

#Iteratate dicktionary key pairs

#for key, value in {"a":1, "b":2, "c":3}.items:
#   print(key, value)




