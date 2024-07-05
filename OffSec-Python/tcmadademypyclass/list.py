# can change after is has been defined and can be combined (unlike tuples)
# list use the []

list1 = ["a", "b","c",]
print(list1)

list2 = ["a", 1, 1.2, ["a"], [], list(), ("S")]
print(list2)

# indexs of list

print(list1[0])
print(list1[-1])

# index of list in a list
print(list2[3][0])

#change list after it has been defined
list1[0] = "X"
print(list1)

#delete item in list
del list1[0]
print(list1)

# Replace in list
list1.insert(0, "A")
print(list1)

list1 = ["A"] + list1
print(list1)

# even if you append the list, you are not changing it
list1.append("G")
print(list1)

#index of list

print(max(list1))
print(min(list1))

#index 
print(list1.index("c"))
print(list1[list1.index("c")])

#reverse list
list1.reverse()
print(list1)

# Slice list in reverse
list1 = list1[::-1]
print(list1)

#Count #s of A's in list

print(list1.count("A"))

#"pop" can remove and return last item in the list

list1.pop()
print(list1)


#Extend list
list3 = ["H", "J", "K"]
print(list3)

list1.extend(list3)
print(list1)

# clear list
list1.clear()
print(list1)

#SORT

list4 = [1, 2, 45, 78, 47, 4]
print(list4)

list4.sort()
print(list4)

list4.sort(reverse=True)
print(list4)

list5 = list4
print(list4)
print(list5)

list5[2] = "X"
print(list5)
print(list4)

# COPY

list6 = list4.copy()
print(list4)
print(list6)

list6[2] = "A"
print(list6)
print(list4)

#map = use map to map a function to an item in the list

list7 = ["1", "2", "3"]
print(list7)

list8 = list(map(float, list7))
print(list8)

