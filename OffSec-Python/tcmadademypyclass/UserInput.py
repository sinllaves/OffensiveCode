# program will hang until user iput
#test = input()
#print(test)


test = input("Enter the IP:")
print(test)

while True:
    test = input("Enter the IP: ")
    print(>>> "{}".format(test))
    if test == "exit":    # for user to break out
        break
    else:
        print("exploiting..")

