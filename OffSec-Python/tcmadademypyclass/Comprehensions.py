# list comprehensions provide shorter syntax to create a list create a shorter list based on some iterable.
# newlist = 
# [expression for item in iterable if condition == True]




#Print a list
print("----EX 1---")


list1 = [ "a", "b", "c", "d"]
print(list1)


# apply/return expression (x) for x (x = ea item in list)
print("----EX 2---")


list2 = [x for x in list1]
print(list2)


# do expression for item in list if condition is True
print("----EX 3---")


list3 = [x for x in list1 if x == 'a']
print(list3)



#range() function to create an iterable
print("----EX 4 ---")

list4 = [x for x in range(5)]
print(list4)



# change values to hex as its added to list
print("----EX 5 ---")

list5 = [hex(x) for x in range(5)]
print(list5)

print("-------")

# add in an iterary 
print("----EX 6 ---")

list6 = [hex(x) if x > 0 else "X" for x in range(5)]
print(list6)

print("-------")

# multiply each item by itsself
print("----EX 7 ---")


list7 = [x * x for x in range(5)]
print(list7)

print("-------")

#expand on simple conditionals
print("----EX 8 ---")

list8 =[ x for x in range(5) if x == 0 or x == 1]
print(list8)

print("-------")

# nested expressions
print("----EX 9 ---")

list9 = [[1,2,3],[4,5,6],[7,8,9]]
print(list9)

print("-------")


# make a list of items from another lists nested items
print("----EX 10 ---")

list10 = [y for x in list9 for y in x]
print(list10)

# comprehensions can apply to ranges and sets
print("----EX 11 ---")


set1 = {x + x for x in range(5)} # add ea # to itself 
print(set1)

# make a list of characters from string ('s','t','r','i','n','g')
print("----EX 12 ---")

list11 = [c for c in "string"]
print(list11)

# Join items together
print("----EX 13 ---")

print("".join(list11))
print("-".join(list11))


#
print("----EX 14 ---")

list12 = []
for c in "string":
    list12.append(c)
print(list12)
