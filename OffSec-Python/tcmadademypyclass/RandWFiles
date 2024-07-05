# open file and iterate pw
f = open('top-100.txt')
print(f)

print("----")

# specify different modes of operation (look at rt or 'r' )
# options include: append, write, create, specify if txt or binary

# READ a file
f = open('top-100.txt', 'rt')
print(f)

# Read/print the real file
print(f.read())


# line by line python array print out
print(f.readlines())

# if executed again, the code already read through to end of fie and will leave an empty array
print(f.readlines())

# Reset pointer to 0 
f.seek(0)
print(f.readlines())

#we can iterate over each line of the file for bruteforce. 
# #this strips the ending off
f.seek(0)
for line in f:
    print(line.strip())

f.close()

#make writeable
# * you can not save written txt if file is opened in write mode. must
# be in append mode
f = open("test.txt", "a")

# To Write
f.write("teset line two!")
f.close()




print(f.name)
print(f.closed)
print(f.mode)

with open('rockyou.txt', encoding='latin-1') as f:
    for line in f:
        pass
